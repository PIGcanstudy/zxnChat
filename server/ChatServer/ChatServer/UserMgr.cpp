#include "UserMgr.h"
#include "CSession.h"
#include "RedisMgr.h"

UserMgr::~UserMgr() {
	uid_to_session_.clear();
}

// 根据id得到对应CSession
std::shared_ptr<CSession> UserMgr::GetSession(int uid) {
	std::lock_guard<std::mutex> lock(session_mtx_);
	auto it = uid_to_session_.find(uid);
	if (it == uid_to_session_.end()) return nullptr;
	return it->second;
}

// 建立id到CSession的映射
void UserMgr::SetUserSession(int uid, std::shared_ptr<CSession> session) {
	std::lock_guard<std::mutex> lock(session_mtx_);
	uid_to_session_[uid] = session;
}

// 移除id到CSesison的映射
void UserMgr::RmvUserSession(int uid) {
	
	auto uid_str = std::to_string(uid);
	//因为再次登录可能是其他服务器，所以会造成本服务器删除key，其他服务器注册key的情况
	// 有可能其他服务登录，本服删除key造成找不到key的情况

	//RedisMgr::GetInstance()->Del(USERIPPREFIX + uid_str);

	std::lock_guard<std::mutex> lock(session_mtx_);
	auto it = uid_to_session_.find(uid);
	if (it != uid_to_session_.end()) uid_to_session_.erase(uid);
}