#include "ChatGrpcClient.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"

#include "MysqlMgr.h"


// 先从config.ini中读出对方服务器的相关内容
ChatGrpcClient::ChatGrpcClient() {
	auto& cfg = ConfigMgr::GetInstance();
	auto server_list = cfg["ChatServers"]["Name"];

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
	return rsp;
}