﻿#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "CServer.h"
#include <iostream>
#include "ConfigMgr.h"
#include "AsioIOServicePool.h"
#include <hiredis.h>
#include "RedisMgr.h"

void TestRedisMgr() {
	assert(RedisMgr::GetInstance()->Set("blogwebsite", "llfc.club"));
	std::string value = "";
	assert(RedisMgr::GetInstance()->Get("blogwebsite", value));
	assert(RedisMgr::GetInstance()->Get("nonekey", value) == false);
	assert(RedisMgr::GetInstance()->HSet("bloginfo", "blogwebsite", "llfc.club"));
	assert(RedisMgr::GetInstance()->HGet("bloginfo", "blogwebsite") != "");
	assert(RedisMgr::GetInstance()->ExistsKey("bloginfo"));
	assert(RedisMgr::GetInstance()->Del("bloginfo"));
	assert(RedisMgr::GetInstance()->Del("bloginfo"));
	assert(RedisMgr::GetInstance()->ExistsKey("bloginfo") == false);
	assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue1"));
	assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue2"));
	assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue3"));
	assert(RedisMgr::GetInstance()->RPop("lpushkey1", value));
	assert(RedisMgr::GetInstance()->RPop("lpushkey1", value));
	assert(RedisMgr::GetInstance()->LPop("lpushkey1", value));
	assert(RedisMgr::GetInstance()->LPop("lpushkey2", value) == false);
}
int main()
{
	TestRedisMgr();
	try {
		auto& gcfgMgr = ConfigMgr::GetInstance();
		std::string gate_port_str = gcfgMgr["GateServer"]["Port"];
		unsigned short gate_port = atoi(gate_port_str.c_str());
		net::io_context ioc{ 1 };
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
			if (error) {
				return;
			}
			ioc.stop();
			});
		std::make_shared<CServer>(ioc, gate_port)->Start();
		std::cout << "Gate Server listen on port: " << gate_port << std::endl;
		ioc.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception is " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
