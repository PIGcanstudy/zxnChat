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
	//检测超时的函数
	void CheckDeadline();

	//应答对方
	void WriteResponse();

	//处理请求,包括解析请求头以及包内内容
	void HandleReq();

	//解析参数
	void PreParseGetParam();

	tcp::socket _socket;

	//接收数据的缓冲区为8K
	beast::flat_buffer _buffer{ 8192 };
	
	//接收对方请求,其中的类型为动态类型，可接受任何类型的请求
	http::request<http::dynamic_body> _request;

	//传给对方的响应，其中的类型为动态类型，可接受任何类型的请求
	http::response<http::dynamic_body> _response;

	//用来定时，连接超时需要用到, 定时60s
	//使用花括号可以做到避免语义上的歧义，因为它可能会被认为是声明一个函数
	net::steady_timer dead_time{ _socket.get_executor(), std::chrono::seconds(60) };

	// 用来表示url
	std::string _get_url;

	//用来存储参数
	std::unordered_map<std::string, std::string> _get_params;
};

