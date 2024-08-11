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

typedef  function<void(std::shared_ptr<CSession>, const short& msg_id, const std::string& msg_data)> FunCallBack;

class LogicSystem :public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	void PostMsgToQue(std::shared_ptr < LogicNode> msg);
private:
	LogicSystem();
	// 处理信息
	void DealMsg();
	//注册功能
	void RegisterCallBacks();
	// 登录逻辑的处理
	void LoginHandler(std::shared_ptr<CSession>, const short& msg_id, const std::string& msg_data);

	// 获取好友列表
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list);

	// 得到还有申请信息的处理
	bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list);
	
	// 查找好友逻辑的处理
	void SearchUserHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);

	// 申请好友逻辑的处理
	void AddFriendApplyHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);

	// 认证好友逻辑的处理
	void AuthFriendApplyHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);

	// 判断是不是纯数字
	bool isPureDigit(const std::string& str);

	// 是纯数字就根据uid查
	void GetUserByUid(std::string uid_str, Json::Value& rtvalue);

	// 不是就根据name查
	void GetUserByName(std::string name, Json::Value& rtvalue);

	// 消息逻辑处理投递给线程池
	void ProcessMessage(const std::shared_ptr<LogicNode>& msg_node);

	// 获取用户信息
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	//还需要工作线程来消费逻辑消息
	std::thread _worker_thread;
	//用来存储逻辑节点
	std::queue<std::shared_ptr<LogicNode>> _msg_que;
	std::mutex _mutex;
	//由于有消息队列，所以需要阻塞线程，所以需要
	//条件变量来通知被阻塞的线程
	std::condition_variable _consume;
	//使用这个来标记逻辑停止
	bool _b_stop;
	//用来存储回调函数，也就是注册进来
	std::unordered_map<short, FunCallBack> _fun_callbacks;
};

