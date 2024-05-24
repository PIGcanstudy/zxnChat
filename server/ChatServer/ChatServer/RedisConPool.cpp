#include "RedisConPool.h"

RedisConPool::RedisConPool(std::size_t pool_size, const char* host, int port, const char* pwd)
	: _pool_size(pool_size), _host(host), _port(port), _pwd(pwd), _b_stop(false)
{
	for (std::size_t i = 0; i < _pool_size; i++) {

		// 创建连接
		auto* context = redisConnect(_host, _port);
		if (context == NULL || context->err != 0) {
			std::cerr << "redisContext 创建失败！" << std::endl;
			if (context != NULL) redisFree(context);
			continue;
		}

		// 进行密码认证
		auto reply = (redisReply*)redisCommand(context, "AUTH %s", _pwd);
		if (reply->type == REDIS_REPLY_ERROR) {
			std::cout << "认证失败， 密码错误" << std::endl;
			freeReplyObject(reply);
			continue;
		}
		// 认证成功就释放内存并加入队列
		freeReplyObject(reply);
		std::cout << "认证成功" << std::endl;

		// 将连接放入队列中
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
