#pragma once
#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
#include <grpcpp/grpcpp.h> 
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <queue>
#include "const.h"
#include "data.h"
#include <mutex>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::GetChatServerRsp;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;

using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

class ChatConPool {
public:
	ChatConPool(unsigned int size, std::string host, std::string port) 
		: b_stop_(false), poolSize_(size), host_(host), port_(port)
	{
		for (int i = 0; i < poolSize_; i++) {
			auto channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
			connections_.emplace(ChatService::NewStub(channel));
		}
	}

	~ChatConPool() {
		Close();
		std::lock_guard<std::mutex> lock(mutex_);
		while (connections_.size()) {
			connections_.pop();
		}
	}

	void Close(){
		b_stop_.store(true);
		cond_.notify_all();
	}

	// �õ�stub
	std::unique_ptr<ChatService::Stub> GetConnection() {
		std::unique_lock<std::mutex> lock(mutex_);
		cond_.wait(lock, [this]() {
			return b_stop_.load() || !connections_.empty();
			});
		if (b_stop_.load()) return nullptr;
		auto connection = std::move(connections_.front());
		connections_.pop();
		lock.unlock();
		return connection;
	}

	// ʹ�����Żس���
	void ReturnConnections(std::unique_ptr<ChatService::Stub> connection) {
		{
			std::lock_guard<std::mutex> lock(mutex_);
			if (b_stop_) {
				return;
			}
			connections_.emplace(std::move(connection));
		}
		cond_.notify_one();
	}
private:	
	std::atomic_bool b_stop_;
	unsigned int poolSize_;
	std::string host_;
	std::string port_;
	std::queue<std::unique_ptr<ChatService::Stub>> connections_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

class ChatGrpcClient: public Singleton<ChatGrpcClient>
{
	friend class Singleton<ChatGrpcClient>;
 public:
	 ~ChatGrpcClient() {

	 }

	 // ֪ͨ�Ӻ��ѽӿ�
	 AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq& req);

	 // ֪ͨ��֤���ѽӿ�
	 AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req);

	 // �õ��������û���Ϣ�ӿ�
	 bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);

	 // ֪ͨ���ı�������Ϣ��Ҫ����
	 TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue);

 private:
	 ChatGrpcClient();
	 // ����һ�����������ܻ��ж���ͻ������ӣ����Ե��÷����ʱ����ܻ��ɶ���ͻ���ͬʱ����ͬһ���������Ĺ���
	 // �����Ҫһ����������Ӧһ�����ӳ�
	 std::unordered_map<std::string, std::unique_ptr<ChatConPool>> pool_;
};

