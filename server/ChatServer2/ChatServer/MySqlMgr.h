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
	// ע���û�
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	// ��������Ƿ����
	bool CheckEmail(const std::string& name, const std::string& email);
	// ��������
	bool UpdatePwd(const std::string& name, const std::string& pwd);
	// �������
	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userinfo);
	// ����Ӻ���ģ��
	bool AddFriendApply(const int& from, const int& to);
	// ��ȡ�����б�
	bool GetApplyList(const int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list, int begin, int limit);
	// ��֤����ģ��
	bool AuthFriendApply(const int& from, const int& to);
	// ��Ӻ���ģ��
	bool AddFriend(const int& from, const int& to, std::string back_name);
	// ��ȡ����ģ��
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info);
	// ��ȡuser�ӿ�
	std::shared_ptr<UserInfo> GetUser(int uid);
	std::shared_ptr<UserInfo> GetUser(std::string& name);
private:
	MysqlMgr();
	MysqlDao  _dao;
};