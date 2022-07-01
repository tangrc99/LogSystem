#include <iostream>
#include <chrono>
#include <list>
#include <fstream>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include "Mongo.h"
#include "MongoLog.h"
#include "LogQueue.h"
#include "LockFreeQueue.h"
#include "LogSender.h"
#include "FileLock.h"
#include "arena.h"
#include "Slice.h"

#define BEST_BATCH 200

int main() {


    Arena arena;
    Slice slice(2,&arena);
    slice[0] = '1';
    slice[1] = '2';
    Slice slice1(2,&arena);
    slice1[0] = '1';
    slice1[1] = '2';
    std::cout<< &slice[0];


    return 0;


    auto logger = std::make_shared<MongoLog>("Heater");
    auto q = std::make_shared<LogBatchQueue>();
    LogSender sender("./", q, logger);


    std::chrono::time_point<std::chrono::system_clock> start, end;
    {
        LogQueue queue("./log", q);

        int nn = 100000;

        start = std::chrono::system_clock::now();

        while (nn--) {
            queue.append();
        }

    }

    end = std::chrono::system_clock::now();

    std::cout << std::chrono::duration<double, std::micro>(end - start).count()
              << "us" << std::endl;
    start = std::chrono::system_clock::now();

    while (sender.sendLogToDB()) {}

    end = std::chrono::system_clock::now();
    std::cout << std::chrono::duration<double, std::micro>(end - start).count()
              << "us" << std::endl;


    return 0;

    bsoncxx::builder::basic::document d;

    bsoncxx::document::view_or_value doc = make_document(
            kvp("time", "111"),
            kvp("volt", "0.2345"),
            kvp("cur", "0.5344")
    );


    int n = 5;
    int i = 20 / n;


    std::ofstream fos("./log", std::ios::app);

    //auto start = std::chrono::system_clock::now();


    //fos << "time " << "volt " << "cur "<< "time " << "volt " << "cur "<< "time " << "volt " << "cur "<< "time " << "volt " << "cur "<< "time " << "volt " << "cur "<< "time " << "volt " << "cur "<<  std::endl;
//    while (i--)
//        list.emplace_back(make_document(
//                kvp("time", ""),
//                kvp("volt", ""),
//                kvp("cur", "")
//        ));

    while (i--)
        logger->appendMany(n);


    return 0;
}
