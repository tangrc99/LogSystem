//
// Created by 唐仁初 on 2022/6/27.
//

#ifndef MONGO_LOGSENDER_H
#define MONGO_LOGSENDER_H

#include <string_view>
#include <utility>
#include <list>
#include "LogQueue.h"
#include "LogInformation.h"
#include "MongoLog.h"

using MongoPtr = std::shared_ptr<MongoLog>;

class LogSender {
public:

    explicit LogSender(const std::string &path, LogBatchQueuePtr queue, MongoPtr db)
            : queue_(std::move(queue)), db_(std::move(db)) {
        checkLogFile();
    }

    void checkLogFile() {

        // 读取上一次持久化的行数
        std::ifstream CUR_LINE(work_dir + "CUR_LINE");
        int cur_line;
        CUR_LINE >> cur_line;

        // 读取旧的 log 数据
        std::fstream log(work_dir + "log");
        std::vector<std::string> lines; // 每一行代表一条 log
        lines.reserve(cur_line + 200);
        std::string line;
        while (std::getline(log, line)) {
            lines.emplace_back(std::move(line));
        }

        // 判断 log 文件和 CUR_LINE 是否相等，以截断文件
        if (cur_line >= lines.size()) {
            std::ofstream logg(work_dir + "log", std::ios::trunc);
            write_cur_line();

        } else {

            // 发送没有发送的数据
            std::vector<bsoncxx::document::value> docs;
            docs.reserve(lines.size() - cur_line + 1);

            for (int i = cur_line; i < lines.size(); i++) {

                docs.emplace_back(make_document(
                        kvp("1", lines[i].substr(0, 3)),
                        kvp("2", lines[i].substr(4, 3)),
                        kvp("3", lines[i].substr(8, 3)),
                        kvp("4", lines[i].substr(12, 12))
                ));
            }
            stdx::optional<result::insert_many> res = db_->appendMany(docs);
            //FIXME : 这里需要判断是否发送完毕


            if (res->inserted_count() != lines.size() - cur_line) {
                count = cur_line + res->inserted_count();
                write_cur_line();
                exit(-1);
            }

            std::ofstream logg(work_dir + "log", std::ios::trunc);
            write_cur_line();
        }
    }

    bool sendLogToDB() {
        // 聚合 log 数量到一个合适的范围
        auto log_batch = queue_->pop();

        if (log_batch == nullptr) {
            return false;
        }
        batches.emplace_back(log_batch);
        log_size += log_batch->size();


        if (log_size >= 300) {
            // 发送到 MongoDB
            std::vector<bsoncxx::document::value> docs;
            docs.reserve(log_size);

            for (auto &batch: batches) {
                for (auto &log: *batch) {
//                    docs.emplace_back(make_document(
//                            kvp("1", log.content_1),
//                            kvp("2", log.content_2),
//                            kvp("3", log.content_3),
//                            kvp("4", log.content_4)
//                    ));     // 这里是否使用 std::move() 性能是接近的


                    auto log_str = log.content_.toString();
                    docs.emplace_back(make_document(
                            kvp("1", std::move(log_str.substr(0, 3))),
                            kvp("2", std::move(log_str.substr(4, 3))),
                            kvp("3", std::move(log_str.substr(8, 3))),
                            kvp("4", std::move(log_str.substr(12, 12)))
                    ));     // 这里使用sub_str 性能也不差

                }
            }


            write_cur_line();
        }
        return true;
    }

    // 如果当前时刻没有数据，则返回 false
    bool getLogFromQueue() {
        // 聚合 log 数量到一个合适的范围
        auto log_batch = queue_->pop();

        if (log_batch == nullptr) {
            return false;
        }

        batches.emplace_back(log_batch);
        log_size += log_batch->size();
        return true;
    };

    void checkBatchSizeAndSend(int batch_size = 300) {
        if (log_size > batch_size)
            sendLog();
    }

    void sendLog() {
        if (log_size == 0)
            return;

        // 发送到 MongoDB
        std::vector<bsoncxx::document::value> docs;
        docs.reserve(log_size);

        for (auto &batch: batches) {
            for (auto &log: *batch) {
//                    docs.emplace_back(make_document(
//                            kvp("1", log.content_1),
//                            kvp("2", log.content_2),
//                            kvp("3", log.content_3),
//                            kvp("4", log.content_4)
//                    ));     // 这里是否使用 std::move() 性能是接近的


                auto log_str = log.content_.toString();
                docs.emplace_back(make_document(
                        kvp("1", std::move(log_str.substr(0, 3))),
                        kvp("2", std::move(log_str.substr(4, 3))),
                        kvp("3", std::move(log_str.substr(8, 3))),
                        kvp("4", std::move(log_str.substr(12, 12)))
                ));     // 这里使用sub_str 性能也不差

            }
        }

        db_->appendMany(docs);
        write_cur_line();

        batches.clear();
        log_size = 0;
    }

    // 对当前发送进度进行持久化操作
    void write_cur_line() {
        count += log_size;
        std::ofstream cur_line("./CUR_LINE", std::ios::out);
        cur_line << count;
        cur_line.flush();
    }

    void start() {
        send_thread_ = std::thread(&LogSender::LoopSendLog, this);
    }

    void LoopSendLog() {
        while (!quit) {
            if (!getLogFromQueue()) {
                // 如果当前没有数据，需要做的
                if (log_size > 0)
                    sendLog();
            } else {
                checkBatchSizeAndSend();
            }
        }
    }

    LogSender(const LogSender &other) = delete;

    LogSender &operator=(const LogSender &other) = delete;

    ~LogSender() {
        quit = true;

        if (send_thread_.joinable())
            send_thread_.join();

        while (getLogFromQueue()) {}

        sendLog();
    }

private:
    size_t count = 0;  // 已经发送出的数量

    std::list<LogBatchPtr> batches;    // 为了防止进行拷贝，所以存储指针而不是直接存链表
    size_t log_size = 0;

    LogBatchQueuePtr queue_;
    MongoPtr db_;

    const std::string work_dir = "./";

    bool quit = false;
    std::thread send_thread_;
};


#endif //MONGO_LOGSENDER_H
