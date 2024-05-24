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
	// 从数据库中获取user信息
	std::shared_ptr<UserInfo> GetUser(int uid);
private:
	std::unique_ptr<MySqlPool> _pool;
	UserInfo userInfo;
};

