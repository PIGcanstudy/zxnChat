#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VarifyGrpcClient.h"
LogicSystem::~LogicSystem()
{
}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> connection)
{
	//如果在存放get请求的map里没找到对应路径 则直接返回false
	if (_get_handlers.find(path) == _get_handlers.end()) {
		return false;
	}
	
	//调用处理函数
	_get_handlers[path](connection);
	return true;
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> connection)
{
    //如果在存放get请求的map里没找到对应路径 则直接返回false
    if (_post_handlers.find(path) == _post_handlers.end()) {
        return false;
    }

    //调用处理函数
    _post_handlers[path](connection);
    return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
	_get_handlers.insert(std::make_pair(url, handler));
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
	_post_handlers.insert(std::make_pair(url, handler));
}

LogicSystem::LogicSystem()
{
	RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
		beast::ostream(connection->_response.body()) << "receive get_test req" << std::endl;
		int i = 0;
		for (auto& elem : connection->_get_params) {
			i++;
			beast::ostream(connection->_response.body()) << "param" << i << " key is " << elem.first;
			beast::ostream(connection->_response.body()) << " value is " << elem.second << std::endl;
		}
		});

    RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
        //把缓冲区的内容解析成string
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        // 设置返回的类型
        connection->_response.set(http::field::content_type, "text/json");
        
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;

        //判断解析是否成功，从body_str解析到src_root
        bool parse_success = reader.parse(body_str, src_root);
        
        //参数不对的话
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        //没有Email参数成员
        if (!src_root.isMember("email")) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        //成功
        auto email = src_root["email"].asString();

        //调用其他接口的响应
        auto rsp = VarifyGrpcClient::GetInstance()->GetVarifyCode(email);

        std::cout << "email is " << email << std::endl;
        root["error"] = rsp.error();
        root["email"] = src_root["email"];
        //因为tcp是基于字节流的，所以要把json转为字符串
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
        });
}


