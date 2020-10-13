package com.east.ffmpeg86.media;

import android.text.TextUtils;
import android.view.Surface;

import com.east.ffmpeg83.media.listener.MediaErrorListener;
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
        System.loadLibrary("video-player");
    }

    /**
     * url 可以是本地文件路径，也可以是 http 链接
     */
    private String url;

    private MediaErrorListener mErrorListener;
    private MediaPreparedListener mPreparedListener;

    public void setOnErrorListener(MediaErrorListener errorListener) {
        this.mErrorListener = errorListener;
    }

    public void setOnPreParedListener(MediaPreparedListener preparedListener) {
        this.mPreparedListener = preparedListener;
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

        nPlay();
    }

    public void prepare() {
        if (TextUtils.isEmpty(url)) {
            throw new NullPointerException("url is null, please call method setDataSource");
        }
        nPrepare(url);
    }


    /**
     *  异步准备
     */
    public void prepareAsync() {
        if (TextUtils.isEmpty(url)) {
            throw new NullPointerException("url is null, please call method setDataSource");
        }
        nPrepareAsync(url);
    }

    public void stop() {

    }

    private native void nPlay();

    private native void nPrepare(String url);

    private native void nPrepareAsync(String url);

    public native void setSurface(Surface surface);
}
