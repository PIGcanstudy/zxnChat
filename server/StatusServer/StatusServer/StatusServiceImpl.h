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

// ������������Ϣ
struct ChatServer {
	std::string host;
	std::string port;
	std::string name;
	int con_count;
};

// �����̳�grpc�ķ�����
class StatusServiceImpl final : public StatusService::Service
{
public:
	StatusServiceImpl();
	// ʵ�ָ��ؾ��⣬�����������һ�����������
	// ���������ѡ��һ��ӵ�������������ķ�����
	// ����Ψһtoken���ͻ���
	Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
		GetChatServerRsp* reply) override;

	// ����������֤�û�ID��token�Ƿ���Ч
	Status Login(ServerContext* context, const LoginReq* request,
		LoginRsp* reply) override;
private:
	// �����û�token
	void insertToken(int uid, std::string token);
	// ��ȡ���������ٵ����������
	ChatServer getChatServer();
	// ����������б�
	std::unordered_map<std::string, ChatServer> _servers;
	// ���� _servers �Ļ�����
	std::mutex _server_mtx;

	// ���û��������� ������ͻ�ռ�÷����������ڴ����Կ��Ǵ���redis��
	/*
	// �û� token ӳ��
	std::unordered_map<int, std::string> _tokens;
	// ���� _tokens �Ļ�����
	std::mutex _token_mtx;
	*/
};

