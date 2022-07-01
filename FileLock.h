//
// Created by 唐仁初 on 2022/6/30.
//

#ifndef MONGO_FILELOCK_H
#define MONGO_FILELOCK_H

#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>

class FileLock {
private:
    int fd;

public:

    explicit FileLock(const std::string &file_name) : fd(open(file_name.c_str(), O_CREAT)) {

        int res = flock(fd, LOCK_EX);

        if (res == -1) {
            std::cerr << "Error To Lock File\n";
            exit(-1);
        }
    }

    ~FileLock() {
        flock(fd, LOCK_UN);
        close(fd);
    }

};


#endif //MONGO_FILELOCK_H
