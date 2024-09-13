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
constexpr int ONESHOT_FLAG = 0x1; // 一次性任务
constexpr int PERIOD_FLAG = 0x2; // 周期性任务
constexpr auto SLEEP_DRIVER = 0; //sleep时间驱动器;
constexpr auto EPOLL_DRIVER = 1; //epoll时间驱动器
constexpr int TW_BITS = 8;
constexpr int TW_SIZE = 1 << TW_BITS;
constexpr int TW_MASK = TW_SIZE - 1; // 每层时间轮的数量
constexpr int TW_LEVEL = 3; //三级时间轮
constexpr int TW_TIME_MS = 100; // 时间轮的精度为100ms
using Callback = std::function<void()>;

inline uint8_t IDX0(uint32_t data) { return data & TW_MASK; } // 第一层时间轮指针
inline uint8_t IDX1(uint32_t data) { return (data >> TW_BITS) & TW_MASK; } //第二层时间轮指针
inline uint8_t IDX2(uint32_t data) { return (data >> (2 * TW_BITS)) & TW_MASK; } // 第三层时间轮指针


struct TwNode {
	std::shared_ptr<TwNode> next; // 指向下一个时间轮结点
	std::shared_ptr<TwNode> last; // 指向上一个时间轮结点
	int32_t expire_time; // 到期时间(ms)
	int32_t period_time; // 周期时间(ms)
	int flags; // 任务类型
	Callback cb; // 需要执行任务的回调函数

	TwNode(int32_t expire, int32_t period, int f, Callback callback)
		: next(nullptr), last(nullptr), expire_time(expire), period_time(period), flags(f), cb(callback) {}
};

struct HeadNode {
	std::shared_ptr<TwNode> next; // 指向下一个时间轮结点
	std::shared_ptr<TwNode> last; // 指向上一个时间轮结点
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

	// 析构函数
	~TimeWheel() {
		Stop();
	}

	// 启动时间轮的函数
	void Start();

	// 停止时间轮的函数
	void Stop();

	// 更新时间轮函数
	void Tw_Update();

	// 增加节点函数
	void Tw_Add_Node(std::shared_ptr<TwNode> node);

	// 通过参数来增加
	void Tw_Add_Node(int32_t expire, int32_t period, int f, Callback callback);
private:

	// 构造函数
	TimeWheel(uint32_t tick_ms = TW_TIME_MS) : tick_ms_(tick_ms), cur_tick_(0), tasks_()
		, slots_(TW_LEVEL, std::vector<std::shared_ptr<HeadNode>>(TW_SIZE)), b_stop_(true), work_thread_() {
		for (int i = 0; i < TW_LEVEL; i++) {
			for (int j = 0; j < TW_SIZE; j++) {
				slots_[i][j] = std::make_shared<HeadNode>();
			}
		}
	}
	// 用于析构时间轮的时候，将存在任务的任务都执行完
	void ProcessRemainingTasks();

	void ProcessLayer(uint8_t layer, uint8_t idx);

	uint32_t tick_ms_; // 时间精度，时间轮时间跨度
	uint32_t cur_tick_; // 当前时间，通过当前时间计算当前时间指针，确定定时任务所在槽位。
	std::vector<std::vector<std::shared_ptr<HeadNode>>> slots_; // 存放的槽
	std::mutex mutex_; // 用来互斥访问一些东西
	std::atomic_bool b_stop_; // 标志时间轮的启动与否
	std::thread work_thread_; // 工作线程
	std::thread ProcessRemaining_thread; // 处理剩余函数线程
	std::atomic_int TaskNums_; /// 任务的数量
	TimedTasksQueue tasks_;
};