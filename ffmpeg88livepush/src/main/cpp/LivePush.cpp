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

    if (pRtmp) {
        RTMP_Close(pRtmp);
        free(pRtmp);
        pRtmp = nullptr;
    }
}

void *threadInitConnect(void *args) {
    LivePush *pLivePush = (LivePush *) args;
    // 1.创建RTMP
    pLivePush->pRtmp = RTMP_Alloc();
    // 2. 初始化
    RTMP_Init(pLivePush->pRtmp);
    // 3. 设置参数
    pLivePush->pRtmp->Link.timeout = 10; // 超时时间 10 秒
    pLivePush->pRtmp->Link.lFlags = RTMP_LF_LIVE; // 长连接
    RTMP_SetupURL(pLivePush->pRtmp, pLivePush->liveUrl);// 设置推流的地址
    RTMP_EnableWrite(pLivePush->pRtmp);// 可写
    // 4. 开始连接
    if (!RTMP_Connect(pLivePush->pRtmp, nullptr)) {
        // 回调到java层
        LOGE("rtmp connect error");
        pLivePush->pJniCall->callConnectError(THREAD_CHILD, INIT_RTMP_CONNECT_ERROR_CODE,
                                              "rtmp connect error");
        return (void *) (INIT_RTMP_CONNECT_ERROR_CODE);
    }

    if (!RTMP_ConnectStream(pLivePush->pRtmp, 0)) {
        // 回调到java层
        LOGE("rtmp connect stream error");
        pLivePush->pJniCall->callConnectError(THREAD_CHILD, INIT_RTMP_CONNECT_STREAM_ERROR_CODE,
                                              "rtmp connect stream error");
        return (void *) (INIT_RTMP_CONNECT_STREAM_ERROR_CODE);
    }

    LOGE("rtmp connect success");
    pLivePush->pJniCall->callConnectSuccess(THREAD_CHILD);
    pLivePush->startTime = RTMP_GetTime();
    while (pLivePush->isPushing) {
        // 不断的往流媒体服务器上推
        RTMPPacket *pPacket = pLivePush->pPacketQueue->pop();
        if(pPacket != NULL){
          /*  int res = */RTMP_SendPacket(pLivePush->pRtmp, pPacket, 1);
//        LOGE("res = %d", res); // 1不一定成功，0一定不成功
            RTMPPacket_Free(pPacket);
            free(pPacket);
        }
    }

    LOGE("停止了");

    return 0;
}

void LivePush::initConnect() {
    // 在子线程中进行连接
    pthread_create(&initConnecTid, nullptr, threadInitConnect, this);
//    pthread_detach(initConnecTid);
}

void LivePush::pushSpsPps(jbyte *spsData, jint spsLen, jbyte *ppsData, jint ppsLen) {

    // FLV 的封装格式
    // frame type : 1关键帧，2 非关键帧（4Bit）
    // CodecID : 7表示 AVC(4bit) , 与 frame type组合起来刚好是 1 个字节 0x17
    // fixed ： 0x00 0x00 0x00 0x00 (4byte)
    // configurationVersion  (1byte) 0x01版本
    // AVCProfileIndication  (1byte) sps[1] profile
    // profile_compatibility (1byte) sps[2] compatibility
    // AVCLevelIndication    (1byte) sps[3] Profile level
    // lengthSizeMinusOne    (1byte) 0xff   包长数据所使用的字节数

    // sps + pps 的数据
    // sps number            (1byte) 0xe1   sps 个数
    // sps data length       (2byte) sps 长度
    // sps data                      sps 的内容
    // pps number            (1byte) 0x01 pps 个数
    // pps data length       (2byte) pps 长度
    // pps data                      pps 的内容

    // 数据的长度（大小） = sps 大小 + pps 大小 + 头 16字节
    int bodySize = spsLen + ppsLen + 16;
    // 构建 RTMPPacket
    RTMPPacket *pPacket = (RTMPPacket *) malloc(sizeof(RTMPPacket));
    RTMPPacket_Alloc(pPacket, bodySize);
    RTMPPacket_Reset(pPacket);

    // 构建 body 按上面的一个一个开始赋值
    char *body = pPacket->m_body;
    int index = 0;
    body[index++] = 0x17;
    // fixed ： 0x00 0x00 0x00 0x00 (4byte)
    body[index++] = 0x00;
    body[index++] = 0x00;
    body[index++] = 0x00;
    body[index++] = 0x00;
    // configurationVersion  (1byte) 0x01版本
    body[index++] = 0x01;
    // AVCProfileIndication  (1byte) sps[1] profile
    body[index++] = spsData[1];
    // profile_compatibility (1byte) sps[2] compatibility
    body[index++] = spsData[2];
    // AVCLevelIndication    (1byte) sps[3] Profile level
    body[index++] = spsData[3];
    // lengthSizeMinusOne    (1byte) 0xff   包长数据所使用的字节数
    body[index++] = 0xff;
    // sps + pps 的数据
    // sps number            (1byte) 0xe1   sps 个数
    body[index++] = 0xe1;
    // sps data length       (2byte) sps 长度
    body[index++] = (spsLen >> 8) & 0xFF;
    body[index++] = spsLen & 0xFF;
    // sps data                      sps 的内容
    memcpy(&body[index], spsData, spsLen);
    index += spsLen;
    // pps number            (1byte) 0x01 pps 个数
    body[index++] = 0x01;
    // pps data length       (2byte) pps 长度
    body[index++] = (ppsLen >> 8) & 0xFF;
    body[index++] = ppsLen & 0xFF;
    // pps data                      pps 的内容
    memcpy(&body[index], ppsData, ppsLen);

    // RTMPPacket 设置一些信息
    pPacket->m_hasAbsTimestamp = 0;
    pPacket->m_nTimeStamp = 0;
    pPacket->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
    pPacket->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    pPacket->m_nBodySize = bodySize;
    pPacket->m_nChannel = 0x04;
    pPacket->m_nInfoField2 = this->pRtmp->m_stream_id;

    pPacketQueue->push(pPacket);

}

