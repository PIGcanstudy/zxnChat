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


// ����
class RPConPool {
public:
	//�������
	RPConPool(std::string host, std::string port, std::size_t poolSize = std::thread::hardware_concurrency());
	//��������
	~RPConPool();
	// �ӳ�����ȡStub
	std::unique_ptr<VarifyService::Stub> getConnection();
	// �߳������stub��������
	void returnConnection(std::unique_ptr<VarifyService::Stub> context);

private:
	//������ǳ����Ƿ�ر�
	std::atomic_bool _b_stop;
	//������Ĵ�С
	size_t _poolSize;
	//������ַ
	std::string _host;
	//�˿�
	std::string _port;

	//���У������洢���Stub�����̴߳���ȡStub
	std::queue<std::unique_ptr<VarifyService::Stub>> _connections;

	//����ʵ�ֻ������
	std::mutex _mutex;

	//����֪ͨ����������
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
