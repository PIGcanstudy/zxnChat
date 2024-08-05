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

	void ClearConnections() {
		std::lock_guard<std::mutex> lock(_mutex);
		while (!_connections.empty()) {
			auto* context = _connections.front();
			redisFree(context);
			_connections.pop();
		}
	}

	redisContext* getConnection();

	void returnConnection(redisContext* context);

	void Close();
private:
	void checkThread() {
		std::lock_guard<std::mutex> lock(_mutex);
		if (_b_stop) {
			return;
		}
		auto pool_size = _connections.size();
		for (int i = 0; i < pool_size && !_b_stop; i++) {
			auto* context = _connections.front();
			_connections.pop();
			try {
				auto reply = (redisReply*)redisCommand(context, "PING");
				if (!reply) {
					std::cout << "reply is null, redis ping failed: " << std::endl;
					_connections.push(context);
					continue;
				}
				freeReplyObject(reply);
				_connections.push(context);
			}
			catch (std::exception& exp) {
				std::cout << "Error keeping connection alive: " << exp.what() << std::endl;
				redisFree(context);
				context = redisConnect(_host, _port);
				if (context == nullptr || context->err != 0) {
					if (context != nullptr) {
						redisFree(context);
					}
					continue;
				}

				auto reply = (redisReply*)redisCommand(context, "AUTH %s", _pwd);
				if (reply->type == REDIS_REPLY_ERROR) {
					std::cout << "认证失败" << std::endl;
					//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
					freeReplyObject(reply);
					continue;
				}

				//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
				freeReplyObject(reply);
				std::cout << "认证成功" << std::endl;
				_connections.push(context);
			}
		}
	}
	std::atomic_bool _b_stop;
	std::mutex _mutex;
	std::condition_variable _cond;
	std::queue<redisContext*> _connections;
	std::thread  check_thread_;
	std::size_t _pool_size;
	int _port;
	const char* _host;
	const char* _pwd;
	int counter_;
};

