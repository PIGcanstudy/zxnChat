//#pragma once
//#include <vector>
//#include <boost/asio.hpp>
//#include "Singleton.h"
//
//class AsioIOServicePool :public Singleton<AsioIOServicePool>
//{
//	friend Singleton<AsioIOServicePool>;
//public:
//	using IOService = boost::asio::io_context;
//	using Work = boost::asio::io_context::work;
//	using WorkPtr = std::unique_ptr<Work>;
//	~AsioIOServicePool();
//	AsioIOServicePool(const AsioIOServicePool&) = delete;
//	AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;
//	// 用来得到ioservice
//	boost::asio::io_context& GetIOService();
//	void Stop();
//private:
//	AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency());
//	std::vector<IOService> _ioServices;
//	std::vector<WorkPtr> _works;
//	std::vector<std::thread> _threads;
//	std::size_t                        _nextIOService;
//};

#pragma

#include <thread>
#include <boost/asio.hpp>
#include <vector>
#include "Singleton.h"

class AsioIOServicePool : public Singleton<AsioIOServicePool> {
public:
	friend class Singleton<AsioIOServicePool>;
	using IOService = boost::asio::io_context;
	using Work = boost::asio::io_context::work;
	using WorkPtr = std::unique_ptr<Work>;
	~AsioIOServicePool();
	boost::asio::io_context& GetIOService();
	void Stop();
private:
	AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency());
	AsioIOServicePool(const AsioIOServicePool&) = delete;
	AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;
	std::vector<IOService> _ioServices;
	std::vector<WorkPtr> _Works;
	std::vector<std::thread> _threads;
	std::size_t _next_IOService;
};