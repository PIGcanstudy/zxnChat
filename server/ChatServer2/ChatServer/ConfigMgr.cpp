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
	//获取当前工作目录
	boost::filesystem::path current_path = boost::filesystem::current_path();
	//构建config.ini文件的完整路径
	boost::filesystem::path config_path = current_path / "config.ini";
	std::cout << "Config path: " << config_path << std::endl;

	// 使用boost.propertytree来读取ini文件
	/*
	* 用于处理树形结构的数据,通常用于解析、操作
	* 和生成各种配置文件、XML 文件、JSON 文件等
	*/
	boost::property_tree::ptree pt;
	// 读取里面的内容
	boost::property_tree::read_ini(config_path.string(), pt);

	// 遍历INI文件中的所有section
	for (const auto& section_pair : pt) {
		// 此为[]里的GateServer
		const std::string& section_name = section_pair.first;
		// 这是[]下面一组键值对
		const boost::property_tree::ptree& section_tree = section_pair.second;

		//存储遍历得到的键值
		std::map<std::string, std::string> section_config;
		
		// 对于每个section，遍历其所有的key-value对  
		for (const auto& key_value_pair : section_tree) {
			// =前面的 比如port
			const std::string& key = key_value_pair.first;
			// =后买你的 比如8080，由于返回的还是ptree 所以直接转换为string
			const std::string& value = key_value_pair.second.get_value<std::string>();
			
			section_config[key] = value;
		}
		// 将上述map存在SecionInfo中
		SectionInfo sectionInfo;
		sectionInfo._sections_datas = section_config;
		// 将section的key-value对保存到config_map中  
		_config_map[section_name] = sectionInfo;
	}

	// 输出所有的section和key-value对  
	for (const auto& section_entry : _config_map) {
		const std::string& section_name = section_entry.first;
		SectionInfo section_config = section_entry.second;

		std::cout << "[" << section_name << "]" << std::endl;

		for (const auto& key_value_pair : section_config._sections_datas) {
			std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
		}
	}
}
