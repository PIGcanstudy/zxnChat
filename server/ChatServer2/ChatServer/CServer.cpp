#include "CServer.h"
#include "CSession.h"
#include "AsioIOServicePool.h"
#include <sstream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "LogicSystem.h"
#include "UserMgr.h"
#include "TimeWheel.h"
CServer::CServer(boost::asio::io_context& ioc, unsigned short port): _ioc(ioc), _port(port),
_acceptor(ioc, tcp::endpoint(tcp::v4(), port)){
	std::cout << "Server start success, listen on port: " << _port << std::endl;
	StartAccept();
}

void CServer::StartAccept(){
	
	//获取服务
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	//通过获取的服务创建连接
	std::shared_ptr<CSession> new_session = std::make_shared<CSession>(io_context, this);
	_acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session, std::placeholders::_1)); 
}

CServer::~CServer()
{
	cout << "Server destruct listen on port : " << _port << endl;
}

void CServer::ClearUserSession(decltype(_sessions.begin()) it) {
	if (it != _sessions.end()) {
		// 移除用户和 session 的关联
		UserMgr::GetInstance()->RmvUserSession(it->second->GetUserId());
	}
}

void CServer::ClearSession(const std::string& session_id)
{
	if (_sessions.find(session_id) != _sessions.end()) {
		//移除用户和session的关联
		UserMgr::GetInstance()->RmvUserSession(_sessions[session_id]->GetUserId());
	}

	{
		lock_guard<mutex> lock(_mutex);
		_sessions.erase(session_id);
	}
}

void CServer::startHeartbeatMonitor(int check_interval_seconds, int timeout_seconds){
	TimeWheel::GetIntance().Tw_Add_Node(check_interval_seconds * 1000, check_interval_seconds * 1000, PERIOD_FLAG, [this, timeout_seconds]() {
		for (auto it = _sessions.begin(); it != _sessions.end(); ) {
			if (it->second->isTimedOut(timeout_seconds)) {
				std::cout << "Client " << it->first << " timed out.\n";
				auto session = it->second;
				ClearUserSession(it);
				std::cout << session.use_count() << std::endl;
				it = _sessions.erase(it);
				std::cout << session.use_count() << std::endl;
				session->Close();
			}
			else {
				++it;
			}
		}
		});
}

void CServer::HandleAccept(shared_ptr<CSession>& new_session, const boost::system::error_code& error)
{
	if (!error) {
		// 监听读写事件
		new_session->Start();

		lock_guard<mutex> lock(_mutex);
		// 插入_sessions中
		_sessions.insert(make_pair(new_session->GetSessionUuid(), new_session));
	}
	else {
		cout << "session accept failed, error is " << error.what() << endl;
	}

	StartAccept();
}

