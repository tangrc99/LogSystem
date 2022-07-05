//
// Created by 唐仁初 on 2022/7/1.
//

#ifndef MONGO_LOGINFORMATION_H
#define MONGO_LOGINFORMATION_H

#include "Slice.h"

//struct LogInformation {
//    std::string content_ = "123 123 123 123123123123";
//    std::string content_1;
//    std::string content_2;
//    std::string content_3;
//    std::string content_4;
//
//    explicit LogInformation(std::string content) : content_(std::move(content)) {}
//
////    explicit LogInformation(const std::string &content_1, const std::string &content_2, const std::string &content_3,
////                            const std::string &content_4) : content_(content_1 + " "+ content_2 + " "+content_3 +" "+ content_4) {}
//    explicit LogInformation(std::string content_1, std::string content_2, std::string content_3,
//                            std::string content_4) : content_1(std::move(content_1)), content_2(std::move(content_2)),
//                                                     content_3(std::move(content_3)),
//                                                     content_4(std::move(content_4)) {}
//
//    LogInformation() : content_("123 123 123 123123123123") {}
//};



struct LogInformation {
    Slice content_;


    explicit LogInformation(Slice content) : content_(std::move(content)) {}

//    explicit LogInformation(const std::string &content_1, const std::string &content_2, const std::string &content_3,
//                            const std::string &content_4) : content_(content_1 + " "+ content_2 + " "+content_3 +" "+ content_4) {}


    //LogInformation() : content_("123 123 123 123123123123") {}
};


#endif //MONGO_LOGINFORMATION_H
