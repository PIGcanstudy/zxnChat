#pragma once

#include "const.h"

class CServer: public std::enable_shared_from_this<CServer>
{
public:
	CServer(boost::asio::io_context& ioc, unsigned short port);
	void Start();
private:
	tcp::acceptor _acceptor;
	net::io_context& _ioc;
	//用来复用，接收对端连接，然后转给其他socket
	tcp::socket _socket;
};

