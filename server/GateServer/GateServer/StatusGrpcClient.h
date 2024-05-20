#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include "const.h"
#include "Singleton.h"
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::StatusService;

class StatusConPool {
public:
	StatusConPool(std::size_t poolSize, std::string host, std::string port);
	~StatusConPool();
	std::unique_ptr<StatusService::Stub> getConnection();
	void returnConnection(std::unique_ptr<StatusService::Stub> context);
	void Close();
private:
	std::mutex _mutex;
	std::atomic_bool _b_stop;
	std::size_t _poolSize;
	std::condition_variable _cond;
	std::queue<std::unique_ptr<StatusService::Stub>> _connections;
	std::string _port;
	std::string _host;
};

class StatusGrpcClient: public Singleton<StatusGrpcClient>
{
	friend class Singleton<StatusGrpcClient>;
public:
	~StatusGrpcClient() {

	}
	GetChatServerRsp GetChatServer(int uid);
	LoginRsp Login(int uid, std::string token);
private:
	StatusGrpcClient();
	std::unique_ptr<StatusConPool> pool_;
};

