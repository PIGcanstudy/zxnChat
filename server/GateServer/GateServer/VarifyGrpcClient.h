#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class VarifyGrpcClient :public Singleton<VarifyGrpcClient> {
	friend class Singleton<VarifyGrpcClient>;
public:
	GetVarifyRsp GetVarifyCode(std::string& email) {
		//客户端的上下文 和 io_context类似
		ClientContext context;
		//接收的回应
		GetVarifyRsp reply;
		//发送的请求
		GetVarifyReq request;
		//设置email
		request.set_email(email);
		//通过stub来调用其他服务器的接口
		Status status = stub_->GetVarifyCode(&context, request,  &reply);
		if (status.ok()) {
			return reply;
		}
		else {
			reply.set_error(ErrorCodes::RPCFailed);
			return reply;
		}
	}
private:
	VarifyGrpcClient() {
		//这个相当于是一个通道，通过通道与服务器通信
		//第一个参数为目的地，第二个参数为通信凭证
		std::shared_ptr<Channel> channel = grpc::CreateChannel("0.0.0.0:50051",
			grpc::InsecureChannelCredentials());
		stub_ = VarifyService::NewStub(channel);
	}
	std::unique_ptr<VarifyService::Stub> stub_;
};
