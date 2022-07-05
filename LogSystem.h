//
// Created by 唐仁初 on 2022/7/1.
//

#ifndef MONGO_LOGSYSTEM_H
#define MONGO_LOGSYSTEM_H

#include "LogSender.h"
#include "LogQueue.h"
#include "FileLock.h"

class LogSystem {
public:

    explicit LogSystem(const std::string &work_dir)
            : queue_(std::make_shared<LogBatchQueue>()),
              lock1(work_dir + "log"), lock2(work_dir + "CUR_LINE") {

        mongo = std::make_shared<MongoLog>("Heater");
        sender_ = std::make_shared<LogSender>(work_dir, queue_, mongo);
        receiver_ = std::make_shared<LogQueue>(work_dir + "log", queue_);
    }

    void run() {
        receiver_->start();
        sender_->start();
    }

    void append(Slice &slice) {
        receiver_->append(slice);
    }

private:

    void readProperties() {

    }


    std::shared_ptr<LogBatchQueue> queue_;
    std::shared_ptr<LogSender> sender_;
    std::shared_ptr<LogQueue> receiver_;
    std::shared_ptr<MongoLog> mongo;

    FileLock lock1, lock2;
};


#endif //MONGO_LOGSYSTEM_H
