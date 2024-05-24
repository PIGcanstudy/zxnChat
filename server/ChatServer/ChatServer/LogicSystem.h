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
	// ������Ϣ
	void DealMsg();
	//ע�Ṧ��
	void RegisterCallBacks();
	// ��¼�߼��Ĵ���
	void LoginHandler(shared_ptr<CSession>, const short& msg_id, const string& msg_data);
	//����Ҫ�����߳��������߼���Ϣ
	std::thread _worker_thread;
	//�����洢�߼��ڵ�
	std::queue<shared_ptr<LogicNode>> _msg_que;
	std::mutex _mutex;
	//��������Ϣ���У�������Ҫ�����̣߳�������Ҫ
	//����������֪ͨ���������߳�
	std::condition_variable _consume;
	//ʹ�����������߼�ֹͣ
	bool _b_stop;
	//�����洢�ص�������Ҳ����ע�����
	std::unordered_map<short, FunCallBack> _fun_callbacks;
	// ��¼�û�����Ϣ
	std::unordered_map<int, std::shared_ptr<UserInfo>> _users;
};

