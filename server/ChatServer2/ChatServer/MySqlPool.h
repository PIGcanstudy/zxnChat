#pragma once
#include "const.h"
#include <mysql/jdbc.h>
#include <queue>
#include <condition_variable>
class SqlConnection {
public:
	SqlConnection(sql::Connection* con, int64_t lasttime) : _con(con), _last_oper_time(lasttime) {
	}
	// 使用智能指针管理连接
	std::unique_ptr<sql::Connection> _con;
	// 用来记录上一次操作的时间
	// 以解决mysql的长时未使用而自动关闭连接的问题
	int64_t _last_oper_time;
};

class MySqlPool
{
public:
	MySqlPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolSize);
	~MySqlPool();
	// 检测连sql连接
	void checkConnection();
	std::unique_ptr<SqlConnection> getConnection();
	void returnConnection(std::unique_ptr<SqlConnection> con);
	void Close();
private:
	// 数据库的路径
	std::string _url;
	// 用户名
	std::string _user;
	// 密码
	std::string _pass;
	// 具体的数据库
	std::string _schema;
	// 池子的大小
	int _poolSize;
	// 锁
	std::mutex _mutex;
	// 停止标记
	std::atomic_bool _b_stop;
	// 条件变量
	std::condition_variable _cond;
	// 检查连接的线程
	std::thread _check_thread;
	// 用来存储连接的池子
	std::queue<std::unique_ptr<SqlConnection>> _pool;
};

