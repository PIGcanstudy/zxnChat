#pragma once
#include "const.h"
#include <mysql/jdbc.h>
#include <queue>
#include <condition_variable>
class SqlConnection {
public:
	SqlConnection(sql::Connection* con, int64_t lasttime) : _con(con), _last_oper_time(lasttime) {
	}
	// ʹ������ָ���������
	std::unique_ptr<sql::Connection> _con;
	// ������¼��һ�β�����ʱ��
	// �Խ��mysql�ĳ�ʱδʹ�ö��Զ��ر����ӵ�����
	int64_t _last_oper_time;
};

class MySqlPool
{
public:
	MySqlPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolSize);
	~MySqlPool();
	// �����sql����
	void checkConnection();
	std::unique_ptr<SqlConnection> getConnection();
	void returnConnection(std::unique_ptr<SqlConnection> con);
	void Close();
private:
	// ���ݿ��·��
	std::string _url;
	// �û���
	std::string _user;
	// ����
	std::string _pass;
	// ��������ݿ�
	std::string _schema;
	// ���ӵĴ�С
	int _poolSize;
	// ��
	std::mutex _mutex;
	// ֹͣ���
	std::atomic_bool _b_stop;
	// ��������
	std::condition_variable _cond;
	// ������ӵ��߳�
	std::thread _check_thread;
	// �����洢���ӵĳ���
	std::queue<std::unique_ptr<SqlConnection>> _pool;
};

