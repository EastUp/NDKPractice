package com.east.ffmpeg83.media;

import android.text.TextUtils;

import com.east.ffmpeg83.media.listener.MediaErrorListener;
import com.east.ffmpeg83.media.listener.MediaInfoListener;
import com.east.ffmpeg83.media.listener.MediaPreparedListener;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 * @description: 音乐播放器的逻辑处理类
 * @author: jamin
 * @date: 2020/10/9
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class JaminPlayer {
    static {
        System.loadLibrary("music-player");
    }

    /**
     * url 可以是本地文件路径，也可以是 http 链接
     */
    private String url;

    private MediaErrorListener mErrorListener;
    private MediaPreparedListener mPreparedListener;
    private MediaInfoListener mInfoListener;

    public void setOnErrorListener(MediaErrorListener errorListener) {
        this.mErrorListener = errorListener;
    }

    public void setOnPreParedListener(MediaPreparedListener preparedListener) {
        this.mPreparedListener = preparedListener;
    }

    public void setOnInfoListener(MediaInfoListener infoListener) {
        this.mInfoListener = infoListener;
    }

    private void musicInfo(int sampleRate,int channels){
        if(mInfoListener!=null)
            mInfoListener.musicInfo(sampleRate,channels);
    }

    private void callbackPcm(byte[] pcmData,int size){
        if(mInfoListener!=null)
            mInfoListener.callbackPcm(pcmData,size);
    }

    // called from jni
    private void onError(int code,String msg){
        if(mErrorListener!=null)
            mErrorListener.onError(code,msg);
    }

    // called from jni
    private void onPrepared(){
        if(mPreparedListener!=null)
            mPreparedListener.onPrepared();
    }

    public void setDataSource(String url){
        this.url = url;
    }

    public void play(){
        if(TextUtils.isEmpty(url)){
            throw new NullPointerException("url is null,please call method setDataSource");
        }

        nPlay(url);
    }

    private native void nPlay(String url);

    public void prepare() {
        if (TextUtils.isEmpty(url)) {
            throw new NullPointerException("url is null, please call method setDataSource");
        }
        nPrepare(url);
    }

    private native void nPrepare(String url);

    /**
     *  异步准备
     */
    public void prepareAsync() {
        if (TextUtils.isEmpty(url)) {
            throw new NullPointerException("url is null, please call method setDataSource");
        }
        nPrepareAsync(url);
    }

    private native void nPrepareAsync(String url);

    public void stop(){
        nStop();
    }

    private native void nStop();
}
