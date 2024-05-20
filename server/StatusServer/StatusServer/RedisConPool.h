#pragma once
#include "const.h"
#include <hiredis.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
class RedisConPool
{
public:
	RedisConPool(std::size_t pool_size, const char* host, int port, const char* pwd);

	~RedisConPool();

	redisContext* getConnection();

	void returnConnection(redisContext* context);

	void Close();
private:
	std::atomic_bool _b_stop;
	std::mutex _mutex;
	std::condition_variable _cond;
	std::queue<redisContext*> _connections;
	std::size_t _pool_size;
	int _port;
	const char* _host;
	const char* _pwd;
};

