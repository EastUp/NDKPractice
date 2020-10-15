package com.east.ffmpeg88livepush;

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

    private String mLiveUrl;
    private ConnectListener mConnectListener;

    public LivePush(String mLiveUrl) {
        this.mLiveUrl = mLiveUrl;
    }

    /**
     *  初始化连接
     */
    public void initConnect(){
        nInitConnect(mLiveUrl);
    }

    private native void nInitConnect(String mLiveUrl);

    public void setConnectListener(ConnectListener connectListener) {
        this.mConnectListener = connectListener;
    }

    private void onConnectError(int code, String msg){
        if(mConnectListener!=null)
            mConnectListener.onConnectError(code,msg);
    }

    private void onConnectSuccess(){
        if(mConnectListener!=null)
            mConnectListener.onConnectSuccesss();
    }

}
