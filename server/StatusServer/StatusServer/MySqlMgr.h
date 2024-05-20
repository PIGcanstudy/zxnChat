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
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	// 检查邮箱是否存在
	bool CheckEmail(const std::string& name, const std::string& email);
	// 更新密码
	bool UpdatePwd(const std::string& name, const std::string& pwd);

	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo);
private:
	MysqlMgr();
	MysqlDao  _dao;
};