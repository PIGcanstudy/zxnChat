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
	// �첽��ȡʵ������
	void AsyncReadBody(int length);
	// �첽��ȡͷ���ڵ�
	void AsyncReadHead(int total_len);

	// ��������ʱ��
	void updateHeartbeat();

	// ��ȡ��һ��������ʱ��
	void GetLastHeartbeat() const;

	// ����Ƿ�ʱ
	bool isTimedOut(int timeout_seconds) const;


private:
	void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code& , std::size_t)> handler);
	void asyncReadLen(std::size_t  read_len, std::size_t total_len,
		std::function<void(const boost::system::error_code&, std::size_t)> handler);
	
	
	void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self);
	tcp::socket _socket;
	std::string _session_uuid;
	int _user_uid;

	// ������ʱ�洢���յ����ݡ�
	char _data[MAX_LENGTH];

	// Ϊ�˵���CServer��clearCSessions����
	CServer* _server;
	// ��ʾ�Ƿ�ر�
	bool _b_close;
	// ������Ϣ����Ϣ���У�������֤������Ϣ���������Լ�
	std::queue<shared_ptr<SendNode>> _send_que;
	// ���ڹ�����Դ�Ļ���
	std::mutex _send_lock;
	//�յ�����Ϣ�ṹ
	std::shared_ptr<RecvNode> _recv_msg_node;
	//�ж�ͷ���ڵ��Ƿ������
	bool _b_head_parse;
	//�յ���ͷ���ṹ
	std::shared_ptr<MsgNode> _recv_head_node;

	// Session��Ӧ��һ�οͻ��˷���������Ϣ��ʱ��
	TimePoint last_heartbeat_time;

};

class LogicNode {
	friend class LogicSystem;
public:
	LogicNode(shared_ptr<CSession>, shared_ptr<RecvNode>);
private:
	// �����߼��Ǵ��ĸ�session������
	shared_ptr<CSession> _session;
	// �������ʲô��Ϣ
	shared_ptr<RecvNode> _recvnode;
};
