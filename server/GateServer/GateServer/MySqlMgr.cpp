#include "MySqlMgr.h"

MysqlMgr::~MysqlMgr()
{
}

int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	return _dao.RegUser(name, email, pwd);
}

bool MysqlMgr::UpdatePwd(const std::string& name, const std::string& pwd)
{
	return _dao.UpdatePwd(name, pwd);
}

bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email)
{
	return _dao.CheckEmail(name, email);
}

MysqlMgr::MysqlMgr()
{
}


bool MysqlMgr::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo)
{
	return _dao.CheckPwd(email, pwd, userinfo);
}
