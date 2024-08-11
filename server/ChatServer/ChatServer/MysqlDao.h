#pragma once
#include "MySqlPool.h"
#include "data.h"

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
	// ����Ӻ���
	bool AddFriendApply(const int& from, const int& to);
	// ��ȡ�����б�
	bool GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit);
	// ��֤����ģ��
	bool AuthFriendApply(const int& from, const int& to);
	// ��Ӻ���ģ��
	bool AddFriend(const int& from, const int& to, std::string back_name);
	// ��ȡ����ģ��
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info_list);
	// �����ݿ��и���uid��ȡuser��Ϣ
	std::shared_ptr<UserInfo> GetUser(int uid);
	// �����ݿ��и���name��ȡuser��Ϣ
	std::shared_ptr<UserInfo> GetUser(std::string& name);
private:
	std::unique_ptr<MySqlPool> _pool;
	UserInfo userInfo;
};

