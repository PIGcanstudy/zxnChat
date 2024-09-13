#include "TimeWheel.h"

void link_add(std::shared_ptr<HeadNode>& head_node, std::shared_ptr<TwNode> tw_node) {
    std::shared_ptr<TwNode> it = head_node->next;
    if (!it) {
        head_node->next = tw_node;
        head_node->last = tw_node;
        return;
    }

    it->last = tw_node;
    head_node->next = tw_node;
    tw_node->next = it;
}

void Get_Time(char* buffer) {
    SYSTEMTIME system_time;
    GetLocalTime(&system_time);  // ��ȡ����ʱ��

    // ��ʽ��ʱ��Ϊ "YYYY-MM-DD HH:MM:SS.mmm"
    snprintf(buffer, 128, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
        system_time.wYear,
        system_time.wMonth,
        system_time.wDay,
        system_time.wHour,
        system_time.wMinute,
        system_time.wSecond,
        system_time.wMilliseconds);
}

std::shared_ptr<TwNode> link_del(std::shared_ptr<HeadNode>& head) {
    if (!head->last) return nullptr;
    auto node = head->last;
    head->last = node->last;
    node->next = node->last = nullptr;
    return node;
}

void TimeWheel::Start()
{
    if (!b_stop_) return; // ����Ѿ�������ֱ�ӷ���
    b_stop_ = false;
    work_thread_ = std::thread([this]() {
        while (!b_stop_) {
            Tw_Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms_));
        }
        });
}

void TimeWheel::Stop()
{
    if (b_stop_) return; // ����Ѿ�ֹͣ��ֱ�ӷ���
    b_stop_ = true;
    if (work_thread_.joinable()) {
        work_thread_.join();
    }

    ProcessRemainingTasks();

    if (ProcessRemaining_thread.joinable()) {
        ProcessRemaining_thread.join();
    }
}

void TimeWheel::Tw_Update()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ++cur_tick_;
    }

    uint8_t idx0 = IDX0(cur_tick_);
    uint8_t idx1 = IDX1(cur_tick_);
    uint8_t idx2 = IDX2(cur_tick_);

    // ������µ�ǰʱ��ʱ��idx0������0 ˵����ʱӦ�ô���ms�����ʱ���� ����100ms ��Ϊ 200ms
    if (idx0 > 0) {
        ProcessLayer(0, idx0);
    }
    else if (idx1 > 0) {
        ProcessLayer(1, idx1);
    }
    else if (idx2 > 0) {
        ProcessLayer(2, idx2);
    }
}

void TimeWheel::Tw_Add_Node(std::shared_ptr<TwNode> node)
{
    std::unique_lock<std::mutex> lock(mutex_);

    uint32_t expire_time = node->expire_time;
    node->expire_time += cur_tick_ * TW_TIME_MS;

    // ��ǰʱ���Ӧ������ʱ��ָ��
    uint8_t idx0 = IDX0(cur_tick_);
    uint8_t idx1 = IDX1(cur_tick_);
    uint8_t idx2 = IDX2(cur_tick_);

    // �½ڵ㵽��ʱ���Ӧ������ʱ��ָ��
    uint8_t e0 = IDX0(node->expire_time / TW_TIME_MS);
    uint8_t e1 = IDX1(node->expire_time / TW_TIME_MS);
    uint8_t e2 = IDX2(node->expire_time / TW_TIME_MS);

    // ���ȼ�ʱ�䳤��ʱ���֣������Ӧʱ��ָ�벻һ�����ͼ��뵽���ʱ�����У��ȴ�����
    if (e2 != idx2) {
        link_add(slots_[2][e2], node);
    }
    else if (e1 != idx1) {
        link_add(slots_[1][e1], node);
    }
    else {
        link_add(slots_[0][e0], node);
    }

    ++TaskNums_;
}

void TimeWheel::Tw_Add_Node(int32_t expire, int32_t period, int f, Callback callback)
{
    std::shared_ptr<TwNode> twnode = std::make_shared<TwNode>(expire, period, f, callback);
    Tw_Add_Node(twnode);
}

void TimeWheel::ProcessRemainingTasks()
{
    ProcessRemaining_thread = std::thread([this]() {
        while (TaskNums_) {
            Tw_Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms_));
        }
        });
}

void TimeWheel::ProcessLayer(uint8_t layer, uint8_t idx) {

    std::unique_lock<std::mutex> lock(mutex_);
    auto& it = slots_[layer][idx];
    std::shared_ptr<TwNode> node;

    // �ռ���Ҫִ�еĻص������������ڳ�����������µ�������
    while ((node = link_del(it)) != nullptr) {
        uint8_t e0 = IDX0(node->expire_time / TW_TIME_MS);
        uint8_t e1 = IDX1(node->expire_time / TW_TIME_MS);
        uint8_t e2 = IDX2(node->expire_time / TW_TIME_MS);

        if (layer == 2 && e1 > 0) {
            link_add(slots_[1][e1], node); // �ƶ����ڶ���
        }
        else if ((layer == 1 || layer == 2) && e0 > 0) {
            link_add(slots_[0][e0], node); // �ƶ�����һ��
        }
        else {
            if (node->flags & PERIOD_FLAG) {
                node->expire_time = node->period_time;
                // �ͷ����Ա���ݹ��������
                lock.unlock();
                if(!b_stop_) Tw_Add_Node(node); // ��������ʱ�ݹ����,���û��ֹͣ�ͼ����ӷ���Ͳ���
                lock.lock(); // ���»�ȡ��;
            }
            // ���ص������洢����ʱ�б�
            tasks_.Commit(node->cb);
        }
    }
}
