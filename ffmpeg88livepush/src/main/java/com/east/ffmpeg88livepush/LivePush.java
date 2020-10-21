package com.east.ffmpeg88livepush;

import android.os.Handler;
import android.os.Looper;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 * @description:
 * @author: jamin
 * @date: 2020/10/15
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class LivePush {
    static {
        System.loadLibrary("live-push");
    }

    private Handler mMainHandler = new Handler(Looper.myLooper());

    private String mLiveUrl;
    private ConnectListener mConnectListener;

    public LivePush(String mLiveUrl) {
        this.mLiveUrl = mLiveUrl;
    }

    /**
     * 初始化连接
     */
    public void initConnect() {
        nInitConnect(mLiveUrl);
    }

    private native void nInitConnect(String mLiveUrl);

    public void setConnectListener(ConnectListener connectListener) {
        this.mConnectListener = connectListener;
    }

    // called from jni
    private void onConnectError(int code, String msg) {
        stop();
        if (mConnectListener != null)
            mConnectListener.onConnectError(code, msg);
    }

    // called from jni
    private void onConnectSuccess() {
        if (mConnectListener != null)
            mConnectListener.onConnectSuccesss();
    }

    // 销毁 jni 层的内存
    public void stop() {
        mMainHandler.post(new Runnable() {
            @Override
            public void run() {
                nStop();
            }
        });
    }

    private native void nStop();

    /**
     * 将sps 和 pps 推送到流媒体服务器上去
     * @param spsData
     * @param spsLen
     * @param ppsData
     * @param ppsLen
     */
    public native void pushSpsPps(byte[] spsData, int spsLen, byte[] ppsData, int ppsLen);

    /**
     *  将视频数据推送到流媒体服务器上
     * @param videoData 视频数据
     * @param dataLen 视频长度
     * @param keyFrame 是否是关键帧
     */
    public native void pushVideo(byte[] videoData, int dataLen, boolean keyFrame);

    /**
     * 将音频数据推送到流媒体服务器上去
     * @param audioData
     * @param dataLen
     */
    public native void pushAudio(byte[] audioData, int dataLen);
}
