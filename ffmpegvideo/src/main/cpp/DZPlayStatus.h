//
// Created by 曾辉 on 2019-06-06.
//

#ifndef NDK_DAY03_DZSTATUS_H
#define NDK_DAY03_DZSTATUS_H


class DZPlayStatus {
public:
    bool isExit = false;
    bool isLoading = false;
    bool isPause = false;
    bool isSeek = false;

public:
    DZPlayStatus();
};


#endif //NDK_DAY03_DZSTATUS_H
