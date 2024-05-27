#pragma once
#include "Singleton.h"
#include "ThreadPool.h"
#include <functional>
#include <map>

//��ֹ�������ã���������Ȼ����Դ�ļ�����������ͷ�ļ�
class HttpConnection;

//����һ�����͵ı��������������һ����������������
//��һ������������һ��Http������
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem: public Singleton<LogicSystem>
{
public:
	friend class Singleton<LogicSystem>;
	~LogicSystem();

	//����get����
	bool HandleGet(std::string path, std::shared_ptr<HttpConnection> connection);
	
	//����post����
	bool HandlePost(std::string path, std::shared_ptr<HttpConnection> connection);

	//����ע��Get����
	void RegGet(std::string url, HttpHandler handler);

	//����ע��Post����
	void RegPost(std::string url, HttpHandler handler);

private:
	LogicSystem(); 
	//����Post����ļ���
	std::map<std::string, HttpHandler> _post_handlers;

	//����Get����ļ���
	std::map<std::string, HttpHandler> _get_handlers;
};

