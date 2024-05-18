#include "VarifyGrpcClient.h"
#include "ConfigMgr.h"

GetVarifyRsp  VarifyGrpcClient::GetVarifyCode(std::string& email) {
	//客户端的上下文 和 io_context类似
	ClientContext context;
	//接收的回应
	GetVarifyRsp reply;
	//发送的请求
	GetVarifyReq request;
	//设置email
	request.set_email(email);

	//得到stub
	auto stub = _pool->getConnection();

	//通过stub来调用其他服务器的接口
	Status status = stub->GetVarifyCode(&context, request, &reply);
	
	if (status.ok()) {
		//还给池子
		_pool->returnConnection(std::move(stub));
		std::cout << "grpc调用成功 "<< std::endl;
		return reply;
	}
	else {
		_pool->returnConnection(std::move(stub));
		std::cout << "grpc调用失败" << std::endl;
		reply.set_error(ErrorCodes::RPCFailed);
		return reply;
	}
}

VarifyGrpcClient::VarifyGrpcClient()
{
	auto& gCfgMgr = ConfigMgr::GetInstance();
	std::string host = gCfgMgr["VarifyServer"]["Host"];
	std::string port = gCfgMgr["VarifyServer"]["Port"];
	std::cout << host << " " << port << std::endl;
	_pool.reset(new RPConPool(host, port, 5));
}

RPConPool::RPConPool(std::string host, std::string port, std::size_t poolSize)
	: _poolSize(poolSize), _host(host), _port(port), _b_stop(false)
{
	for (std::size_t i = 0; i < _poolSize; i++) {
	//这个相当于是一个通道，通过通道与服务器通信
	//第一个参数为目的地，第二个参数为通信凭证
		std::shared_ptr<Channel> channel = grpc::CreateChannel(_host + ":" + _port,
			grpc::InsecureChannelCredentials());
		//创建stub
		_connections.emplace(VarifyService::NewStub(channel));
	}
}

RPConPool::~RPConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);
	//将标志置为true 并通知其他被阻塞的线程
	_b_stop = true;
	_cond.notify_all();

	// 清空队列
	while (!_connections.empty()) {
		_connections.pop();
	}
}

std::unique_ptr<VarifyService::Stub> RPConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);
	//队列不空或者标志为true就不阻塞
	_cond.wait(lock, [this] {
		return _b_stop == true || !_connections.empty();
		});
	if (_b_stop) return nullptr;
	std::unique_ptr<VarifyService::Stub> context = std::move(_connections.front());
	_connections.pop();
	return context;
}

void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_b_stop) {
		return;
	}
	_connections.push(std::move(context));
	_cond.notify_one();
}
