#pragma once
/*
* 对应调用 DAO 的 Server 层
*/
#include "MysqlDao.h"
#include "Singleton.h"
#include <string>
class MysqlMgr : public Singleton<MysqlMgr>
{
	friend class Singleton<MysqlMgr>;
public:
	~MysqlMgr();
	// 注册用户
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	// 检查邮箱是否存在
	bool CheckEmail(const std::string& name, const std::string& email);
	// 更新密码
	bool UpdatePwd(const std::string& name, const std::string& pwd);
	// 检查密码
	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo);
	// 申请加好友模块
	bool AddFriendApply(const int& from, const int& to);
	// 获取申请列表
	bool GetApplyList(const int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list, int begin, int limit);
	// 认证好友模块
	bool AuthFriendApply(const int& from, const int& to);
	// 添加好友模块
	bool AddFriend(const int& from, const int& to, std::string back_name);
	// 获取好友模块
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info);
	// 获取user接口
	std::shared_ptr<UserInfo> GetUser(int uid);
	std::shared_ptr<UserInfo> GetUser(std::string& name);
private:
	MysqlMgr();
	MysqlDao  _dao;
};