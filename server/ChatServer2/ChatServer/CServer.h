#pragma once

#include "const.h"
#include "CSession.h"
#include <unordered_map>
#include <mutex>

class CServer: public std::enable_shared_from_this<CServer>
{
public:
	CServer(boost::asio::io_context& ioc, unsigned short port);
	~CServer();
	void ClearSession(std::string session_id);
private:
	void HandleAccept(shared_ptr<CSession> new_session, const boost::system::error_code& error);
	void StartAccept();
	tcp::acceptor _acceptor;
	net::io_context& _ioc;
	unsigned short _port;
	//�����Ӽ���map���������ü�������ֹδִ�����߼��ͱ�����
	std::unordered_map<std::string, std::shared_ptr<CSession>> _sessions;
	std::mutex _mutex;
};

