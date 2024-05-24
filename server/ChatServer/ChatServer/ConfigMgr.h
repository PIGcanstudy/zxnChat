#pragma once
#include "const.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <map>


/*
* 以下是一个config.ini的部分示例
* [GateServer]
* port = 8080
* address = 0.0.0.0
*/

//这个结构体对应的是config【】下方的键值对
// port, address 是key，8080,0.0.0.0是值
struct SectionInfo {
	SectionInfo(){}
	~SectionInfo() { _sections_datas.clear(); }
	SectionInfo(const SectionInfo& section) {
		this->_sections_datas = section._sections_datas;
	}
	SectionInfo& operator=(const SectionInfo& section) {
		//如果是一个对象
		if (&section == this) {
			return *this;
		}
		this->_sections_datas = section._sections_datas;
		return *this;
	}

	// 重载[]运算符，用来获取value值的同时检查是否存在
	std::string operator[](const std::string& key) {
		if (_sections_datas.find(key) == _sections_datas.end()) {
			return "";
		}
		return _sections_datas[key];
	}

	// 能多样性的获取值
	std::string GetValue(const std::string& key) {
		if (_sections_datas.find(key) == _sections_datas.end()) {
			return "";
		}
		return _sections_datas[key];
	}
	
	std::map<std::string, std::string> _sections_datas;
};

//对应的是[]里的与下面的几组键值对
class ConfigMgr
{
public:
	~ConfigMgr();

	static ConfigMgr& GetInstance() {
		static ConfigMgr ins;
		return ins;
	}

	//重载[]运算符，用来获取value值的同时检查是否存在
	SectionInfo operator[](const std::string& section);

	std::string GetValue(const std::string& section, const std::string& key);

private:
	ConfigMgr();
	ConfigMgr(const ConfigMgr& src) = delete;
	ConfigMgr operator=(const ConfigMgr& src) = delete;
	std::map<std::string, SectionInfo> _config_map;
};

