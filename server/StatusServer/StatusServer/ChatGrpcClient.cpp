#include "ChatGrpcClient.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"

#include "MysqlMgr.h"


// �ȴ�config.ini�ж����Է����������������
ChatGrpcClient::ChatGrpcClient() {
	auto& cfg = ConfigMgr::GetInstance();
	auto server_list = cfg["ChatServers"]["Name"];

	// �����洢ÿ��server�Ķ�Ӧ����
	std::vector<std::string> words;

	std::stringstream ss(server_list);
	std::string word;

	// �ȴӡ�PeerServer����ȡ��server���֣������ö��Ÿ���
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

// ֪ͨ�Ӻ��ѽӿ�
AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq& req) {
	AddFriendRsp rsp;
	return rsp;
}