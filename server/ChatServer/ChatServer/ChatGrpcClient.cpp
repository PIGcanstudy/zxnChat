#include "ChatGrpcClient.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"
#include "UserMgr.h"

#include "CSession.h"
#include "MysqlMgr.h"


// 先从config.ini中读出对方服务器的相关内容
ChatGrpcClient::ChatGrpcClient() {
	auto& cfg = ConfigMgr::GetInstance();
	auto server_list = cfg["PeerServer"]["Servers"];

	// 用来存储每个server的对应属性
	std::vector<std::string> words;

	std::stringstream ss(server_list);
	std::string word;

	// 先从【PeerServer】中取出server名字，可能用逗号隔开
	while (std::getline(ss, word, ',')) {
		words.push_back(word);
	}

	for (auto& word : words) {
		if (cfg[word]["Name"].empty()) {
			continue;
		}
		pool_[cfg[word]["Name"]] = std::make_unique<ChatConPool>(5, cfg[word]["Host"], cfg[word]["Port"]);
	}

}

// 通知加好友接口
AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq& req) {
	AddFriendRsp rsp;
	Defer defer([&rsp, &req]() {
		rsp.set_error(ErrorCodes::Success);
		rsp.set_applyuid(req.applyuid());
		rsp.set_touid(req.touid());
		});

	// 根据ip获取服务池
	auto find_iter = pool_.find(server_ip);
	if (find_iter == pool_.end()) {
		return rsp;
	}
	auto& pool = find_iter->second;

	// 调用对端相应的服务
	ClientContext context;
	auto stub = pool->GetConnection();
	Status status = stub->NotifyAddFriend(&context, req, &rsp);
	Defer defercon([&stub, this, &pool]() {
		pool->ReturnConnections(std::move(stub));
		});
	if (!status.ok()) {
		rsp.set_error(ErrorCodes::RPCFailed);
		std::cout << "Service NotifyAddFriend failed!\n";
		return rsp;
	}
	std::cout << "Service NotifyAddFriend succeed!\n";
	return rsp;
}

// 通知认证好友接口
AuthFriendRsp ChatGrpcClient::NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req) {
	AuthFriendRsp rsp;
	return rsp;
}

// 得到基础的用户信息接口
bool ChatGrpcClient::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
	return true;
}

// 通知有文本聊天消息需要发送
TextChatMsgRsp ChatGrpcClient::NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue) {
	TextChatMsgRsp rsp;
	return rsp;
}