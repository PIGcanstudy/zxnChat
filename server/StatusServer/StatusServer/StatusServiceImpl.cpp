#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "boost/uuid/uuid_generators.hpp"

#include "const.h"
#include "RedisMgr.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

// 生成唯一字符串（UUID）
std::string generate_unique_string() {
	// 创建UUID对象
	boost::uuids::uuid uuid = boost::uuids::random_generator()();

	// 将UUID转换为字符串
	std::string unique_string = boost::uuids::to_string(uuid);

	return unique_string;
}

// 获取聊天服务器方法实现
Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply)
{
	std::string prefix("chat status server has received :  ");
	// 获取负载最少的聊天服务器
	const auto& server = getChatServer();
	reply->set_host(server.host);
	reply->set_port(server.port);
	reply->set_error(ErrorCodes::Success);
	reply->set_token(generate_unique_string());
	// 将token存入map，与用户相匹配
	insertToken(request->uid(), reply->token());
	return Status::OK;
}

// 构造函数，实现服务器列表的初始化
StatusServiceImpl::StatusServiceImpl()
{
	auto& cfg = ConfigMgr::GetInstance();

	std::vector<std::string> words;

	// 读出server列表 
	auto server_list = cfg["ChatServers"]["Name"];

	// ChatServers

	std::stringstream ss(server_list);
	std::string word;

	// 读出每个server的具体名字
	while (std::getline(ss, word, ',')) {
		words.emplace_back(word);
	}

	// 读出每个server的具体属性并存下来
	for (auto& w : words) {
		if (cfg[w]["Name"].empty()) {
			continue;
		}

		ChatServer server;
		server.name = cfg[w]["Name"];
		server.host = cfg[w]["Host"];
		server.port = cfg[w]["Port"];
		_servers[server.name] = server;
	}
}

// 获取连接数最少的聊天服务器
ChatServer StatusServiceImpl::getChatServer() {
	std::lock_guard<std::mutex> guard(_server_mtx);
	auto minServer = _servers.begin()->second;

	// 从redis中取出最新的服务器里连接客户端数量
	auto count_str = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, minServer.name);
	if (count_str.empty()) {
		//不存在则默认设置为最大
		minServer.con_count = INT_MAX;
	}
	else {
		minServer.con_count = std::stoi(count_str);
	}

	// 使用范围基于for循环
	for (auto& server : _servers) {
		if (server.second.name == minServer.name) {
			continue;
		}

		auto count_str = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server.second.name);
		if (count_str.empty()) {
			server.second.con_count = INT_MAX;
		}
		else {
			server.second.con_count = std::stoi(count_str);
		}

		if (server.second.con_count < minServer.con_count) {
			minServer = server.second;
		}
	}

	return minServer;
}

// 用户登录方法实现，验证token
Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request, LoginRsp* reply)
{
	auto uid = request->uid();
	auto token = request->token();

	std::string uid_str = std::to_string(uid);
	std::string token_key = USERTOKENPREFIX + uid_str;
	std::string token_value = "";

	bool success = RedisMgr::GetInstance()->Get(token_key, token_value);
	if (!success) {
		reply->set_error(ErrorCodes::UidInvalid);
		return Status::OK;
	}

	if (token_value != token) {
		reply->set_error(ErrorCodes::TokenInvalid);
		return Status::OK;
	}

	reply->set_error(ErrorCodes::Success);
	reply->set_uid(uid);
	reply->set_token(token);
	return Status::OK;
}

// 插入用户 token
void StatusServiceImpl::insertToken(int uid, std::string token)
{
	std::string uid_str = std::to_string(uid);
	// 拼成key值
	std::string token_key = USERTOKENPREFIX + uid_str;
	RedisMgr::GetInstance()->Set(token_key, token);
}

