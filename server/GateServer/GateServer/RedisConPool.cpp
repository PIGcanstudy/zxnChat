#include "RedisConPool.h"

RedisConPool::RedisConPool(std::size_t pool_size, const char* host, int port, const char* pwd)
	: _pool_size(pool_size), _host(host), _port(port), _pwd(pwd), _b_stop(false)
{
	for (std::size_t i = 0; i < _pool_size; i++) {

		// ��������
		auto* context = redisConnect(_host, _port);
		if (context == NULL || context->err != 0) {
			std::cerr << "redisContext ����ʧ�ܣ�" << std::endl;
			if (context != NULL) redisFree(context);
			continue;
		}

		// ����������֤
		auto reply = (redisReply*)redisCommand(context, "AUTH %s", _pwd);
		if (reply->type == REDIS_REPLY_ERROR) {
			std::cout << "��֤ʧ�ܣ� �������" << std::endl;
			freeReplyObject(reply);
			continue;
		}
		// ��֤�ɹ����ͷ��ڴ沢�������
		freeReplyObject(reply);
		std::cout << "��֤�ɹ�" << std::endl;

		// �����ӷ��������
		_connections.push(context);
	}
}

RedisConPool::~RedisConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);
	Close();
	while (_connections.size()) _connections.pop();
}

redisContext* RedisConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cond.wait(lock, [this]() {
		return !_connections.empty() || _b_stop;
		});

	if (_b_stop) {
		return nullptr;
	}

	auto* context = _connections.front();
	_connections.pop();
	return context;
}

void RedisConPool::returnConnection(redisContext* context)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_b_stop) return;
	_connections.push(context);
	_cond.notify_one();
}

void RedisConPool::Close()
{
	_b_stop.store(true);
	_cond.notify_all();
}
