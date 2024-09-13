#pragma once

#include "const.h"
#include "CSession.h"
#include <unordered_map>
#include <mutex>
// ��ʱʱ��Ϊ60s(45s + 15s) ����Ϊ����ʱ��
class CServer: public std::enable_shared_from_this<CServer>
{
private:
	//�����Ӽ���map���������ü�������ֹδִ�����߼��ͱ�����
	std::unordered_map<std::string, std::shared_ptr<CSession>> _sessions;
public:
	CServer(boost::asio::io_context& ioc, unsigned short port);
	~CServer();
	void ClearUserSession(decltype(_sessions.begin()) it);
	void ClearSession(const std::string& session_id);
	void startHeartbeatMonitor(int check_interval_seconds, int timeout_seconds);
private:
	void HandleAccept(shared_ptr<CSession>& new_session, const boost::system::error_code& error);
	void StartAccept();
	tcp::acceptor _acceptor;
	net::io_context& _ioc;
	unsigned short _port;
	std::mutex _mutex;
};

