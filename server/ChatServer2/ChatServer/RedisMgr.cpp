#include "RedisMgr.h"
#include "ConfigMgr.h"

RedisMgr::RedisMgr() {
	auto& gCfgMgr = ConfigMgr::GetInstance();
	auto host = gCfgMgr["Redis"]["Host"];
	auto port = gCfgMgr["Redis"]["Port"];
	auto pwd = gCfgMgr["Redis"]["Passwd"];
	_con_pool.reset(new RedisConPool(5, host.c_str(), atoi(port.c_str()), pwd.c_str()));
}

RedisMgr::~RedisMgr()
{
	Close();
}


//通过key值查value
bool RedisMgr::Get(const std::string& key, std::string& value)
{
	auto connection = _con_pool->getConnection();
	Defer defer([&connection, this]() {
		_con_pool->returnConnection(connection);
		});
	if (connection == NULL) return false;
	//由于源码是接收const char * 所以一定要把string转换为char * 否则会出错
	auto reply = (redisReply*)redisCommand(connection, "GET %s", key.c_str());
	if (reply == NULL) {
		std::cout << "[ GET  " << key << " ] failed" << std::endl;
		freeReplyObject(reply);
		return false;
	}

	// 如果返回的类型不是string，就说明出错了
	if (reply->type != REDIS_REPLY_STRING) {
		std::cout << "[ GET  " << key << " ] failed" << std::endl;
		freeReplyObject(reply);
		return false;
	}

	//获取值
	value = reply->str;
	freeReplyObject(reply);

	std::cout << "Succeed to execute command [ GET " << key << "  ]" << std::endl;
	return true;
}

//设置key 与 value 键值对
bool RedisMgr::Set(const std::string& key, const std::string& value) {
	auto connection = _con_pool->getConnection();
	Defer defer([&connection, this]() {
		_con_pool->returnConnection(connection);
		});
	if (connection == NULL) return false;
	//执行redis命令行
	auto reply = (redisReply*)redisCommand(connection, "SET %s %s", key.c_str(), value.c_str());

	//如果返回NULL则说明执行失败
	if (NULL == reply)
	{
		std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}

	//如果执行失败则释放连接，set如果成功了就会返回ok
	if (!(reply->type == REDIS_REPLY_STATUS && (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0)))
	{
		std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}

	//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
	freeReplyObject(reply);
	std::cout << "Execut command [ SET " << key << "  " << value << " ] success ! " << std::endl;
	return true;
}


/*
* redis 一个键值可以对应一个类似队列的东西
* 可以支持跨服务
*/

// 从左侧push数据
bool RedisMgr::LPush(const std::string& key, const std::string& value)
{
	auto connection = _con_pool->getConnection();
	Defer defer([&connection, this]() {
		_con_pool->returnConnection(connection);
		});
	if (connection == NULL) return false;
	auto reply = (redisReply*)redisCommand(connection, "LPUSH %s %s", key.c_str(), value.c_str());
	if (NULL == reply)
	{
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}

	//如果成功了 integer 返回值一定是1
	//所以如果返回<=0就表示出错了
	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}

	std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	return true;
}

// 从左侧pop数据
bool RedisMgr::LPop(const std::string& key, std::string& value) {
	auto connection = _con_pool->getConnection();
	Defer defer([&connection, this]() {
		_con_pool->returnConnection(connection);
		});
	if (connection == NULL) return false;
	auto reply = (redisReply*)redisCommand(connection, "LPOP %s ", key.c_str());
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		std::cout << "Execut command [ LPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}
	value = reply->str;
	std::cout << "Execut command [ LPOP " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	return true;
}

// 从右侧push数据
bool RedisMgr::RPush(const std::string& key, const std::string& value) {
	auto connection = _con_pool->getConnection();
	Defer defer([&connection, this]() {
		_con_pool->returnConnection(connection);
		});
	if (connection == NULL) return false;
	auto reply = (redisReply*)redisCommand(connection, "RPUSH %s %s", key.c_str(), value.c_str());
	if (NULL == reply)
	{
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}

	std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	return true;
}

