//#include "AsioIOServicePool.h"
//#include <iostream>
//
//AsioIOServicePool::AsioIOServicePool(std::size_t size) :_ioServices(size),
//_works(size), _nextIOService(0) {
//	for (std::size_t i = 0; i < size; ++i) {
//		_works[i] = std::unique_ptr<Work>(new Work(_ioServices[i]));
//	}
//
//	//遍历多个ioservice,创建多个线程，每个线程内部启动iocontext
//	for (std::size_t i = 0; i < _ioServices.size(); ++i) {
//		_threads.emplace_back([this, i]() {
//			_ioServices[i].run();
//			});
//	}
//}
//
//AsioIOServicePool::~AsioIOServicePool() {
//	Stop();
//	std::cout << "AsioIOServicePool destruct" << std::endl;
//}
//
//boost::asio::io_context& AsioIOServicePool::GetIOService() {
//	auto& service = _ioServices[_nextIOService++];
//	if (_nextIOService == _ioServices.size()) {
//		_nextIOService = 0;
//	}
//	return service;
//}
//
//void AsioIOServicePool::Stop() {
//	//因为仅仅执行work.reset并不能让iocontext从run的状态中退出
//	//当iocontext已经绑定了读或写的监听事件后，还需要手动stop该服务。
//	for (auto& work : _works) {
//		//把服务先停止
//		work->get_io_context().stop();
//		work.reset();
//	}
//
//	for (auto& t : _threads) {
//		t.join();
//	}
//}

#include "AsioIOServicePool.h"
#include <iostream>

AsioIOServicePool::~AsioIOServicePool()
{
	Stop();
	std::cout << "AsioIOService destructed!" << std::endl;
}

boost::asio::io_context& AsioIOServicePool::GetIOService()
{
	if (_next_IOService == _ioServices.size()) {
		_next_IOService = 0;
	}
	return _ioServices[_next_IOService++];
}

void AsioIOServicePool::Stop()
{
	// 关闭work绑定的io_context
	for (std::size_t i = 0; i < _ioServices.size(); i++) {
		_Works[i]->get_io_context().stop();
		_Works[i].reset();
	}

	// 等待线程结束
	for (auto& td : _threads) {
		if (td.joinable()) {
			td.join();
		}
	}
}

AsioIOServicePool::AsioIOServicePool(std::size_t size):_ioServices(size), _Works(size), _next_IOService(0)
{
	// 创建work指针
	for (std::size_t i = 0; i < size; i++) {
		// 这种方式不行，因为_ioServices是私有的，访问不到
		// _Works[i] = std::make_unique<Work>(_ioServices[i]);
		_Works[i] = std::unique_ptr<Work>(new Work(_ioServices[i]));
	}

	// 创建线程
	for (std::size_t i = 0; i < size; i++) {
		_threads.emplace_back([this, i]() {
			_ioServices[i].run();
			});
	}
}
