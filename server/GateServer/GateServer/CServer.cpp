#include "CServer.h"
#include "HttpConnection.h"
CServer::CServer(boost::asio::io_context& ioc, unsigned short port): _ioc(ioc),
_acceptor(ioc, tcp::endpoint(tcp::v4(), port)), _socket(ioc){
}

void CServer::Start(){
	//这样写就会报错，因为没有用伪闭包
	/*
	std::cout << "1" << std::endl;
	//auto self = shared_from_this();
	std::cout << "2" << std::endl;
	_acceptor.async_accept(_socket, [this](boost::system::error_code ec) {
		try {
			//如果出错就放弃这个连接，继续监听其他连接
			if (!ec) {
				//创建一个新连接，并且使用HttpConnection类来管理这个连接
				//使用智能指针是为了让他拥有离开作用域仍有机会不被释放的能力
				std::make_shared<HttpConnection>(std::move(this->_socket))->Start();
			}
			this->Start();
		}
		catch (std::exception& e) {
			std::cerr << "Exception is " << e.what() << std::endl;
			this->Start();
		}
		});
	*/

	auto self = shared_from_this();
	_acceptor.async_accept(_socket, [self](boost::system::error_code ec) {
		try {
			//如果出错就放弃这个连接，继续监听其他连接
			if (!ec) {
				//创建一个新连接，并且使用HttpConnection类来管理这个连接
				//使用智能指针是为了让他拥有离开作用域仍有机会不被释放的能力
				std::make_shared<HttpConnection>(std::move(self->_socket))->Start();
			}
			self->Start();
		}
		catch (std::exception& e) {
			std::cerr << "Exception is " << e.what() << std::endl;
			self->Start();
		}
		});
}
