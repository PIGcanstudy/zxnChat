#pragma once
#include "const.h"
#include "LogicSystem.h"
#include <unordered_map>

class HttpConnection: public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem;
	HttpConnection(boost::asio::io_context& ioc);
	void Start();

	tcp::socket& GetSocket() {
		return _socket;
	}
private:
	//��ⳬʱ�ĺ���
	void CheckDeadline();

	//Ӧ��Է�
	void WriteResponse();

	//��������,������������ͷ�Լ���������
	void HandleReq();

	//��������
	void PreParseGetParam();

	tcp::socket _socket;

	//�������ݵĻ�����Ϊ8K
	beast::flat_buffer _buffer{ 8192 };
	
	//���նԷ�����,���е�����Ϊ��̬���ͣ��ɽ����κ����͵�����
	http::request<http::dynamic_body> _request;

	//�����Է�����Ӧ�����е�����Ϊ��̬���ͣ��ɽ����κ����͵�����
	http::response<http::dynamic_body> _response;

	//������ʱ�����ӳ�ʱ��Ҫ�õ�, ��ʱ60s
	//ʹ�û����ſ����������������ϵ����壬��Ϊ�����ܻᱻ��Ϊ������һ������
	net::steady_timer dead_time{ _socket.get_executor(), std::chrono::seconds(60) };

	// ������ʾurl
	std::string _get_url;

	//�����洢����
	std::unordered_map<std::string, std::string> _get_params;
};