void LivePush::pushVideoData(jbyte *videoData, jint dataLen, jboolean keyFrame) {
    // frame type : 1关键帧，2非关键帧 (4bit)
    // CodecID: 7表示 AVC(4bit),与 frame type 组合起来刚好是 1 个字节 0x17
    // fixed：0x01 0x00 0x00 0x00 (4byte),0x01 表示 NALU 单元

    // video data length  (4byte) video长度
    // video data

    // 数据的长度（大小） = dataLen + 头 9字节
    int bodySize = dataLen + 9;
    // 构建 RTMPPacket
    RTMPPacket *pPacket = (RTMPPacket *) malloc(sizeof(RTMPPacket));
    RTMPPacket_Alloc(pPacket, bodySize);
    RTMPPacket_Reset(pPacket);

    // 构建 body 按上面的一个一个开始赋值
    char *body = pPacket->m_body;
    int index = 0;
    // frame type : 1关键帧，2非关键帧 (4bit)
    // CodecID: 7表示 AVC(4bit),与 frame type 组合起来刚好是 1 个字节 0x17
    if (keyFrame)
        body[index++] = 0x17;
    else
        body[index++] = 0x27;
    // fixed：0x01 0x00 0x00 0x00 (4byte),0x01 表示 NALU 单元
    body[index++] = 0x01;
    body[index++] = 0x00;
    body[index++] = 0x00;
    body[index++] = 0x00;

    // video data length  (4byte) video长度
    body[index++] = (dataLen >> 24) & 0xFF;
    body[index++] = (dataLen >> 16) & 0xFF;
    body[index++] = (dataLen >> 8) & 0xFF;
    body[index++] = dataLen & 0xFF;
    // video data
    memcpy(&body[index], videoData, dataLen);

    // RTMPPacket 设置一些信息
    pPacket->m_headerType = RTMP_PACKET_SIZE_LARGE;
    pPacket->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    pPacket->m_hasAbsTimestamp = 0;
    pPacket->m_nTimeStamp = RTMP_GetTime() - startTime;
    pPacket->m_nBodySize = bodySize;
    pPacket->m_nChannel = 0x04;
    pPacket->m_nInfoField2 = this->pRtmp->m_stream_id;

    pPacketQueue->push(pPacket);

}

void LivePush::pushAudioData(jbyte *audioData, jint dataLen) {
    // 2个字节头信息：
        // 前四位表示音频数据格式 AAC ：十进制的10(A) -> 二进制：1010 -> 十六进制：A
        // 五六位表示采样率 十进制：0=5.5k   十进制：1=11k   十进制：2=22k   十进制：3(二进制：11)=44k
        // 七位表示采样的精度 0=8bits  1=16bits
        // 八位表示音频类型 0=mono   1=stereo
        // 我们这里算出来第一个字节是 0xAF = 1010 1111

    // 0x01 代表 aac 原始数据

    // 数据的长度（大小） = dataLen + 2
    int bodySize = dataLen + 2;
    // 构建 RTMPPacket
    RTMPPacket *pPacket = (RTMPPacket *) malloc(sizeof(RTMPPacket));
    RTMPPacket_Alloc(pPacket, bodySize);
    RTMPPacket_Reset(pPacket);

    // 构建 body 按上面的一个一个开始赋值
    char *body = pPacket->m_body;
    int index = 0;
    body[index++] = 0xAF;
    // 0x01 代表 aac 原始数据
    body[index++] = 0x01;

    // audio data
    memcpy(&body[index], audioData, dataLen);

    // RTMPPacket 设置一些信息
    pPacket->m_headerType = RTMP_PACKET_SIZE_LARGE;
    pPacket->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    pPacket->m_hasAbsTimestamp = 0;
    pPacket->m_nTimeStamp = RTMP_GetTime() - startTime;
    pPacket->m_nBodySize = bodySize;
    pPacket->m_nChannel = 0x04;
    pPacket->m_nInfoField2 = this->pRtmp->m_stream_id;

    pPacketQueue->push(pPacket);

}

void LivePush::stop() {
    isPushing = false;
    pPacketQueue->notify();
    pthread_join(initConnecTid, nullptr);// 这里会阻塞等待initConnectTid 线程结束，保证了不会空指针
    LOGE("等待停止了");
}
