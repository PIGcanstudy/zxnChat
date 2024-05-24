#include "MySqlPool.h"

MySqlPool::MySqlPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolSize):_url(url),
	_user(user), _pass(pass), _schema(schema), _poolSize(poolSize), _b_stop(false)
{
	try {
		for (int i = 0; i < _poolSize; i++) {
			// �����������ݿ��
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			// ��ȡ����
			auto* con = driver->connect(_url, _user, _pass);

			std::cout << "���ݿ����ӳɹ�" << std::endl;
			// ���ݿ�ľ�������
			con->setSchema(_schema);
			// ��ȡʱ���
			auto currentTime = std::chrono::system_clock::now().time_since_epoch();
			// ��ʱ��ת��Ϊ��
			long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
			// ���������
			_pool.emplace(std::make_unique<SqlConnection>(con, timestamp));
		}
		// ���Ӽ���߳�
		_check_thread = std::thread([this]() {
			while (!_b_stop) {
				checkConnection();
				// ÿ��60s���һ��
				std::this_thread::sleep_for(std::chrono::seconds(60));
			}
			});
		// ���ں�̨
		_check_thread.detach();
	}
	catch (sql::SQLException& e) {
		std::cout << "mysql pool init failedm error is " << e.what() << std::endl;
	}
}

MySqlPool::~MySqlPool()
{
	Close();
	while (_pool.size()) {
		_pool.pop();
	}
}

void MySqlPool::checkConnection()
{
	std::lock_guard<std::mutex> lock(_mutex);
	int poolsize = _pool.size();
	// ��ȡ��ǰʱ���
	auto currentTime = std::chrono::system_clock::now().time_since_epoch();
	// ��ʱ���ת��Ϊ��
	long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
	// ȡ��ÿ�����ӣ���ʱ����selcet����
	for (int i = 0; i < poolsize; i++) {

		auto con = std::move(_pool.front());
		_pool.pop();

		// ����ʱ�Զ��ͷ�����
		Defer defer([this, &con]() {
			_pool.push(std::move(con));
			});

		if (timestamp - con->_last_oper_time < 5) {
			continue;
		}

		try {
			std::unique_ptr<sql::Statement> stmt(con->_con->createStatement());
			stmt->executeQuery("SELECT 1");
			con->_last_oper_time = timestamp;
			//std::cout << "execute timer alive query , cur is " << timestamp << std::endl;
		}
		catch (sql::SQLException& e) {
			std::cout << "Error keeping connection alive: " << e.what() << std::endl;
			// ���´������Ӳ��滻�ɵ�����
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			auto* newcon = driver->connect(_url, _user, _pass);
			newcon->setSchema(_schema);
			con->_con.reset(newcon);
			con->_last_oper_time = timestamp;
		}
	}
}

std::unique_ptr<SqlConnection> MySqlPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]() {
		return _b_stop.load() || !_pool.empty();
		});

	if (_b_stop.load() == true) {
		return nullptr;
	}

	std::unique_ptr<SqlConnection> con(std::move(_pool.front()));
	_pool.pop();
	return con;
}

void MySqlPool::returnConnection(std::unique_ptr<SqlConnection> con)
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (_b_stop) {
		return;
	}
	_pool.push(std::move(con));
	_cond.notify_one();
}

void MySqlPool::Close()
{
	_b_stop.store(true);
	_cond.notify_all();
}
