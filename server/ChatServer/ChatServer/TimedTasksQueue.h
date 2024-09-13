#pragma once
#include <chrono>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>


// �˶������������ն�ʱ����ģ���ʱ���ֹ����̸߳���ʱ���ֺ�ִ�ж�ʱ�������
class TimedTasksQueue
{
	using Task = std::function<void()>;
public:
	TimedTasksQueue();
	~TimedTasksQueue();
	void Stop();
	void Commit(Task task);
private:
	std::thread work_thread; // �����߳�
	std::queue<Task> timed_tasks; // ��ʱ�������
	std::mutex mutex_; // ���ڻ������
	std::condition_variable cond_; // ����֪ͨ�����߳�
	bool stop_; // ����Ƿ�ֹͣ
};

