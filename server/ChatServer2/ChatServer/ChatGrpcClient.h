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

	// 得到stub
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

	// 使用完后放回池子
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

	 // 通知加好友接口
	 AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq& req);

	 // 通知认证好友接口
	 AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req);

	 // 得到基础的用户信息接口
	 bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);

	 // 通知有文本聊天消息需要发送
	 TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue);

 private:
	 ChatGrpcClient();
	 // 由于一个服务器可能会有多个客户端连接，所以调用服务的时候可能会由多个客户端同时调用同一个服务器的功能
	 // 因此需要一个服务器对应一个连接池
	 std::unordered_map<std::string, std::unique_ptr<ChatConPool>> pool_;
};

