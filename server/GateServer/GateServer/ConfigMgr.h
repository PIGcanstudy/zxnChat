#pragma once
#include "const.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <map>


/*
* ������һ��config.ini�Ĳ���ʾ��
* [GateServer]
* port = 8080
* address = 0.0.0.0
*/

//����ṹ���Ӧ����config�����·��ļ�ֵ��
// port, address ��key��8080,0.0.0.0��ֵ
struct SectionInfo {
	SectionInfo(){}
	~SectionInfo() { _sections_datas.clear(); }
	SectionInfo(const SectionInfo& section) {
		this->_sections_datas = section._sections_datas;
	}
	SectionInfo& operator=(const SectionInfo& section) {
		//�����һ������
		if (&section == this) {
			return *this;
		}
		this->_sections_datas = section._sections_datas;
		return *this;
	}

	// ����[]�������������ȡvalueֵ��ͬʱ����Ƿ����
	std::string operator[](const std::string& key) {
		if (_sections_datas.find(key) == _sections_datas.end()) {
			return "";
		}
		return _sections_datas[key];
	}

	// �ܶ����ԵĻ�ȡֵ
	std::string GetValue(const std::string& key) {
		if (_sections_datas.find(key) == _sections_datas.end()) {
			return "";
		}
		return _sections_datas[key];
	}
	
	std::map<std::string, std::string> _sections_datas;
};

//��Ӧ����[]���������ļ����ֵ��
class ConfigMgr
{
public:
	~ConfigMgr();

	static ConfigMgr& GetInstance() {
		static ConfigMgr ins;
		return ins;
	}

	//����[]�������������ȡvalueֵ��ͬʱ����Ƿ����
	SectionInfo operator[](const std::string& section);

	std::string GetValue(const std::string& section, const std::string& key);

private:
	ConfigMgr();
	ConfigMgr(const ConfigMgr& src) = delete;
	ConfigMgr operator=(const ConfigMgr& src) = delete;
	std::map<std::string, SectionInfo> _config_map;
};

