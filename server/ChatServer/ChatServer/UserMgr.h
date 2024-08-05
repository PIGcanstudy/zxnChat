#pragma once
#include "Singleton.h"
#include <unordered_map>
#include <memory>
#include <mutex>

class CSession;
// 这个类是用来将id绑定一个CSession，用来表示一个用户
class UserMgr : public Singleton<UserMgr>
{
	friend class Singleton<UserMgr>;
public:
	~UserMgr();
	// 根据id得到对应CSession
	std::shared_ptr<CSession> GetSession(int uid);
	// 建立id到CSession的映射
	void SetUserSession(int uid, std::shared_ptr<CSession> session);
	// 移除id到CSesison的映射
	void RmvUserSession(int uid);
private:
	UserMgr() = default;
	// 用来互斥访问
	std::mutex session_mtx_;
	// uid到一个CSession的映射
	std::unordered_map<int, std::shared_ptr<CSession>> uid_to_session_;
};

