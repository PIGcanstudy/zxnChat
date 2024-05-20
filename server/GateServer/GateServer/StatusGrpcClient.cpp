#include "StatusGrpcClient.h"
#include "ConfigMgr.h"

StatusConPool::StatusConPool(std::size_t poolSize, std::string host, std::string port)
	: _poolSize(poolSize), _host(host), _port(port), _b_stop(false)
{
	for (size_t i = 0; i < _poolSize; ++i) {

		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
			grpc::InsecureChannelCredentials());

		_connections.push(StatusService::NewStub(channel));
	}
}

StatusConPool::~StatusConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);
	Close();
	while (!_connections.empty()) {
		_connections.pop();
	}
}

std::unique_ptr<StatusService::Stub> StatusConPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this] {
        return !_connections.empty() || _b_stop.load();
        });
    //如果停止则直接返回空指针
    if (_b_stop) {
        return  nullptr;
    }
    auto context = std::move(_connections.front());
    _connections.pop();
    return context;
}

void StatusConPool::returnConnection(std::unique_ptr<StatusService::Stub> context)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_b_stop.load()) {
        return;
    }
    _connections.push(std::move(context));
    _cond.notify_one();
}

void StatusConPool::Close()
{
	_b_stop = true;
	_cond.notify_all();
}

GetChatServerRsp StatusGrpcClient::GetChatServer(int uid)
{
	ClientContext context;
	GetChatServerRsp reply;
	GetChatServerReq request;
	request.set_uid(uid);
	//得到链接
	auto stub = pool_->getConnection();

	Status status = stub->GetChatServer(&context, request, &reply);
	
	Defer defer([&stub, this]() {
		pool_->returnConnection(std::move(stub));
		});

	if (status.ok()) {
		return reply;
	}
	else {
		reply.set_error(ErrorCodes::RPCFailed);
		return reply;
	}
}

LoginRsp StatusGrpcClient::Login(int uid, std::string token)
{
	ClientContext context;
	LoginRsp reply;
	LoginReq request;
	request.set_uid(uid);
	request.set_token(token);

	auto stub = pool_->getConnection();

	Status status = stub->Login(&context, request, &reply);
	
	Defer defer([&stub, this]() {
		pool_->returnConnection(std::move(stub));
		});

	if (status.ok()) {
		return reply;
	}
	else {
		reply.set_error(ErrorCodes::RPCFailed);
		return reply;
	}
}

StatusGrpcClient::StatusGrpcClient()
{
    auto& gCfgMgr = ConfigMgr::GetInstance();
    std::string host = gCfgMgr["StatusServer"]["Host"];
    std::string port = gCfgMgr["StatusServer"]["Port"];
    pool_.reset(new StatusConPool(5, host, port));
}
