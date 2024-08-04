#pragma once

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,  //Json��������
	RPCFailed = 1002,  //RPC�������
	VarifyExpired = 1003, //��֤�����
	VarifyCodeErr = 1004, //��֤�����
	UserExist = 1005,       //�û��Ѿ�����
	PasswdErr = 1006,    //�������
	EmailNotMatch = 1007,  //���䲻ƥ��
	PasswdUpFailed = 1008,  //��������ʧ��
	PasswdInvalid = 1009,   //�������ʧ��
	TokenInvalid = 1010,   //TokenʧЧ
	UidInvalid = 1011,  //uid��Ч
};

// Defer��, ������һ������������ִ�б���Ҫִ�еĶ���
// ���Լ򻯴����ʵ�֣���Ϊ��������ʱ��Ҫ����ָ��Ļص�����
class Defer {
public:
	// ����һ��lambda���ʽ���ߺ���ָ��
	Defer(std::function<void()> func) : func_(func) {}

	// ����������ִ�д���ĺ���
	~Defer() {
		func_();
	}

private:
	std::function<void()> func_;
};

// ���յ���󳤶�
constexpr int MAX_LENGTH = 1024 * 2;
// ���սڵ���������
constexpr int MAX_RECVQUE = 10000;
// ���ͽڵ���������
constexpr int MAX_SENDQUE = 1000;
// ͷ�����ܳ���
constexpr int HEAD_TOTAL_LEN = 4;
// id���ֽ���
constexpr int HEAD_ID_LEN = 2;
// ���ȵ��ֽ���
constexpr int HEAD_DATA_LEN = 2;


enum MSG_IDS {
	MSG_CHAT_LOGIN = 1005, //�û���½
	MSG_CHAT_LOGIN_RSP = 1006, //�û���½�ذ�
};