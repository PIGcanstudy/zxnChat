#pragma once
/*
* ��Ӧ���� DAO �� Server ��
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
	// ��������Ƿ����
	bool CheckEmail(const std::string& name, const std::string& email);
	// ��������
	bool UpdatePwd(const std::string& name, const std::string& pwd);

	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo);
private:
	MysqlMgr();
	MysqlDao  _dao;
};