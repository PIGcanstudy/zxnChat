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
	rsp.set_error(ErrorCodes::Success);

	Defer defer([&rsp, &req]() {
		rsp.set_fromuid(req.fromuid());
		rsp.set_touid(req.touid());
		});

	auto find_iter = pool_.find(server_ip);
	if (find_iter == pool_.end()) {
		return rsp;
	}

	auto& pool = find_iter->second;
	ClientContext context;
	auto stub = pool->GetConnection();
	Status status = stub->NotifyAuthFriend(&context, req, &rsp);

	Defer defercon([&stub, this, &pool]() {
		pool->ReturnConnections(std::move(stub));
		});

	if (!status.ok()) {
		rsp.set_error(ErrorCodes::RPCFailed);
		return rsp;
	}

	return rsp;
}

// 得到基础的用户信息接口
bool ChatGrpcClient::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo) {
	//优先查redis中查询用户信息
	std::string info_str = "";
	bool b_base = RedisMgr::GetInstance()->Get(base_key, info_str);
	if (b_base) {
		Json::Reader reader;
		Json::Value root;
		reader.parse(info_str, root);
		userinfo->uid = root["uid"].asInt();
		userinfo->name = root["name"].asString();
		userinfo->pwd = root["pwd"].asString();
		userinfo->email = root["email"].asString();
		userinfo->nick = root["nick"].asString();
		userinfo->desc = root["desc"].asString();
		userinfo->sex = root["sex"].asInt();
		userinfo->icon = root["icon"].asString();
		std::cout << "user login uid is  " << userinfo->uid << " name  is "
			<< userinfo->name << " pwd is " << userinfo->pwd << " email is " << userinfo->email << endl;
	}
	else {
		//redis中没有则查询mysql
		//查询数据库
		std::shared_ptr<UserInfo> user_info = nullptr;
		user_info = MysqlMgr::GetInstance()->GetUser(uid);
		if (user_info == nullptr) {
			return false;
		}

		userinfo = user_info;

		//将数据库内容写入redis缓存
		Json::Value redis_root;
		redis_root["uid"] = uid;
		redis_root["pwd"] = userinfo->pwd;
		redis_root["name"] = userinfo->name;
		redis_root["email"] = userinfo->email;
		redis_root["nick"] = userinfo->nick;
		redis_root["desc"] = userinfo->desc;
		redis_root["sex"] = userinfo->sex;
		redis_root["icon"] = userinfo->icon;
		RedisMgr::GetInstance()->Set(base_key, redis_root.toStyledString());
	}
	return true;
}

// 通知有文本聊天消息需要发送
TextChatMsgRsp ChatGrpcClient::NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue) {
	TextChatMsgRsp rsp;
	rsp.set_error(ErrorCodes::Success);

	// 设置响应给调用客户端的函数
	Defer defer([&rsp, &req]() {
		rsp.set_fromuid(req.fromuid());
		rsp.set_touid(req.touid());
		for (const auto& text_data : req.textmsgs()) {
			TextChatData* new_msg = rsp.add_textmsgs();
			new_msg->set_msgid(text_data.msgid());
			new_msg->set_msgcontent(text_data.msgcontent());
		}
		});

	// 找服务器ip对应的连接池
	auto find_iter = pool_.find(server_ip);
	if (find_iter == pool_.end()) {
		return rsp;
	}

	auto& pool = find_iter->second;
	ClientContext context;
	// 从连接池中获取一个stub
	auto stub = pool->GetConnection();
	// 通过stub调用gRPC服务端
	Status status = stub->NotifyTextChatMsg(&context, req, &rsp);
	
	// 用完返回
	Defer defercon([&stub, this, &pool]() {
		pool->ReturnConnections(std::move(stub));
		});

	// 调用不成功就设置错误码
	if (!status.ok()) {
		rsp.set_error(ErrorCodes::RPCFailed);
		return rsp;
	}

	return rsp;
}