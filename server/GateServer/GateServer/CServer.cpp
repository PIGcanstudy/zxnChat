#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"
CServer::CServer(boost::asio::io_context& ioc, unsigned short port): _ioc(ioc),
_acceptor(ioc, tcp::endpoint(tcp::v4(), port)){
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
	//获取服务
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	//通过获取的服务创建连接
	std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);
	_acceptor.async_accept(new_con->GetSocket(), [self, new_con](boost::system::error_code ec) {
		try {
			//如果出错就放弃这个连接，继续监听其他连接
			if (ec) {
				self->Start();
				return;
			}
			
			// 启动新连接，监听读事件
			new_con->Start();

			//继续监听其他连接
			self->Start();
		}
		catch (std::exception& e) {
			std::cerr << "Exception is " << e.what() << std::endl;
			self->Start();
		}
		});
}
