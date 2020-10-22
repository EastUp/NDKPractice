package com.east.ffmpeg88livepush;

import android.content.Context;

import javax.microedition.khronos.egl.EGLContext;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description:  灰色视频的推流
 *  @author: jamin
 *  @date: 2020/10/20 13:39
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class GrayVideoPush extends BaseVideoPush {
    private PushRenderer mPushRenderer;

    public GrayVideoPush(Context context, EGLContext eglContext, int textureId) {
        super(context, eglContext);
        mPushRenderer = new PushRenderer(context, textureId);
        setRenderer(mPushRenderer);
        mPushRenderer.setFragmentRender(R.raw.filter_fragment_gray);
    }
}
