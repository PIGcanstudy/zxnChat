#include "TimedTasksQueue.h"
#include <iostream>

TimedTasksQueue::TimedTasksQueue(): stop_(false)
{
	work_thread = std::thread([this]() {
		while (!stop_) {
			std::unique_lock<std::mutex> lock(mutex_);
			cond_.wait(lock, [this]() {
				return !timed_tasks.empty() || stop_;
				});
			if (stop_) {
				return;
			}
			auto task = timed_tasks.front();
			std::cout << "Ö´ÐÐÈÎÎñ\n";
			task();
			timed_tasks.pop();
		}
		});
}

TimedTasksQueue::~TimedTasksQueue()
{
	Stop();
}

void TimedTasksQueue::Stop()
{
	stop_ = true;
	cond_.notify_all();
	if (work_thread.joinable()) {
		work_thread.join();
	}
}

void TimedTasksQueue::Commit(Task task)
{
	timed_tasks.emplace(task);
	cond_.notify_one();

}
