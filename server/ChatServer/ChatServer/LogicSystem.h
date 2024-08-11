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
	// ������Ϣ
	void DealMsg();
	//ע�Ṧ��
	void RegisterCallBacks();
	// ��¼�߼��Ĵ���
	void LoginHandler(std::shared_ptr<CSession>, const short& msg_id, const std::string& msg_data);

	// ��ȡ�����б�
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list);

	// �õ�����������Ϣ�Ĵ���
	bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list);
	
	// ���Һ����߼��Ĵ���
	void SearchUserHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);

	// ��������߼��Ĵ���
	void AddFriendApplyHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);

	// ��֤�����߼��Ĵ���
	void AuthFriendApplyHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);

	// �ж��ǲ��Ǵ�����
	bool isPureDigit(const std::string& str);

	// �Ǵ����־͸���uid��
	void GetUserByUid(std::string uid_str, Json::Value& rtvalue);

	// ���Ǿ͸���name��
	void GetUserByName(std::string name, Json::Value& rtvalue);

	// ��Ϣ�߼�����Ͷ�ݸ��̳߳�
	void ProcessMessage(const std::shared_ptr<LogicNode>& msg_node);

	// ��ȡ�û���Ϣ
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	//����Ҫ�����߳��������߼���Ϣ
	std::thread _worker_thread;
	//�����洢�߼��ڵ�
	std::queue<std::shared_ptr<LogicNode>> _msg_que;
	std::mutex _mutex;
	//��������Ϣ���У�������Ҫ�����̣߳�������Ҫ
	//����������֪ͨ���������߳�
	std::condition_variable _consume;
	//ʹ�����������߼�ֹͣ
	bool _b_stop;
	//�����洢�ص�������Ҳ����ע�����
	std::unordered_map<short, FunCallBack> _fun_callbacks;
};

