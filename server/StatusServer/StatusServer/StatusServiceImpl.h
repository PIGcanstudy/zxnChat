#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include <mutex>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

// 聊天服务相关信息
struct ChatServer {
	std::string host;
	std::string port;
	std::string name;
	int con_count;
};

// 这个类继承grpc的服务器
class StatusServiceImpl final : public StatusService::Service
{
public:
	StatusServiceImpl();
	// 实现负载均衡，根据请求分配一个聊天服务器
	// 分配策略是选择一个拥有最少连接数的服务器
	// 生成唯一token给客户端
	Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
		GetChatServerRsp* reply) override;

	// 根据请求验证用户ID和token是否有效
	Status Login(ServerContext* context, const LoginReq* request,
		LoginRsp* reply) override;
private:
	// 插入用户token
	void insertToken(int uid, std::string token);
	// 获取连接数最少的聊天服务器
	ChatServer getChatServer();
	// 聊天服务器列表
	std::unordered_map<std::string, ChatServer> _servers;
	// 保护 _servers 的互斥锁
	std::mutex _server_mtx;

	// 当用户量上来后 存在这就会占用服务器大量内存所以考虑存在redis中
	/*
	// 用户 token 映射
	std::unordered_map<int, std::string> _tokens;
	// 保护 _tokens 的互斥锁
	std::mutex _token_mtx;
	*/
};

