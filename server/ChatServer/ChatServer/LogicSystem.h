#pragma once
#include "Singleton.h"
#include <queue>
#include <thread>
#include "CSession.h"
#include <queue>
#include <map>
#include <functional>
#include "const.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <unordered_map>
#include "data.h"

typedef  function<void(shared_ptr<CSession>, const short& msg_id, const string& msg_data)> FunCallBack;

class LogicSystem :public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	void PostMsgToQue(shared_ptr < LogicNode> msg);
private:
	LogicSystem();
	// 处理信息
	void DealMsg();
	//注册功能
	void RegisterCallBacks();
	// 登录逻辑的处理
	void LoginHandler(shared_ptr<CSession>, const short& msg_id, const string& msg_data);
	// 消息逻辑处理投递给线程池
	void ProcessMessage(const std::shared_ptr<LogicNode>& msg_node);

	// 获取用户信息
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	//还需要工作线程来消费逻辑消息
	std::thread _worker_thread;
	//用来存储逻辑节点
	std::queue<shared_ptr<LogicNode>> _msg_que;
	std::mutex _mutex;
	//由于有消息队列，所以需要阻塞线程，所以需要
	//条件变量来通知被阻塞的线程
	std::condition_variable _consume;
	//使用这个来标记逻辑停止
	bool _b_stop;
	//用来存储回调函数，也就是注册进来
	std::unordered_map<short, FunCallBack> _fun_callbacks;
};