// 从右侧删除数据
bool RedisMgr::RPop(const std::string& key, std::string& value) {
	auto connection = _con_pool->getConnection();
	Defer defer([&connection, this]() {
		_con_pool->returnConnection(connection);
		});
	if (connection == NULL) return false;
	auto reply = (redisReply*)redisCommand(connection, "RPOP %s ", key.c_str());
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}
	value = reply->str;
	std::cout << "Execut command [ RPOP " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	return true;
}

// redis他支持多key值，也就是支持多级key
// 这是处理二级key的代码，此种写法方便处理 字符串
bool RedisMgr::HSet(const std::string& key, const std::string& hkey, const std::string& value) {
	auto connection = _con_pool->getConnection();
	Defer defer([&connection, this]() {
		_con_pool->returnConnection(connection);
		});
	if (connection == NULL) return false;
	auto reply = (redisReply*)redisCommand(connection, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}
	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	return true;
}


// 第二种写法，通过数组的方式
// 对于二进制用此种方法会好一点
bool RedisMgr::HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen)
{
	const char* argv[4];
	size_t argvlen[4];
	argv[0] = "HSET";
	argvlen[0] = 4;
	argv[1] = key;
	argvlen[1] = strlen(key);
	argv[2] = hkey;
	argvlen[2] = strlen(hkey);
	argv[3] = hvalue;
	argvlen[3] = hvaluelen;

	auto connection = _con_pool->getConnection();
	Defer defer([&connection, this]() {
		_con_pool->returnConnection(connection);
		});
	if (connection == NULL) return false;
	auto reply = (redisReply*)redisCommandArgv(connection, 4, argv, argvlen);
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}
	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] success ! " << std::endl;
	freeReplyObject(reply);
	return true;
}

// 多级获取
std::string RedisMgr::HGet(const std::string& key, const std::string& hkey)
{
	const char* argv[3];
	size_t argvlen[3];
	argv[0] = "HGET";
	argvlen[0] = 4;
	argv[1] = key.c_str();
	argvlen[1] = key.length();
	argv[2] = hkey.c_str();
	argvlen[2] = hkey.length();
	auto connection = _con_pool->getConnection();
	Defer defer([&connection, this]() {
		_con_pool->returnConnection(connection);
		});
	if (connection == NULL) return "";
	auto reply = (redisReply*)redisCommandArgv(connection, 3, argv, argvlen);
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		freeReplyObject(reply);
		std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
		return "";
	}

	std::string value = reply->str;
	freeReplyObject(reply);
	std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
	return value;
}

bool RedisMgr::HDel(const std::string& key, const std::string& field) {
	auto connect = _con_pool->getConnection();
	if (connect == nullptr) {
		return false;
	}

	Defer defer([&connect, this]() {
		_con_pool->returnConnection(connect);
		});

	redisReply* reply = (redisReply*)redisCommand(connect, "HDEL %s %s", key.c_str(), field.c_str());
	if (reply == nullptr) {
		std::cerr << "HDEL command failed" << std::endl;
		return false;
	}

	bool success = false;
	if (reply->type == REDIS_REPLY_INTEGER) {
		success = reply->integer > 0;
	}

	freeReplyObject(reply);
	return success;
}


//删除key与value
bool RedisMgr::Del(const std::string& key)
{
	auto connection = _con_pool->getConnection();
	Defer defer([&connection, this]() {
		_con_pool->returnConnection(connection);
		});
	if (connection == NULL) return false;
	auto reply = (redisReply*)redisCommand(connection, "DEL %s", key.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ Del " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}
	std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	return true;
}

bool RedisMgr::ExistsKey(const std::string& key)
{
	auto connection = _con_pool->getConnection();
	Defer defer([&connection, this]() {
		_con_pool->returnConnection(connection);
		});
	if (connection == NULL) return false;
	auto reply = (redisReply*)redisCommand(connection, "exists %s", key.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
		std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
		freeReplyObject(reply);
		return false;
	}
	std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
	freeReplyObject(reply);
	return true;
}

void RedisMgr::Close()
{
	_con_pool->Close();
	_con_pool->ClearConnections();
}