#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "boost/uuid/uuid_generators.hpp"

#include "const.h"
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
	ChatServer server;

	// 初始化第一个聊天服务器
	server.port = cfg["ChatServer1"]["Port"];
	server.host = cfg["ChatServer1"]["Host"];
	server.con_count = 0;
	server.name = cfg["ChatServer1"]["Name"];
	_servers[server.name] = server;

	// 初始化第二个聊天服务器
	server.port = cfg["ChatServer2"]["Port"];
	server.host = cfg["ChatServer2"]["Host"];
	server.name = cfg["ChatServer2"]["Name"];
	server.con_count = 0;
	_servers[server.name] = server;
}

// 获取连接数最少的聊天服务器
ChatServer StatusServiceImpl::getChatServer() {

	std::lock_guard<std::mutex> guard(_server_mtx);

	auto minServer = _servers.begin()->second;
	// 使用范围基于for循环
	for (const auto& server : _servers) {
		// 遍历服务器列表，选择连接数最少的服务器
		// con_count 是chatserver用来返回连接数量的
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
	// 保证线程安全
	std::lock_guard<std::mutex> guard(_token_mtx);

	auto iter = _tokens.find(uid);

	// 用户id无效
	if (iter == _tokens.end()) {
		reply->set_error(ErrorCodes::UidInvalid);
		return Status::OK;
	}

	// 用户token无效
	if (iter->second != token) {
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
	std::lock_guard<std::mutex> guard(_token_mtx);
	_tokens[uid] = token;
}

