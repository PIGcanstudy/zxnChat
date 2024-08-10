#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "boost/uuid/uuid_generators.hpp"

#include "const.h"
#include "RedisMgr.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

// ����Ψһ�ַ�����UUID��
std::string generate_unique_string() {
	// ����UUID����
	boost::uuids::uuid uuid = boost::uuids::random_generator()();

	// ��UUIDת��Ϊ�ַ���
	std::string unique_string = boost::uuids::to_string(uuid);

	return unique_string;
}

// ��ȡ�������������ʵ��
Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply)
{
	std::string prefix("chat status server has received :  ");
	std::cout << "handling getChatServer!\n";
	// ��ȡ�������ٵ����������
	const auto& server = getChatServer();
	std::cout << "getChatServer ok!\n";
	reply->set_host(server.host);
	reply->set_port(server.port);
	reply->set_error(ErrorCodes::Success);
	reply->set_token(generate_unique_string());
	// ��token����map�����û���ƥ��
	insertToken(request->uid(), reply->token());
	return Status::OK;
}

// ���캯����ʵ�ַ������б�ĳ�ʼ��
StatusServiceImpl::StatusServiceImpl()
{
	auto& cfg = ConfigMgr::GetInstance();

	std::vector<std::string> words;

	// ����server�б� 
	auto server_list = cfg["ChatServers"]["Name"];

	// ChatServers

	std::stringstream ss(server_list);
	std::string word;

	// ����ÿ��server�ľ�������
	while (std::getline(ss, word, ',')) {
		words.emplace_back(word);
	}

	// ����ÿ��server�ľ������Բ�������
	for (auto& w : words) {
		if (cfg[w]["Name"].empty()) {
			continue;
		}

		ChatServer server;
		server.name = cfg[w]["Name"];
		server.host = cfg[w]["Host"];
		server.port = cfg[w]["Port"];
		_servers[server.name] = server;
		std::cout << "server is " << server.name << " Host is " << server.host << std::endl;
	}
}

// ��ȡ���������ٵ����������
ChatServer StatusServiceImpl::getChatServer() {

	std::cout << "1 \n";
	std::lock_guard<std::mutex> guard(_server_mtx);
	ChatServer minServer = _servers.begin()->second;

	// ��redis��ȡ�����µķ����������ӿͻ�������
	auto count_str = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, minServer.name);
	if (count_str.empty()) {
		//��������Ĭ������Ϊ���
		minServer.con_count = INT_MAX;
	}
	else {
		minServer.con_count = std::stoi(count_str);
	}

	// ʹ�÷�Χ����forѭ��
	for (auto& server : _servers) {
		if (server.second.name == minServer.name) {
			continue;
		}

		auto count_str = RedisMgr::GetInstance()->HGet(LOGIN_COUNT, server.second.name);
		if (count_str.empty()) {
			server.second.con_count = INT_MAX;
		}
		else {
			server.second.con_count = std::stoi(count_str);
		}

		if (server.second.con_count < minServer.con_count) {
			minServer = server.second;
		}
	}
	std::cout << "minServer is " << minServer.name << std::endl;
	return minServer;
}

// �û���¼����ʵ�֣���֤token
Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request, LoginRsp* reply)
{
	auto uid = request->uid();
	auto token = request->token();

	std::string uid_str = std::to_string(uid);
	std::string token_key = USERTOKENPREFIX + uid_str;
	std::string token_value = "";

	std::cout << "handling Redis Get!\n";
	bool success = RedisMgr::GetInstance()->Get(token_key, token_value);
	std::cout << "Redis Get ok!\n";
	if (!success) {
		reply->set_error(ErrorCodes::UidInvalid);
		return Status::OK;
	}

	if (token_value != token) {
		reply->set_error(ErrorCodes::TokenInvalid);
		return Status::OK;
	}

	reply->set_error(ErrorCodes::Success);
	reply->set_uid(uid);
	reply->set_token(token);
	return Status::OK;
}

// �����û� token
void StatusServiceImpl::insertToken(int uid, std::string token)
{
	std::string uid_str = std::to_string(uid);
	// ƴ��keyֵ
	std::string token_key = USERTOKENPREFIX + uid_str;
	RedisMgr::GetInstance()->Set(token_key, token);
}

