cmake_minimum_required(VERSION 3.20)
project(Mongo)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(Mongo
        main.cpp
        Mongo.cpp Mongo.h
        MongoLog.cpp MongoLog.h
        LogQueue.cpp LogQueue.h
        LogSender.cpp LogSender.h
        LockFreeQueue.cpp LockFreeQueue.h
        FileLock.h
        arena.h arena.cc Slice.h)

target_link_libraries(Mongo mongocxx bsoncxx)

