package com.east.ffmpeg86.media;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.east.ffmpeg83.media.listener.MediaPreparedListener;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 * @description: 视频播放的View
 * @author: jamin
 * @date: 2020/10/12
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class JaminVideoView extends SurfaceView implements MediaPreparedListener{
    private JaminPlayer mPlayer;

    public JaminVideoView(Context context) {
        this(context,null);
    }

    public JaminVideoView(Context context, AttributeSet attrs) {
        this(context, attrs,0);
    }

    public JaminVideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        // 设置显示的像素格式
        SurfaceHolder holder = getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);
        mPlayer = new JaminPlayer();
        mPlayer.setOnPreParedListener(this);

    }

    public void play(String url){
        stop();
        mPlayer.setDataSource(url);
        mPlayer.prepareAsync();
    }

    /**
     *  停止方法，释放上一个视频的内存
     */
    private void stop() {
    }


    @Override
    public void onPrepared() {
        mPlayer.play();
    }
}
