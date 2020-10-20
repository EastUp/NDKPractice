package com.east.record;

import android.content.Context;

import com.east.ffmpeg88livepush.R;

import javax.microedition.khronos.egl.EGLContext;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description:  灰色视频的录制
 *  @author: jamin
 *  @date: 2020/10/20 13:39
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class GrayVideoRecorder extends BaseVideoRecorder {
    private RecorderRenderer mRecorderRenderer;

    public GrayVideoRecorder(Context context, EGLContext eglContext, int textureId) {
        super(context, eglContext);
        mRecorderRenderer = new RecorderRenderer(context, textureId);
        setRenderer(mRecorderRenderer);
        mRecorderRenderer.setFragmentRender(R.raw.filter_fragment_gray);
    }
}
