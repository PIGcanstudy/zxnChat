#pragma once
#include "Singleton.h"
#include <unordered_map>
#include <memory>
#include <mutex>

class CSession;
// �������������id��һ��CSession��������ʾһ���û�
class UserMgr : public Singleton<UserMgr>
{
	friend class Singleton<UserMgr>;
public:
	~UserMgr();
	// ����id�õ���ӦCSession
	std::shared_ptr<CSession> GetSession(int uid);
	// ����id��CSession��ӳ��
	void SetUserSession(int uid, std::shared_ptr<CSession> session);
	// �Ƴ�id��CSesison��ӳ��
	void RmvUserSession(int uid);
private:
	UserMgr() = default;
	// �����������
	std::mutex session_mtx_;
	// uid��һ��CSession��ӳ��
	std::unordered_map<int, std::shared_ptr<CSession>> uid_to_session_;
};

