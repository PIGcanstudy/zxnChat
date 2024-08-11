#pragma once
#include "MySqlPool.h"
#include "data.h"

class MysqlDao
{
public:
	MysqlDao();
	~MysqlDao();
	// 用于注册用户
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	// 检查邮箱是否存在
	bool CheckEmail(const std::string& name, const std::string& email);
	// 更新密码
	bool UpdatePwd(const std::string& name, const std::string& newpwd);
	// 检测密码是否存在
	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo);
	// 申请加好友
	bool AddFriendApply(const int& from, const int& to);
	// 获取申请列表
	bool GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit);
	// 认证好友模块
	bool AuthFriendApply(const int& from, const int& to);
	// 添加好友模块
	bool AddFriend(const int& from, const int& to, std::string back_name);
	// 获取好友模块
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info_list);
	// 从数据库中根据uid获取user信息
	std::shared_ptr<UserInfo> GetUser(int uid);
	// 从数据库中根据name获取user信息
	std::shared_ptr<UserInfo> GetUser(std::string& name);
private:
	std::unique_ptr<MySqlPool> _pool;
	UserInfo userInfo;
};

