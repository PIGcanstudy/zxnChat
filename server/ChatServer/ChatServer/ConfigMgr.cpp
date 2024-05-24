#include "ConfigMgr.h"

ConfigMgr::~ConfigMgr()
{
	_config_map.clear();
}

SectionInfo ConfigMgr::operator[](const std::string& section)
{
	if (_config_map.find(section) == _config_map.end()) {
		return SectionInfo{};
	}
	
	return _config_map[section];
}

std::string ConfigMgr::GetValue(const std::string& section, const std::string& key)
{
	if (_config_map.find(section) == _config_map.end()) {
		return "";
	}

	return _config_map[section].GetValue(key);
}

ConfigMgr::ConfigMgr()
{
	//��ȡ��ǰ����Ŀ¼
	boost::filesystem::path current_path = boost::filesystem::current_path();
	//����config.ini�ļ�������·��
	boost::filesystem::path config_path = current_path / "config.ini";
	std::cout << "Config path: " << config_path << std::endl;

	// ʹ��boost.propertytree����ȡini�ļ�
	/*
	* ���ڴ������νṹ������,ͨ�����ڽ���������
	* �����ɸ��������ļ���XML �ļ���JSON �ļ���
	*/
	boost::property_tree::ptree pt;
	// ��ȡ���������
	boost::property_tree::read_ini(config_path.string(), pt);

	// ����INI�ļ��е�����section
	for (const auto& section_pair : pt) {
		// ��Ϊ[]���GateServer
		const std::string& section_name = section_pair.first;
		// ����[]����һ���ֵ��
		const boost::property_tree::ptree& section_tree = section_pair.second;

		//�洢�����õ��ļ�ֵ
		std::map<std::string, std::string> section_config;
		
		// ����ÿ��section�����������е�key-value��  
		for (const auto& key_value_pair : section_tree) {
			// =ǰ��� ����port
			const std::string& key = key_value_pair.first;
			// =������� ����8080�����ڷ��صĻ���ptree ����ֱ��ת��Ϊstring
			const std::string& value = key_value_pair.second.get_value<std::string>();
			
			section_config[key] = value;
		}
		// ������map����SecionInfo��
		SectionInfo sectionInfo;
		sectionInfo._sections_datas = section_config;
		// ��section��key-value�Ա��浽config_map��  
		_config_map[section_name] = sectionInfo;
	}

	// ������е�section��key-value��  
	for (const auto& section_entry : _config_map) {
		const std::string& section_name = section_entry.first;
		SectionInfo section_config = section_entry.second;

		std::cout << "[" << section_name << "]" << std::endl;

		for (const auto& key_value_pair : section_config._sections_datas) {
			std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
		}
	}
}
