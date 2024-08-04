#include "ThreadPool.h"

ThreadPool::~ThreadPool()
{
	Stop();
}

int ThreadPool::GetThreadNums()
{
	return _thread_nums;
}

void ThreadPool::Start()
{
	for (int i = 0; i < _thread_nums; i++) {
		_threads.emplace_back([this]() {
			while (true) {
				Task task;
				{
					std::unique_lock<std::mutex> lock(_mutex);
					_cond.wait(lock, [this]()->bool {
						return _b_stop.load() || !_tasks.empty();
						});
					if (_b_stop.load() == true || _tasks.empty()) {
						return;
					}
					task = std::move(_tasks.front());
					_tasks.pop();
				}
				_thread_nums--;
				task();
				_thread_nums++;
			}
			});
	}
}

void ThreadPool::Stop()
{
	_b_stop.store(true);
	_cond.notify_all();
	for (auto& td : _threads) {
		td.join();
	}
}

ThreadPool::ThreadPool(std::size_t size) :_thread_nums(size), _b_stop(false)
{
	Start();
}
