#include "CServer.h"
#include "HttpConnection.h"
CServer::CServer(boost::asio::io_context& ioc, unsigned short port): _ioc(ioc),
_acceptor(ioc, tcp::endpoint(tcp::v4(), port)), _socket(ioc){
}

void CServer::Start(){
	//����д�ͻᱨ����Ϊû����α�հ�
	/*
	std::cout << "1" << std::endl;
	//auto self = shared_from_this();
	std::cout << "2" << std::endl;
	_acceptor.async_accept(_socket, [this](boost::system::error_code ec) {
		try {
			//�������ͷ���������ӣ�����������������
			if (!ec) {
				//����һ�������ӣ�����ʹ��HttpConnection���������������
				//ʹ������ָ����Ϊ������ӵ���뿪���������л��᲻���ͷŵ�����
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
			//�������ͷ���������ӣ�����������������
			if (!ec) {
				//����һ�������ӣ�����ʹ��HttpConnection���������������
				//ʹ������ָ����Ϊ������ӵ���뿪���������л��᲻���ͷŵ�����
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
