#pragma once
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <memory>
#include <functional>
#include <vector>
#include <thread>
#include <time.h>
#include <mutex>
#include "TimedTasksQueue.h"
constexpr int ONESHOT_FLAG = 0x1; // һ��������
constexpr int PERIOD_FLAG = 0x2; // ����������
constexpr auto SLEEP_DRIVER = 0; //sleepʱ��������;
constexpr auto EPOLL_DRIVER = 1; //epollʱ��������
constexpr int TW_BITS = 8;
constexpr int TW_SIZE = 1 << TW_BITS;
constexpr int TW_MASK = TW_SIZE - 1; // ÿ��ʱ���ֵ�����
constexpr int TW_LEVEL = 3; //����ʱ����
constexpr int TW_TIME_MS = 100; // ʱ���ֵľ���Ϊ100ms
using Callback = std::function<void()>;

inline uint8_t IDX0(uint32_t data) { return data & TW_MASK; } // ��һ��ʱ����ָ��
inline uint8_t IDX1(uint32_t data) { return (data >> TW_BITS) & TW_MASK; } //�ڶ���ʱ����ָ��
inline uint8_t IDX2(uint32_t data) { return (data >> (2 * TW_BITS)) & TW_MASK; } // ������ʱ����ָ��


struct TwNode {
	std::shared_ptr<TwNode> next; // ָ����һ��ʱ���ֽ��
	std::shared_ptr<TwNode> last; // ָ����һ��ʱ���ֽ��
	int32_t expire_time; // ����ʱ��(ms)
	int32_t period_time; // ����ʱ��(ms)
	int flags; // ��������
	Callback cb; // ��Ҫִ������Ļص�����

	TwNode(int32_t expire, int32_t period, int f, Callback callback)
		: next(nullptr), last(nullptr), expire_time(expire), period_time(period), flags(f), cb(callback) {}
};

struct HeadNode {
	std::shared_ptr<TwNode> next; // ָ����һ��ʱ���ֽ��
	std::shared_ptr<TwNode> last; // ָ����һ��ʱ���ֽ��
	HeadNode() {
		next = nullptr;
		last = nullptr;
	}
};

class TimeWheel
{
public:
	friend class TimedTasksQueue;

	static TimeWheel& GetIntance() {
		static TimeWheel tw;
		return tw;
	}

	// ��������
	~TimeWheel() {
		Stop();
	}

	// ����ʱ���ֵĺ���
	void Start();

	// ֹͣʱ���ֵĺ���
	void Stop();

	// ����ʱ���ֺ���
	void Tw_Update();

	// ���ӽڵ㺯��
	void Tw_Add_Node(std::shared_ptr<TwNode> node);

	// ͨ������������
	void Tw_Add_Node(int32_t expire, int32_t period, int f, Callback callback);
private:

	// ���캯��
	TimeWheel(uint32_t tick_ms = TW_TIME_MS) : tick_ms_(tick_ms), cur_tick_(0), tasks_()
		, slots_(TW_LEVEL, std::vector<std::shared_ptr<HeadNode>>(TW_SIZE)), b_stop_(true), work_thread_() {
		for (int i = 0; i < TW_LEVEL; i++) {
			for (int j = 0; j < TW_SIZE; j++) {
				slots_[i][j] = std::make_shared<HeadNode>();
			}
		}
	}
	// ��������ʱ���ֵ�ʱ�򣬽��������������ִ����
	void ProcessRemainingTasks();

	void ProcessLayer(uint8_t layer, uint8_t idx);

	uint32_t tick_ms_; // ʱ�侫�ȣ�ʱ����ʱ����
	uint32_t cur_tick_; // ��ǰʱ�䣬ͨ����ǰʱ����㵱ǰʱ��ָ�룬ȷ����ʱ�������ڲ�λ��
	std::vector<std::vector<std::shared_ptr<HeadNode>>> slots_; // ��ŵĲ�
	std::mutex mutex_; // �����������һЩ����
	std::atomic_bool b_stop_; // ��־ʱ���ֵ��������
	std::thread work_thread_; // �����߳�
	std::thread ProcessRemaining_thread; // ����ʣ�ຯ���߳�
	std::atomic_int TaskNums_; /// ���������
	TimedTasksQueue tasks_;
};