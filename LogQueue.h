//
// Created by 唐仁初 on 2022/6/13.
//

#ifndef MONGO_LOGQUEUE_H
#define MONGO_LOGQUEUE_H

#include <thread>
#include <atomic>
#include <string>
#include <utility>
#include <list>
#include <fstream>
#include <chrono>
#include <iostream>
#include "LockFreeQueue.h"
#include "Slice.h"



// 目前是 0.5 us 一条 log ？？

struct LogInformation ;

using LogBatch = std::list<LogInformation>;
using LogBatchPtr = std::shared_ptr<std::list<LogInformation>>;
using LogBatchQueue = LockFreeQueue<LogBatch>;
using LogBatchQueuePtr = std::shared_ptr<LogBatchQueue>;

class LogQueue {
public:

    // 将一条 log 添加到队列中
    void append();

    void append(Slice &slice);

    void start();

    explicit LogQueue(const std::string &file, LogBatchQueuePtr queue = nullptr);

    ~LogQueue();

    LogQueue(const LogQueue &other) = delete;

    LogQueue &operator=(const LogQueue &other) = delete;

private:
    bool flush();

    void LoopFlush();



    std::thread log_thread_;

    std::list<LogInformation> logs_;

    int count = 0;

    std::string path_;
    std::ofstream log_file;

    std::atomic_flag swp_flag = ATOMIC_FLAG_INIT;
    std::atomic_flag flush_flag = ATOMIC_FLAG_INIT;

    bool quit;
    bool finished = false;

    LogBatchQueuePtr queue_; // 用于发送 log 的一个中间件
};


#endif //MONGO_LOGQUEUE_H
