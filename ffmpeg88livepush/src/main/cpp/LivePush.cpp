//
// Created by 123 on 2020/10/15.
//

#include "LivePush.h"

LivePush::LivePush(JNICall *pJniCall, const char *liveUrl) {
    this->pJniCall = pJniCall;
    this->liveUrl = static_cast<char *>(malloc(strlen(liveUrl) + 1));
    strcpy(this->liveUrl, liveUrl);
    pPacketQueue = new PacketQueue();
}

LivePush::~LivePush() {
    if (this->liveUrl)
        free(this->liveUrl);

    if (pPacketQueue) {
        delete pPacketQueue;
        pPacketQueue = nullptr;
    }
}

void *threadInitConnect(void *args) {
    LivePush *livePush = (LivePush *) args;
    // 1.创建RTMP
    livePush->pRtmp = RTMP_Alloc();
    // 2. 初始化
    RTMP_Init(livePush->pRtmp);
    // 3. 设置参数
    livePush->pRtmp->Link.timeout = 10; // 超时时间 10 秒
    livePush->pRtmp->Link.lFlags = RTMP_LF_LIVE; // 长连接
    RTMP_SetupURL(livePush->pRtmp, livePush->liveUrl);// 设置推流的地址
    RTMP_EnableWrite(livePush->pRtmp);// 可写
    // 4. 开始连接
    if (!RTMP_Connect(livePush->pRtmp, nullptr)) {
        // 回调到java层
        LOGE("rtmp connect error");
        livePush->pJniCall->callConnectError(THREAD_CHILD, INIT_RTMP_CONNECT_ERROR_CODE,
                                             "rtmp connect error");
        return (void *)(INIT_RTMP_CONNECT_ERROR_CODE);
    }

    if (!RTMP_ConnectStream(livePush->pRtmp, 0)) {
        // 回调到java层
        LOGE("rtmp connect stream error");
        livePush->pJniCall->callConnectError(THREAD_CHILD, INIT_RTMP_CONNECT_STREAM_ERROR_CODE,
                                             "rtmp connect stream error");
        return (void *)(INIT_RTMP_CONNECT_STREAM_ERROR_CODE);
    }

    LOGE("rtmp connect success");
    livePush->pJniCall->callConnectSuccess(THREAD_CHILD);

    return 0;
}

void LivePush::initConnect() {
    // 在子线程中进行连接
    pthread_t initConnecTid;
    pthread_create(&initConnecTid, nullptr,threadInitConnect, this);
}
