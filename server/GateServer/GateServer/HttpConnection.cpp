#include "HttpConnection.h"

HttpConnection::HttpConnection(boost::asio::io_context& ioc): _socket(ioc)
{
	
}

void HttpConnection::Start()
{
	//用来防止伪闭包
	auto self = shared_from_this();
	//一共有四个参数，第一个参数为异步读的流（理解为sokcet）
	//第二个参数为 一个缓冲区buffer
	//第三个参数req：表示一个 HTTP 请求对象，用于存储从流中读取的 HTTP 请求内容。
	//第四个参数为异步函数所需要的回调处理函数
	http::async_read(_socket, _buffer, _request, [self](beast::error_code ec, std::size_t bytes_transferred) {
		try {
			if (ec) {
				std::cerr << "http read err is " << ec.what() << std::endl;
				return;
			}

			//因为是http服务器所以无需做粘包处理，底层帮我们实现了
			//所以可以忽略已经传送的字节
			boost::ignore_unused(bytes_transferred);

			// 处理请求
			self->HandleReq();

			//检测是否超时
			self->CheckDeadline();
		}
		catch (std::exception& e) {
			std::cerr << "Exception is " << e.what() << std::endl;
		}
		});
}

void HttpConnection::CheckDeadline()
{
	
	auto self = shared_from_this();
	dead_time.async_wait([self](beast::error_code ec){
		if (!ec) {
			//关掉socket
			self->_socket.close(ec);
			}
		});
}

void HttpConnection::WriteResponse()
{
	auto self = shared_from_this();
	//设置包体长度
	_response.content_length(_response.body().size());
	//一共三个参数，第一个为流（理解为socket）
	//第二个为发送的响应
	//第三个为异步回调处理函数
	http::async_write(_socket, _response, [self](beast::error_code ec, std::size_t bytes_transferred) {
		// 这个是为了关闭一端，关闭发送端
		self->_socket.shutdown(tcp::socket::shutdown_send, ec);
		// 取消定时器
		self->dead_time.cancel();
		});
}

//char 转为16进制
unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

//从十六进制转为十进制char
unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

//URL编码
std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		//判断是否仅有数字和字母构成
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ') //为空字符
			strTemp += "+";
		else
		{
			//其他字符需要提前加%（用以标识）并且高四位和低四位分别转为16进制
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] & 0x0F);
		}
	}
	return strTemp;
}

//URL解码
std::string UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		//还原+为空
		if (str[i] == '+') strTemp += ' ';
		//遇到%将后面的两个字符从16进制转为char再拼接
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			//转为10进制 十六进制 10 代表 十进制16
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}

void HttpConnection::PreParseGetParam() {
	// 提取 URI   get_test?key1=value1&key2=value2
	auto uri = _request.target();
	// 查找查询字符串的开始位置（即 '?' 的位置）  
	auto query_pos = uri.find('?');
	//没找到就说明没有参数，只有url
	if (query_pos == std::string::npos) {
		_get_url = uri;
		return;
	}

	// ?前为URL
	_get_url = uri.substr(0, query_pos);
	//存储参数
	std::string query_string = uri.substr(query_pos + 1);

	std::string key;
	std::string value;
	// 用来记录&前面的每一组参数最后字符的位置
	size_t pos = 0;

	while ((pos = query_string.find('&')) != std::string::npos) {
		// 读取到key1=value1
		auto pair = query_string.substr(0, pos);
		size_t eq_pos = pair.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(pair.substr(0, eq_pos)); // 假设有 url_decode 函数来处理URL解码  
			value = UrlDecode(pair.substr(eq_pos + 1));
			_get_params[key] = value;
		}
		//后面的元素会前移
		query_string.erase(0, pos + 1);
	}
	// 处理最后一个参数对（如果没有 & 分隔符）  
	if (!query_string.empty()) {
		size_t eq_pos = query_string.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(query_string.substr(0, eq_pos));
			value = UrlDecode(query_string.substr(eq_pos + 1));
			_get_params[key] = value;
		}
	}
}

void HttpConnection::HandleReq()
{
	//设置版本
	_response.version(_request.version());
	//设置连接为短连接
	_response.keep_alive(false);

	switch (_request.method())
	{
	case http::verb::get:
	{
		//解析参数
		PreParseGetParam();
		//获取路由并且传给逻辑系统的处理get请求的函数
		bool success = LogicSystem::GetInstance()->HandleGet(_get_url, shared_from_this());
		////如果失败
		//if (!success) {
		//	//设置结果
		//	_response.result(http::status::not_found);
		//	//设置发送的类型,
		//	// "text/plain" 是一种 MIME 类型（Multipurpose Internet Mail Extensions）
		//	// 用于标识纯文本数据的内容类型
		//	_response.set(http::field::content_type, "text/plain");
		//	//设置body内容
		//	beast::ostream(_response.body()) << "url not found\r\n";
		//	WriteResponse();
		//	return;
		//}

		//_response.result(http::status::ok);
		////设置是哪个服务发过去的
		//_response.set(http::field::server, "GateServer");
		//WriteResponse();
		break;
	}
	case http::verb::post:
	{
		//获取路由并且传给逻辑系统的处理post请求的函数
		bool success = LogicSystem::GetInstance()->HandlePost(_request.target(), shared_from_this());
		////如果失败
		//if (!success) {
		//	//设置结果
		//	_response.result(http::status::not_found);
		//	//设置发送的类型,
		//	// "text/plain" 是一种 MIME 类型（Multipurpose Internet Mail Extensions）
		//	// 用于标识纯文本数据的内容类型
		//	_response.set(http::field::content_type, "text/plain");
		//	//设置body内容
		//	beast::ostream(_response.body()) << "url not found\r\n";
		//	WriteResponse();
		//	return;
		//}

		//_response.result(http::status::ok);
		////设置是哪个服务发过去的
		//_response.set(http::field::server, "GateServer");
		//WriteResponse();
		break;
	}
	default:
		break;
	}
}
