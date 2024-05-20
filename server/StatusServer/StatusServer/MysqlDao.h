#pragma once
#include "MySqlPool.h"

// �û��ӿ�
struct UserInfo {
	std::string name;
	std::string pwd;
	int uid;
	std::string email;
};

class MysqlDao
{
public:
	MysqlDao();
	~MysqlDao();
	// ����ע���û�
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	// ��������Ƿ����
	bool CheckEmail(const std::string& name, const std::string& email);
	// ��������
	bool UpdatePwd(const std::string& name, const std::string& newpwd);
	// ��������Ƿ����
	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo);
private:
	std::unique_ptr<MySqlPool> _pool;
	UserInfo userInfo;
};

