#include "UserMgr.h"
#include "CSession.h"
#include "RedisMgr.h"

UserMgr::~UserMgr() {
	uid_to_session_.clear();
}

// ����id�õ���ӦCSession
std::shared_ptr<CSession> UserMgr::GetSession(int uid) {
	std::lock_guard<std::mutex> lock(session_mtx_);
	auto it = uid_to_session_.find(uid);
	if (it == uid_to_session_.end()) return nullptr;
	return it->second;
}

// ����id��CSession��ӳ��
void UserMgr::SetUserSession(int uid, std::shared_ptr<CSession> session) {
	std::lock_guard<std::mutex> lock(session_mtx_);
	uid_to_session_[uid] = session;
}

// �Ƴ�id��CSesison��ӳ��
void UserMgr::RmvUserSession(int uid) {
	
	auto uid_str = std::to_string(uid);
	//��Ϊ�ٴε�¼���������������������Ի���ɱ�������ɾ��key������������ע��key�����
	// �п������������¼������ɾ��key����Ҳ���key�����

	//RedisMgr::GetInstance()->Del(USERIPPREFIX + uid_str);

	std::lock_guard<std::mutex> lock(session_mtx_);
	auto it = uid_to_session_.find(uid);
	if (it != uid_to_session_.end()) uid_to_session_.erase(uid);
}