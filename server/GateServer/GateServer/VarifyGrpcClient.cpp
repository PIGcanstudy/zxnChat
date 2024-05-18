#include "VarifyGrpcClient.h"
#include "ConfigMgr.h"

GetVarifyRsp  VarifyGrpcClient::GetVarifyCode(std::string& email) {
	//�ͻ��˵������� �� io_context����
	ClientContext context;
	//���յĻ�Ӧ
	GetVarifyRsp reply;
	//���͵�����
	GetVarifyReq request;
	//����email
	request.set_email(email);

	//�õ�stub
	auto stub = _pool->getConnection();

	//ͨ��stub�����������������Ľӿ�
	Status status = stub->GetVarifyCode(&context, request, &reply);
	
	if (status.ok()) {
		//��������
		_pool->returnConnection(std::move(stub));
		std::cout << "grpc���óɹ� "<< std::endl;
		return reply;
	}
	else {
		_pool->returnConnection(std::move(stub));
		std::cout << "grpc����ʧ��" << std::endl;
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
	//����൱����һ��ͨ����ͨ��ͨ���������ͨ��
	//��һ������ΪĿ�ĵأ��ڶ�������Ϊͨ��ƾ֤
		std::shared_ptr<Channel> channel = grpc::CreateChannel(_host + ":" + _port,
			grpc::InsecureChannelCredentials());
		//����stub
		_connections.emplace(VarifyService::NewStub(channel));
	}
}

RPConPool::~RPConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);
	//����־��Ϊtrue ��֪ͨ�������������߳�
	_b_stop = true;
	_cond.notify_all();

	// ��ն���
	while (!_connections.empty()) {
		_connections.pop();
	}
}

std::unique_ptr<VarifyService::Stub> RPConPool::getConnection()
{
	std::unique_lock<std::mutex> lock(_mutex);
	//���в��ջ��߱�־Ϊtrue�Ͳ�����
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
