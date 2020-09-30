package com.east.ffmpeg83;

import android.content.Context;
import android.graphics.PixelFormat;
import android.media.AudioTrack;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.File;

public class DVideoView extends SurfaceView {

    // 加载 so 库文件
    static {
        System.loadLibrary("native-lib");
        // 不需要全部 load 相当于 android 调用其他方法类型，不需要全部 load
    }

    public DVideoView(Context context) {
        this(context, null);
    }

    public DVideoView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public DVideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        //初始化，SufaceView绘制的像素格式
        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);
    }

    public void playFile(File file) {
        // nativeDecodeVideo(file.getAbsolutePath(), getHolder().getSurface());
        nativeDecodeAudio(file.getAbsolutePath());
    }

    private native void nativeDecodeAudio(String absolutePath);

    private native void nativeDecodeVideo(String absolutePath, Surface surface);

    public void playUrl(String url) {

    }
}
