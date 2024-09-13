#pragma once
#include <chrono>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>


// 此队列是用来接收定时任务的，将时间轮工作线程更新时间轮和执行定时任务解耦
class TimedTasksQueue
{
	using Task = std::function<void()>;
public:
	TimedTasksQueue();
	~TimedTasksQueue();
	void Stop();
	void Commit(Task task);
private:
	std::thread work_thread; // 工作线程
	std::queue<Task> timed_tasks; // 定时任务队列
	std::mutex mutex_; // 用于互斥访问
	std::condition_variable cond_; // 用于通知阻塞线程
	bool stop_; // 标记是否停止
};

