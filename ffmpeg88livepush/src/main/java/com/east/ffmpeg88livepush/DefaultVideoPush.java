package com.east.ffmpeg88livepush;

import android.content.Context;

import com.east.record.RecorderRenderer;

import javax.microedition.khronos.egl.EGLContext;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description:  默认彩色视频的推流
 *  @author: jamin
 *  @date: 2020/10/21 11:05
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class DefaultVideoPush extends BaseVideoPush {
    public DefaultVideoPush(Context context, EGLContext eglContext, int textureId) {
        super(context, eglContext);
        setRenderer(new RecorderRenderer(context, textureId));
    }
}
