package com.east.ffmpegvideo.media.render;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class DZVideoView extends GLSurfaceView {
    private DZRender mDZRender;

    public DZVideoView(Context context) {
        this(context, null);
    }

    public DZVideoView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(2);
        mDZRender = new DZRender(context);
        setRenderer(mDZRender);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        mDZRender.setOnRenderListener(new DZRender.RenderListener() {
            @Override
            public void onRender() {
                requestRender();
            }
        });
    }

    public void setYUVData(int width, int height, byte[] y, byte[] u, byte[] v)
    {
        if(mDZRender != null)
        {
            mDZRender.setYUVRenderData(width, height, y, u, v);
            requestRender();
        }
    }

    public DZRender getWlRender() {
        return mDZRender;
    }
}
