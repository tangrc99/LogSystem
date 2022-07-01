//
// Created by 唐仁初 on 2022/6/30.
//

#ifndef MONGO_SLICE_H
#define MONGO_SLICE_H

#include "arena.h"
#include <string_view>

class Slice {
public:
    // 从 Arena 中获得内存分配
    explicit Slice(size_t bytes, Arena *arena) : data_(arena->Allocate(bytes)), size_(bytes) {}

    char &operator[](size_t pos) {
        return data_[pos];
    }

    std::string_view view() {
        return {data_, size_};
    }

    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    const char *data() {
        return data_;
    }

    [[nodiscard]] size_t size() const {
        return size_;
    }

    std::string toString() {
        return {data_, size_};
    }

private:

    char *data_;
    size_t size_;
};


#endif //MONGO_SLICE_H
