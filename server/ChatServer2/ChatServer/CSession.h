#pragma once
#include <boost/asio.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <queue>
#include <mutex>
#include <memory>
#include "const.h"
#include "MsgNode.h"
using namespace std;


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


class CServer;
class LogicSystem;

class CSession: public std::enable_shared_from_this<CSession>
{
public:
	using Clock = std::chrono::system_clock;
	using TimePoint = std::chrono::time_point<Clock>;
	CSession(boost::asio::io_context& io_context, CServer* server);
	~CSession();
	tcp::socket& GetSocket();
	std::string& GetSessionUuid();
	int GetUserId();
	void SetUserId(int uid);
	void Start();
	void Send(char* msg,  short max_length, short msgid);
	void Send(std::string msg, short msgid);
	void Close();
	std::shared_ptr<CSession> SharedSelf();
	// 异步读取实际数据
	void AsyncReadBody(int length);
	// 异步读取头部节点
	void AsyncReadHead(int total_len);

	// 更新心跳时间
	void updateHeartbeat();

	// 获取上一次心跳的时间
	void GetLastHeartbeat() const;

	// 检查是否超时
	bool isTimedOut(int timeout_seconds) const;


private:
	void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code& , std::size_t)> handler);
	void asyncReadLen(std::size_t  read_len, std::size_t total_len,
		std::function<void(const boost::system::error_code&, std::size_t)> handler);
	
	
	void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self);
	tcp::socket _socket;
	std::string _session_uuid;
	int _user_uid;

	// 用于临时存储接收的数据。
	char _data[MAX_LENGTH];

	// 为了调用CServer的clearCSessions函数
	CServer* _server;
	// 表示是否关闭
	bool _b_close;
	// 发送信息的消息队列，用来保证发送信息的有序性以及
	std::queue<shared_ptr<SendNode>> _send_que;
	// 用于共享资源的互斥
	std::mutex _send_lock;
	//收到的消息结构
	std::shared_ptr<RecvNode> _recv_msg_node;
	//判断头部节点是否构造完成
	bool _b_head_parse;
	//收到的头部结构
	std::shared_ptr<MsgNode> _recv_head_node;

	// Session对应上一次客户端发送心跳信息的时间
	TimePoint last_heartbeat_time;

};

class LogicNode {
	friend class LogicSystem;
public:
	LogicNode(shared_ptr<CSession>, shared_ptr<RecvNode>);
private:
	// 保存逻辑是从哪个session中来的
	shared_ptr<CSession> _session;
	// 处理的是什么信息
	shared_ptr<RecvNode> _recvnode;
};
