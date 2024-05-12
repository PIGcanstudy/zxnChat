#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VarifyGrpcClient.h"
LogicSystem::~LogicSystem()
{
}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> connection)
{
	//����ڴ��get�����map��û�ҵ���Ӧ·�� ��ֱ�ӷ���false
	if (_get_handlers.find(path) == _get_handlers.end()) {
		return false;
	}
	
	//���ô�����
	_get_handlers[path](connection);
	return true;
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> connection)
{
    //����ڴ��get�����map��û�ҵ���Ӧ·�� ��ֱ�ӷ���false
    if (_post_handlers.find(path) == _post_handlers.end()) {
        return false;
    }

    //���ô�����
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
        //�ѻ����������ݽ�����string
        auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        // ���÷��ص�����
        connection->_response.set(http::field::content_type, "text/json");
        
        Json::Value root;
        Json::Reader reader;
        Json::Value src_root;

        //�жϽ����Ƿ�ɹ�����body_str������src_root
        bool parse_success = reader.parse(body_str, src_root);
        
        //�������ԵĻ�
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        //û��Email������Ա
        if (!src_root.isMember("email")) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = ErrorCodes::Error_Json;
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->_response.body()) << jsonstr;
            return true;
        }

        //�ɹ�
        auto email = src_root["email"].asString();

        //���������ӿڵ���Ӧ
        auto rsp = VarifyGrpcClient::GetInstance()->GetVarifyCode(email);

        std::cout << "email is " << email << std::endl;
        root["error"] = rsp.error();
        root["email"] = src_root["email"];
        //��Ϊtcp�ǻ����ֽ����ģ�����Ҫ��jsonתΪ�ַ���
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return true;
        });
}


