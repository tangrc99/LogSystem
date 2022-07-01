//
// Created by 唐仁初 on 2022/6/12.
//

#ifndef MONGO_MONGOLOG_H
#define MONGO_MONGOLOG_H

#include "Mongo.h"
#include <vector>

static const std::string MongoLogName = "LOG";
using namespace bsoncxx::builder::basic;


class MongoLog : public Mongo {
public:

    explicit MongoLog(const bsoncxx::string::view_or_value &collection) : db_(getDataBase(MongoLogName)) {
        collection_ = db_.collection(collection);
        auto wt_con = collection_.write_concern();

        wt_con.acknowledge_level(mongocxx::write_concern::level::k_unacknowledged); // 不返回事务信息
        wt_con.journal(false);  // 允许数据库预提交
        collection_.write_concern(wt_con);
    }

    virtual ~MongoLog() = default;

    auto append() {
        bsoncxx::document::view_or_value doc = make_document(
                kvp("time", 35345),
                kvp("volt", 432),
                kvp("cur", 345)
        );

        return collection_.insert_one(doc);
    }

    void clearData(){
        collection_.drop();
    }

    auto appendMany(const std::vector<bsoncxx::document::value> &docs) {
        mongocxx::options::insert op;
        op.ordered(true);

        return collection_.insert_many(docs, op);
    }

    auto appendMany(int n) {
        std::vector<bsoncxx::document::value> docs;

        docs.reserve(n);
        while (n--){
            auto start = std::chrono::system_clock::now();
            docs.emplace_back( make_document(
                    kvp("time", 35345),
                    kvp("volt", 432),
                    kvp("cur", 345)
            ));
        }

        mongocxx::options::insert op;
        op.ordered(true);

        return collection_.insert_many(docs, op);
    }

//    void delayAppend() {
//        if (batch_size == 0) {
//            batch = std::make_unique<LogBatch>(&collection_);
//        }
//
//        batch_size++;
//        last_write = time(nullptr);
//
//        batch->record(mongocxx::model::insert_one(
//                make_document(
//                        kvp("time", ""),
//                        kvp("volt", ""),
//                        kvp("cur", "")
//                )));
//
//        if (batch_size >= 100) {
//            auto res = batch->execute();
//
//            // FIXME : 这里要加一个判断
//            batch.reset();
//        }
//
//    }

    bool changeCollection(const bsoncxx::string::view_or_value &name) {
        if (db_.has_collection(name)) {
            collection_ = db_.collection(name);
            return true;
        }
        return false;
    }

private:

    mongocxx::collection collection_;
    mongocxx::database db_;
};


#endif //MONGO_MONGOLOG_H
