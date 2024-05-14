#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;


// 池子
class RPConPool {
public:
	//构造池子
	RPConPool(std::string host, std::string port, std::size_t poolSize = std::thread::hardware_concurrency());
	//析构池子
	~RPConPool();
	// 从池子中取Stub
	std::unique_ptr<VarifyService::Stub> getConnection();
	// 线程用完后将stub还给池子
	void returnConnection(std::unique_ptr<VarifyService::Stub> context);

private:
	//用来标记池子是否关闭
	std::atomic_bool _b_stop;
	//池子里的大小
	size_t _poolSize;
	//主机地址
	std::string _host;
	//端口
	std::string _port;

	//队列，用来存储多个Stub，多线程从这取Stub
	std::queue<std::unique_ptr<VarifyService::Stub>> _connections;

	//用来实现互斥的锁
	std::mutex _mutex;

	//用来通知的条件变量
	std::condition_variable _cond;
};

class VarifyGrpcClient :public Singleton<VarifyGrpcClient> {
	friend class Singleton<VarifyGrpcClient>;
public:
	GetVarifyRsp GetVarifyCode(std::string& email);
private:
	VarifyGrpcClient();
	
	std::unique_ptr<RPConPool> _pool;
};
