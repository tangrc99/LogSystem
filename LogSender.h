//
// Created by 唐仁初 on 2022/6/27.
//

#ifndef MONGO_LOGSENDER_H
#define MONGO_LOGSENDER_H

#include <string_view>
#include <utility>
#include <list>
#include "LogQueue.h"
#include "MongoLog.h"

using MongoPtr = std::shared_ptr<MongoLog>;

class LogSender {
public:

    explicit LogSender(const std::string &path, LogBatchQueuePtr queue, MongoPtr db)
            : queue_(std::move(queue)), db_(std::move(db)) {
        checkLogFile();
    }

    void checkLogFile() {

        // 读取上一次持久化的数据
        std::ifstream CUR_LINE(work_dir + "CUR_LINE");
        int cur_line;
        CUR_LINE >> cur_line;

        // 读取旧的 log 数据
        std::fstream log(work_dir + "log");
        std::vector<std::string> lines;
        lines.reserve(cur_line + 200);
        std::string line;
        while (std::getline(log, line)) {
            lines.emplace_back(std::move(line));
        }

        // 判断 log 文件和 CUR_LINE 是否相等，以截断文件
        if (cur_line == lines.size()) {
            std::ofstream logg(work_dir + "log", std::ios::trunc);

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
            auto res = db_->appendMany(docs);
            //FIXME : 这里需要判断是否发送完毕
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
        if (log_size >= 0) {
            // 发送到 MongoDB
            std::vector<bsoncxx::document::value> docs;
            docs.reserve(log_size);

            for (auto &batch: batches) {
                for (auto &log: *batch) {
                    docs.emplace_back(make_document(
                            kvp("1", log.content_1),
                            kvp("2", log.content_2),
                            kvp("3", log.content_3),
                            kvp("4", log.content_4)
                    ));     // 这里是否使用 std::move() 性能是接近的

//                        docs.emplace_back(make_document(
//                                kvp("1",log.content_.substr(0,3)),
//                                kvp("2",log.content_.substr(4,3)),
//                                kvp("3",log.content_.substr(8,3)),
//                                kvp("4",log.content_.substr(12,12))
//                        ));     // 这里使用sub_str 性能也不差

                }
            }


            // 对当前发送进度进行持久化操作
            count += log_size;
            std::ofstream cur_line("./CUR_LINE", std::ios::out);
            cur_line << init_line + count;
            cur_line.flush();

            batches.clear();
            log_size = 0;
        }
        return true;
    }

private:
    size_t count = 0;  // 已经发送出的数量
    size_t init_line{};

    std::list<LogBatchPtr> batches;    // 为了防止进行拷贝，所以存储指针而不是直接存链表
    size_t log_size = 0;

    LogBatchQueuePtr queue_;
    MongoPtr db_;

    const std::string work_dir = "./";
};


#endif //MONGO_LOGSENDER_H
