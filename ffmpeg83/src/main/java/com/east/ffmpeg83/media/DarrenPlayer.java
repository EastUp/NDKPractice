package com.east.ffmpeg83.media;

import android.text.TextUtils;

import com.east.ffmpeg83.media.listener.MediaCompleteListener;
import com.east.ffmpeg83.media.listener.MediaErrorListener;
import com.east.ffmpeg83.media.listener.MediaLoadListener;
import com.east.ffmpeg83.media.listener.MediaPauseListener;
import com.east.ffmpeg83.media.listener.MediaPreparedListener;
import com.east.ffmpeg83.media.listener.MediaProgressListener;

public class DarrenPlayer {

    // 加载 so 库文件
    static {
        System.loadLibrary("native-lib");
        // 不需要全部 load 相当于 android 调用其他方法类型，不需要全部 load
    }

    private String source;
    private boolean isPlaying = false;

    private MediaPreparedListener mPreparedListener;

    private MediaLoadListener mLoadListener;

    private MediaPauseListener mPauseListener;

    private MediaProgressListener mProgressListener;

    private MediaErrorListener mErrorListener;

    private MediaCompleteListener mCompleteListener;

    public void setOnPreparedListener(MediaPreparedListener preparedListener) {
        mPreparedListener = preparedListener;
    }

    public void setOnLoadListener(MediaLoadListener loadListener) {
        mLoadListener = loadListener;
    }

    public void setOnPauseListener(MediaPauseListener pauseListener) {
        mPauseListener = pauseListener;
    }

    public void setOnProgressListener(MediaProgressListener progressListener) {
        mProgressListener = progressListener;
    }

    public void setOnErrorListener(MediaErrorListener errorListener) {
        mErrorListener = errorListener;
    }

    public void setOnCompleteListener(MediaCompleteListener completeListener) {
        mCompleteListener = completeListener;
    }

    // Call from jni
    private void onPrepared() {
        if (mPreparedListener != null) {
            mPreparedListener.onPrepared();
        }
    }

    // Called from jni
    private void onLoading(boolean loading) {
        if (mLoadListener != null) {
            mLoadListener.onLoad(loading);
        }
    }

    // Called from jni
    private void onProgress(int current, int total) {
        if (mProgressListener != null) {
            mProgressListener.onProgress(current, total);
        }
    }

    // Called from jni
    private void onError(int errorCode, String errorMsg) {
        if (mErrorListener != null) {
            mErrorListener.onError(errorCode, errorMsg);
        }
    }

    // Called from jni
    private void onComplete() {
        isPlaying = false;
        if (mCompleteListener != null) {
            mCompleteListener.onComplete();
        }
    }

    public void setDataSource(String source) {
        this.source = source;
    }

    public void prepare() {
        if (TextUtils.isEmpty(source)) {
            throw new NullPointerException("source is null, Please call setDataSource method Settings!");
        }
        nPrepare(source);
    }

    public void prepareAsync() {
        if (TextUtils.isEmpty(source)) {
            throw new NullPointerException("source is null, Please call setDataSource method Settings!");
        }
        nPrepareAsync(source);
    }

    public void start() {
        isPlaying = true;
        nStart();
    }

    public void pause() {
        isPlaying = false;
        nPause();
        if (mPauseListener != null) {
            mPauseListener.onPause(true);
        }
    }

    public void resume() {
        isPlaying = true;
        nResume();
        if (mPauseListener != null) {
            mPauseListener.onPause(false);
        }
    }

    private native void nPrepareAsync(String source);

    private native void nPrepare(String source);

    private native void nStart();

    private native void nPause();

    private native void nResume();

    public void stop() {
        nStop();
    }

    private native void nStop();

    public void seekTo(int seconds) {
        nSeekTo(seconds);
    }

    private native void nSeekTo(int seconds);

    public boolean isPlaying() {
        return isPlaying;
    }
}
