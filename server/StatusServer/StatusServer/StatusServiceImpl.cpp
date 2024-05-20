#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "boost/uuid/uuid_generators.hpp"

#include "const.h"
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
	// ��ȡ�������ٵ����������
	const auto& server = getChatServer();
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
	ChatServer server;

	// ��ʼ����һ�����������
	server.port = cfg["ChatServer1"]["Port"];
	server.host = cfg["ChatServer1"]["Host"];
	server.con_count = 0;
	server.name = cfg["ChatServer1"]["Name"];
	_servers[server.name] = server;

	// ��ʼ���ڶ������������
	server.port = cfg["ChatServer2"]["Port"];
	server.host = cfg["ChatServer2"]["Host"];
	server.name = cfg["ChatServer2"]["Name"];
	server.con_count = 0;
	_servers[server.name] = server;
}

// ��ȡ���������ٵ����������
ChatServer StatusServiceImpl::getChatServer() {

	std::lock_guard<std::mutex> guard(_server_mtx);

	auto minServer = _servers.begin()->second;
	// ʹ�÷�Χ����forѭ��
	for (const auto& server : _servers) {
		// �����������б�ѡ�����������ٵķ�����
		// con_count ��chatserver������������������
		if (server.second.con_count < minServer.con_count) {
			minServer = server.second;
		}
	}

	return minServer;
}

// �û���¼����ʵ�֣���֤token
Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request, LoginRsp* reply)
{
	auto uid = request->uid();
	auto token = request->token();
	// ��֤�̰߳�ȫ
	std::lock_guard<std::mutex> guard(_token_mtx);

	auto iter = _tokens.find(uid);

	// �û�id��Ч
	if (iter == _tokens.end()) {
		reply->set_error(ErrorCodes::UidInvalid);
		return Status::OK;
	}

	// �û�token��Ч
	if (iter->second != token) {
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
	std::lock_guard<std::mutex> guard(_token_mtx);
	_tokens[uid] = token;
}

