#include "CSession.h"
#include "CServer.h"
#include <iostream>
#include <sstream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "LogicSystem.h"

// 生成uuid和构造头节点
CSession::CSession(boost::asio::io_context& io_context, CServer* server):
	_socket(io_context), _server(server), _b_close(false),_b_head_parse(false){
	// 初始化成员变量
	memset(_data, 0, MAX_LENGTH);
	// 生成session的uuid
	boost::uuids::uuid  a_uuid = boost::uuids::random_generator()();
	// 转换为string
	_session_uuid = boost::uuids::to_string(a_uuid);
	// 构造头部节点
	_recv_head_node = make_shared<MsgNode>(HEAD_TOTAL_LEN);
}
CSession::~CSession() {
	Close();
	std::cout << "~CSession destruct" << endl;
}

tcp::socket& CSession::GetSocket() {
	return _socket;
}

std::string& CSession::GetSessionUuid() {
	return _session_uuid;
}

void CSession::SetUserId(int uid)
{
	_user_uid = uid;
}

int CSession::GetUserId()
{
	return _user_uid;
}

void CSession::Start(){
	// 接收数据头节点
	AsyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::Send(std::string msg, short msgid) {
	std::lock_guard<std::mutex> lock(_send_lock);
	int send_que_size = _send_que.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << _session_uuid << " send que fulled, size is " << MAX_SENDQUE << endl;
		return;
	}

	_send_que.push(make_shared<SendNode>(msg.c_str(), msg.length(), msgid));
	
	// 如果消息投递到队列前，队列已经有数据了，就先不调用发送逻辑，也就是async_write
	if (send_que_size > 0) {
		return;
	}
	// 取出头部节点
	auto& msgnode = _send_que.front();
	boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::Send(char* msg, short max_length, short msgid) {
	std::lock_guard<std::mutex> lock(_send_lock);
	int send_que_size = _send_que.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << _session_uuid << " send que fulled, size is " << MAX_SENDQUE << endl;
		return;
	}

	_send_que.push(make_shared<SendNode>(msg, max_length, msgid));
	if (send_que_size>0) {
		return;
	}
	auto& msgnode = _send_que.front();
	boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len), 
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::Close() {
	_socket.close();
	_b_close = true;
}

std::shared_ptr<CSession>CSession::SharedSelf() {
	return shared_from_this();
}

void CSession::AsyncReadBody(int total_len)
{
	auto self = shared_from_this();
	// 必须读完整的数据，后者为回调函数
	asyncReadFull(total_len, [self, this, total_len](const boost::system::error_code& ec, std::size_t bytes_transfered) {
		try {
			if (ec) {
				std::cout << "handle read failed, error is " << ec.what() << endl;
				Close();
				_server->ClearSession(_session_uuid);
				return;
			}


			// 读到了完整数据
			memcpy(_recv_msg_node->_data , _data , bytes_transfered);
			_recv_msg_node->_cur_len += bytes_transfered;
			_recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
			cout << "receive data is " << _recv_msg_node->_data << endl;
			//此处将消息投递到逻辑队列中
			LogicSystem::GetInstance()->PostMsgToQue(make_shared<LogicNode>(shared_from_this(), _recv_msg_node));
			//继续监听头部接受事件
			AsyncReadHead(HEAD_TOTAL_LEN);
		}
		catch (std::exception& e) {
			std::cout << "Exception code is " << e.what() << endl;
		}
		});
}

void CSession::AsyncReadHead(int total_len)
{
	auto self = shared_from_this();
	// bytes_transfered 表示当前已经接受了多长
	asyncReadFull(HEAD_TOTAL_LEN, [self, this](const boost::system::error_code& ec, std::size_t bytes_transfered) {
		try {
			if (ec) {
				std::cout << "handle read failed, error is " << ec.what() << endl;
				// 关闭session
				Close();
				_server->ClearSession(_session_uuid);
				return;
			}

			// 头部节点不够长度
			if (bytes_transfered < HEAD_TOTAL_LEN) {
				std::cout << "read length not match, read [" << bytes_transfered << "] , total ["
					<< HEAD_TOTAL_LEN << "]" << endl;
				// 关闭sessison
				Close();
				_server->ClearSession(_session_uuid);
				return;
			}

			// 足够读取头部长度

			// 清理头部节点
			_recv_head_node->Clear();

			// 将之前接收并缓存的头部长度复制
			memcpy(_recv_head_node->_data, _data, bytes_transfered);

			//获取头部MSGID数据
			short msg_id = 0;
			
			// 获取头部的id
			memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);

			//网络字节序转化为本地字节序
			msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);

			std::cout << "msg_id is " << msg_id << endl;

			//id非法
			if (msg_id > MAX_LENGTH) {
				std::cout << "invalid msg_id is " << msg_id << endl;

				_server->ClearSession(_session_uuid);
				return;
			}

			// 用来获取头部的MSGLEN数据
			short msg_len = 0;

			memcpy(&msg_len, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);

			//网络字节序转化为本地字节序
			msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);

			std::cout << "msg_len is " << msg_len << endl;

			//长度非法
			if (msg_len > MAX_LENGTH) {
				std::cout << "invalid data length is " << msg_len << endl;
				_server->ClearSession(_session_uuid);
				return;
			}

			// 构造头部节点
			_recv_msg_node = make_shared<RecvNode>(msg_len, msg_id);

			// 开始读实际数据
			AsyncReadBody(msg_len);
		}
		catch (std::exception& e) {
			std::cout << "Exception code is " << e.what() << endl;
		}
		});
}

void CSession::HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self) {
	//增加异常处理
	try {
		if (!error) {
			std::lock_guard<std::mutex> lock(_send_lock);
			//cout << "send data " << _send_que.front()->_data+HEAD_LENGTH << endl;
			_send_que.pop();
			if (!_send_que.empty()) {
				auto& msgnode = _send_que.front();
				boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
					std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_self));
			}
		}
		else {
			std::cout << "handle write failed, error is " << error.what() << endl;
			Close();
			_server->ClearSession(_session_uuid);
		}
	}
	catch (std::exception& e) {
		std::cerr << "Exception code : " << e.what() << endl;
	}
	
}

//读取完整长度
void CSession::asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code&, std::size_t)> handler )
{
	// 将接收的数据清空
	::memset(_data, 0, MAX_LENGTH);

	// 开始读取指定长度的数据
	asyncReadLen(0, maxLength, handler);
}

//读取指定字节数，read_len表示已经读取的长度
void CSession::asyncReadLen(std::size_t read_len, std::size_t total_len, 
	std::function<void(const boost::system::error_code&, std::size_t)> handler)
{
	auto self = shared_from_this();
	_socket.async_read_some(boost::asio::buffer(_data + read_len, total_len - read_len),
		[read_len, total_len, handler, self](const boost::system::error_code& ec, std::size_t  bytesTransfered) {
			if (ec) {
				// 出现错误，调用回调函数
				handler(ec, read_len + bytesTransfered);
				return;
			}

			if (read_len + bytesTransfered >= total_len) {
				//长度够了就调用回调函数
				handler(ec, read_len + bytesTransfered);
				return;
			}

			// 没有错误，且长度不足则继续读取
			self->asyncReadLen(read_len + bytesTransfered, total_len, handler);
	});
}

LogicNode::LogicNode(shared_ptr<CSession>  session, 
	shared_ptr<RecvNode> recvnode):_session(session),_recvnode(recvnode) {
	
}
