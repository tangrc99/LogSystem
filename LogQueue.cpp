//
// Created by 唐仁初 on 2022/6/13.
//

#include "LogQueue.h"
#include "LogInformation.h"


//void LogQueue::append() {
//    LogInformation log("123", "123", "123", "123123123123");
//    // 进入临界区
//    while (swp_flag.test_and_set(std::memory_order_acquire));
//    logs_.emplace_back();
//    //logs_.emplace_back(std::move(log));
//    swp_flag.clear(std::memory_order_release);
//
//    // 若log线程非活跃，则进行通知
//    flush_flag.test_and_set(std::memory_order_acquire);
//    flush_flag.notify_one();
//
//}

void LogQueue::append(Slice &slice) {
    // 进入临界区
    while (swp_flag.test_and_set(std::memory_order_acquire));
    logs_.emplace_back(std::move(slice));
    //logs_.emplace_back(std::move(log));
    swp_flag.clear(std::memory_order_release);

    // 若log线程非活跃，则进行通知
    flush_flag.test_and_set(std::memory_order_acquire);
    flush_flag.notify_one();

}

bool LogQueue::flush() {
    if (logs_.empty()) {
        flush_flag.clear(std::memory_order_release);
        return false;
    }

    // 初始化资源并且进入临界区
    std::list<LogInformation> logs;

    while (swp_flag.test_and_set(std::memory_order_acquire));
    std::swap(logs_, logs);
    swp_flag.clear(std::memory_order_release);

    // 退出临界区后进行工作
    for (auto &log: logs) {
        log_file << log.content_.view() << '\n';
        //log_file << log.content_1 << " "<< log.content_2 << " "<<log.content_3 << " "<<log.content_4 << " "<<'\n';

    }
    log_file.flush();
    count++;

    if (queue_)
        queue_->push(std::move(logs));

    return true;
}

void LogQueue::LoopFlush() {

    while (!quit) {
        if (!flush()) {
            flush_flag.wait(false);
        }
    }
}

LogQueue::LogQueue(const std::string &file, LogBatchQueuePtr queue)
        : path_(file), quit(false), queue_(std::move(queue)) {

    log_file.open(file, std::ios::app);

    if (!log_file.is_open()) {
        std::cerr << "NOT OPEN FILE\n";
        exit(-1);
    }

}

LogQueue::~LogQueue() {
    // 等待线程将剩余的 log 全部写入到硬盘中
    while (!logs_.empty()) {
        flush_flag.test_and_set(std::memory_order_acquire);
        flush_flag.notify_all();
    }

    // 控制线程活跃并退出。
    quit = true;
    flush_flag.test_and_set(std::memory_order_acquire);
    flush_flag.notify_one();

    log_thread_.join();

    log_file.close();
    std::cout << "count " << count << std::endl;
}

void LogQueue::start() {
    log_thread_ = std::thread(&LogQueue::LoopFlush, this);     // 这里线程必须在这里初始化，否则可能会在quit前被初始化

}
