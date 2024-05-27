#pragma once
#include "Singleton.h"
#include "ThreadPool.h"
#include <functional>
#include <map>

//防止互相引用，先声明，然后在源文件包含这个类的头文件
class HttpConnection;

//定义一个类型的别名，这个类型是一个函数，用来处理
//有一个参数，接受一个Http连接类
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem: public Singleton<LogicSystem>
{
public:
	friend class Singleton<LogicSystem>;
	~LogicSystem();

	//处理get请求
	bool HandleGet(std::string path, std::shared_ptr<HttpConnection> connection);
	
	//处理post请求
	bool HandlePost(std::string path, std::shared_ptr<HttpConnection> connection);

	//用来注册Get请求
	void RegGet(std::string url, HttpHandler handler);

	//用来注册Post请求
	void RegPost(std::string url, HttpHandler handler);

private:
	LogicSystem(); 
	//处理Post请求的集合
	std::map<std::string, HttpHandler> _post_handlers;

	//处理Get请求的集合
	std::map<std::string, HttpHandler> _get_handlers;
};

