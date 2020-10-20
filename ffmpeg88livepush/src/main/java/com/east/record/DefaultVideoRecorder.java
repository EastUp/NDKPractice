package com.east.record;

import android.content.Context;

import javax.microedition.khronos.egl.EGLContext;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description:  默认彩色视频的录制
 *  @author: jamin
 *  @date: 2020/10/20 13:39
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class DefaultVideoRecorder extends BaseVideoRecorder {
    public DefaultVideoRecorder(Context context, EGLContext eglContext, int textureId) {
        super(context, eglContext);
        setRenderer(new RecorderRenderer(context, textureId));
    }
}
