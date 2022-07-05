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
#include "LogSystem.h"

#define BEST_BATCH 200


int main() {

    Arena arena;

    std::chrono::time_point<std::chrono::system_clock> start, end;

//    auto logger = std::make_shared<MongoLog>("Heater");
//    auto q = std::make_shared<LogBatchQueue>();
    {


        //LogSender sender("./", q, logger);


        {
           // LogQueue queue("./log", q);

            int nn = 10000;

//            queue.start();
//            sender.start();

            LogSystem logger("./");

            for(;;);

            logger.run();

            start = std::chrono::system_clock::now();

            while (nn--) {

                Slice slice(24, &arena);
                slice[0] = '1';
                slice[1] = '2';
                slice[2] = '3';
                slice[3] = ' ';
                slice[4] = '1';
                slice[5] = '2';
                slice[6] = '3';
                slice[7] = ' ';
                slice[8] = '1';
                slice[9] = '2';
                slice[10] = '3';
                slice[11] = ' ';

                slice[12] = '1';
                slice[13] = '2';
                slice[14] = '3';
                slice[15] = '1';
                slice[16] = '2';
                slice[17] = '3';
                slice[18] = '1';
                slice[19] = '2';
                slice[20] = '3';
                slice[21] = '1';
                slice[22] = '2';
                slice[23] = '3';


                logger.append(slice);
            }

        }

        end = std::chrono::system_clock::now();

        std::cout << std::chrono::duration<double, std::micro>(end - start).count()
                  << "us" << std::endl;
        //start = std::chrono::system_clock::now();
//        {
//
//        }
//        while (sender.getLogFromQueue()) { sender.checkBatchSizeAndSend(); }
    }
    end = std::chrono::system_clock::now();

    std::cout << std::chrono::duration<double, std::micro>(end - start).count()
              << "us" << std::endl;


    return 0;


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

//    while (i--)
//        logger->appendMany(n);


    return 0;
}
