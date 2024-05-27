#include "HttpConnection.h"

HttpConnection::HttpConnection(boost::asio::io_context& ioc): _socket(ioc)
{
	
}

void HttpConnection::Start()
{
	//������ֹα�հ�
	auto self = shared_from_this();
	//һ�����ĸ���������һ������Ϊ�첽�����������Ϊsokcet��
	//�ڶ�������Ϊ һ��������buffer
	//����������req����ʾһ�� HTTP ����������ڴ洢�����ж�ȡ�� HTTP �������ݡ�
	//���ĸ�����Ϊ�첽��������Ҫ�Ļص�������
	http::async_read(_socket, _buffer, _request, [self](beast::error_code ec, std::size_t bytes_transferred) {
		try {
			if (ec) {
				std::cerr << "http read err is " << ec.what() << std::endl;
				return;
			}

			//��Ϊ��http����������������ճ�������ײ������ʵ����
			//���Կ��Ժ����Ѿ����͵��ֽ�
			boost::ignore_unused(bytes_transferred);

			// ��������
			self->HandleReq();

			//����Ƿ�ʱ
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
			//�ص�socket
			self->_socket.close(ec);
			}
		});
}

void HttpConnection::WriteResponse()
{
	auto self = shared_from_this();
	//���ð��峤��
	_response.content_length(_response.body().size());
	//һ��������������һ��Ϊ�������Ϊsocket��
	//�ڶ���Ϊ���͵���Ӧ
	//������Ϊ�첽�ص�������
	http::async_write(_socket, _response, [self](beast::error_code ec, std::size_t bytes_transferred) {
		// �����Ϊ�˹ر�һ�ˣ��رշ��Ͷ�
		self->_socket.shutdown(tcp::socket::shutdown_send, ec);
		// ȡ����ʱ��
		self->dead_time.cancel();
		});
}

//char תΪ16����
unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

//��ʮ������תΪʮ����char
unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

//URL����
std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		//�ж��Ƿ�������ֺ���ĸ����
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ') //Ϊ���ַ�
			strTemp += "+";
		else
		{
			//�����ַ���Ҫ��ǰ��%�����Ա�ʶ�����Ҹ���λ�͵���λ�ֱ�תΪ16����
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] & 0x0F);
		}
	}
	return strTemp;
}

//URL����
std::string UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		//��ԭ+Ϊ��
		if (str[i] == '+') strTemp += ' ';
		//����%������������ַ���16����תΪchar��ƴ��
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			//תΪ10���� ʮ������ 10 ���� ʮ����16
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}

void HttpConnection::PreParseGetParam() {
	// ��ȡ URI   get_test?key1=value1&key2=value2
	auto uri = _request.target();
	// ���Ҳ�ѯ�ַ����Ŀ�ʼλ�ã��� '?' ��λ�ã�  
	auto query_pos = uri.find('?');
	//û�ҵ���˵��û�в�����ֻ��url
	if (query_pos == std::string::npos) {
		_get_url = uri;
		return;
	}

	// ?ǰΪURL
	_get_url = uri.substr(0, query_pos);
	//�洢����
	std::string query_string = uri.substr(query_pos + 1);

	std::string key;
	std::string value;
	// ������¼&ǰ���ÿһ���������ַ���λ��
	size_t pos = 0;

	while ((pos = query_string.find('&')) != std::string::npos) {
		// ��ȡ��key1=value1
		auto pair = query_string.substr(0, pos);
		size_t eq_pos = pair.find('=');
		if (eq_pos != std::string::npos) {
			key = UrlDecode(pair.substr(0, eq_pos)); // ������ url_decode ����������URL����  
			value = UrlDecode(pair.substr(eq_pos + 1));
			_get_params[key] = value;
		}
		//�����Ԫ�ػ�ǰ��
		query_string.erase(0, pos + 1);
	}
	// �������һ�������ԣ����û�� & �ָ�����  
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
	//���ð汾
	_response.version(_request.version());
	//��������Ϊ������
	_response.keep_alive(false);

	switch (_request.method())
	{
	case http::verb::get:
	{
		//��������
		PreParseGetParam();
		//��ȡ·�ɲ��Ҵ����߼�ϵͳ�Ĵ���get����ĺ���
		bool success = LogicSystem::GetInstance()->HandleGet(_get_url, shared_from_this());
		////���ʧ��
		//if (!success) {
		//	//���ý��
		//	_response.result(http::status::not_found);
		//	//���÷��͵�����,
		//	// "text/plain" ��һ�� MIME ���ͣ�Multipurpose Internet Mail Extensions��
		//	// ���ڱ�ʶ���ı����ݵ���������
		//	_response.set(http::field::content_type, "text/plain");
		//	//����body����
		//	beast::ostream(_response.body()) << "url not found\r\n";
		//	WriteResponse();
		//	return;
		//}

		//_response.result(http::status::ok);
		////�������ĸ����񷢹�ȥ��
		//_response.set(http::field::server, "GateServer");
		//WriteResponse();
		break;
	}
	case http::verb::post:
	{
		//��ȡ·�ɲ��Ҵ����߼�ϵͳ�Ĵ���post����ĺ���
		bool success = LogicSystem::GetInstance()->HandlePost(_request.target(), shared_from_this());
		////���ʧ��
		//if (!success) {
		//	//���ý��
		//	_response.result(http::status::not_found);
		//	//���÷��͵�����,
		//	// "text/plain" ��һ�� MIME ���ͣ�Multipurpose Internet Mail Extensions��
		//	// ���ڱ�ʶ���ı����ݵ���������
		//	_response.set(http::field::content_type, "text/plain");
		//	//����body����
		//	beast::ostream(_response.body()) << "url not found\r\n";
		//	WriteResponse();
		//	return;
		//}

		//_response.result(http::status::ok);
		////�������ĸ����񷢹�ȥ��
		//_response.set(http::field::server, "GateServer");
		//WriteResponse();
		break;
	}
	default:
		break;
	}
}
