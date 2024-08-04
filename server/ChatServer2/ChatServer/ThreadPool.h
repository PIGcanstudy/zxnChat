#pragma once
#include "Singleton.h"
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>
#include <future>
class ThreadPool:public Singleton<ThreadPool>
{
	using Task = std::packaged_task<void()>;
	friend class Singleton<ThreadPool>;
public:
	~ThreadPool();
	int GetThreadNums();
	template<typename F, typename ...Args>
	auto commit(F&& f, Args&& ...args) -> std::future<decltype(f(args...))>;
	void Stop();
private:
	void Start();
	ThreadPool(std::size_t size = std::thread::hardware_concurrency());
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	std::atomic_bool _b_stop;
	std::atomic_int _thread_nums;
	std::condition_variable _cond;
	std::vector<std::thread> _threads;
	std::mutex _mutex;
	std::queue<Task> _tasks;
};

template<typename F, typename ...Args>
inline auto ThreadPool::commit(F&& f, Args && ...args) -> std::future<decltype(f(args ...))>
{
	using RetType = decltype(f(args...));
	if (_b_stop.load()) {
		return std::future<RetType>{};
	}
	auto task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	std::future<RetType> ret = task->get_future();
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_tasks.emplace([task]() {(*task)(); });
	}
	_cond.notify_one();
	return ret;
}
