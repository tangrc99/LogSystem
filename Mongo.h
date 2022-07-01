//
// Created by 唐仁初 on 2022/6/12.
//

#ifndef MONGO_MONGO_H
#define MONGO_MONGO_H

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

using namespace mongocxx;

class Mongo {
public:

    explicit Mongo(const bsoncxx::string::view_or_value &uri = "mongodb://localhost:27017")
            : conn(mongocxx::uri(uri)) {
        //auto session = conn.start_session();
    }

    ~Mongo() = default;

    // 应该按照 DataBase 来隔离业务，即一个 DataBase 对应一种具体服务
    auto getDataBase(const bsoncxx::string::view_or_value &db){
        return conn.database(db);
    }

private:
    mongocxx::client conn;
    mongocxx::instance instance{};      // mongodb-driver 需要进行初始化操作
};


#endif //MONGO_MONGO_H
