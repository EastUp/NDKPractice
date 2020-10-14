package com.east.ffmpegvideo.media.render;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.view.Surface;

import com.east.ffmpegvideo.R;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class DZRender implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {
    public static final int RENDER_YUV = 1;
    public static final int RENDER_MEDIA_CODEC = 2;

    private Context mContext;

    private final float[] vertexData = {
            -1f, -1f,
            1f, -1f,
            -1f, 1f,
            1f, 1f
    };

    private final float[] mTextureData = {
            0f, 1f,
            1f, 1f,
            0f, 0f,
            1f, 0f
    };

    private FloatBuffer mVertexBuffer;
    private FloatBuffer mTextureBuffer;
    private int mRenderType = RENDER_YUV;

    //yuv
    private int mYuvProgram;
    private int mYuvAvPosition;
    private int mYuvAfPosition;

    private int samplerY;
    private int samplerU;
    private int samplerV;
    private int[] mTextureYuv;

    private int mYuvWidth;
    private int mYuvHeight;
    private ByteBuffer y;
    private ByteBuffer u;
    private ByteBuffer v;

    private int mMediaCodecProgram;
    private int mMediaCodecAvPosition;
    private int mMediaCodecAfPosition;
    private int mMediaCodecSamplerOES;
    private int mMediaCodecTextureId;
    private SurfaceTexture mSurfaceTexture;
    private Surface mSurface;

    private SurfaceCreateListener mSurfaceCreateListener;
    private RenderListener mRenderListener;

    public DZRender(Context context) {
        this.mContext = context;
        mVertexBuffer = ByteBuffer.allocateDirect(vertexData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(vertexData);
        mVertexBuffer.position(0);

        mTextureBuffer = ByteBuffer.allocateDirect(mTextureData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(mTextureData);
        mTextureBuffer.position(0);
    }

    public void setRenderType(int mRenderType) {
        this.mRenderType = mRenderType;
    }

    public void setOnSurfaceCreateListener(SurfaceCreateListener onSurfaceCreateListener) {
        this.mSurfaceCreateListener = onSurfaceCreateListener;
    }

    public void setOnRenderListener(RenderListener onRenderListener) {
        this.mRenderListener = onRenderListener;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        initRenderYUV();
        initRenderMediaCodec();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        if (mRenderType == RENDER_YUV) {
            renderYUV();
        } else if (mRenderType == RENDER_MEDIA_CODEC) {
            renderMediaCodec();
        }
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    @Override
    public void onFrameAvailable(SurfaceTexture mSurfaceTexture) {
        if (mRenderListener != null) {
            mRenderListener.onRender();
        }
    }

    private void initRenderYUV() {
        String vertexSource = Utils.readRawTxt(mContext, R.raw.vertex_shader);
        String fragmentSource = Utils.readRawTxt(mContext, R.raw.fragment_yuv);
        mYuvProgram = Utils.createProgram(vertexSource, fragmentSource);

        mYuvAvPosition = GLES20.glGetAttribLocation(mYuvProgram, "av_Position");
        mYuvAfPosition = GLES20.glGetAttribLocation(mYuvProgram, "af_Position");

        samplerY = GLES20.glGetUniformLocation(mYuvProgram, "samplerY");
        samplerU = GLES20.glGetUniformLocation(mYuvProgram, "samplerU");
        samplerV = GLES20.glGetUniformLocation(mYuvProgram, "samplerV");

        mTextureYuv = new int[3];
        GLES20.glGenTextures(3, mTextureYuv, 0);

        for (int i = 0; i < 3; i++) {
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureYuv[i]);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        }
    }

    public void setYUVRenderData(int width, int height, byte[] y, byte[] u, byte[] v) {
        this.mYuvWidth = width;
        this.mYuvHeight = height;
        this.y = ByteBuffer.wrap(y);
        this.u = ByteBuffer.wrap(u);
        this.v = ByteBuffer.wrap(v);
    }

    private void renderYUV() {
        if (mYuvWidth > 0 && mYuvHeight > 0 && y != null && u != null && v != null) {
            GLES20.glUseProgram(mYuvProgram);

            GLES20.glEnableVertexAttribArray(mYuvAvPosition);
            GLES20.glVertexAttribPointer(mYuvAvPosition, 2, GLES20.GL_FLOAT, false, 8, mVertexBuffer);

            GLES20.glEnableVertexAttribArray(mYuvAfPosition);
            GLES20.glVertexAttribPointer(mYuvAfPosition, 2, GLES20.GL_FLOAT, false, 8, mTextureBuffer);

            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureYuv[0]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, mYuvWidth, mYuvHeight, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, y);

            GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureYuv[1]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, mYuvWidth / 2, mYuvHeight / 2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, u);

            GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureYuv[2]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, mYuvWidth / 2, mYuvHeight / 2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, v);

            GLES20.glUniform1i(samplerY, 0);
            GLES20.glUniform1i(samplerU, 1);
            GLES20.glUniform1i(samplerV, 2);

            y.clear();
            u.clear();
            v.clear();
        }
    }

    private void initRenderMediaCodec() {
        String vertexSource = Utils.readRawTxt(mContext, R.raw.vertex_shader);
        String fragmentSource = Utils.readRawTxt(mContext, R.raw.fragment_mediacodec);
        mMediaCodecProgram = Utils.createProgram(vertexSource, fragmentSource);

        mMediaCodecAvPosition = GLES20.glGetAttribLocation(mMediaCodecProgram, "av_Position");
        mMediaCodecAfPosition = GLES20.glGetAttribLocation(mMediaCodecProgram, "af_Position");
        mMediaCodecSamplerOES = GLES20.glGetUniformLocation(mMediaCodecProgram, "sTexture");

        int[] textureIds = new int[1];
        GLES20.glGenTextures(1, textureIds, 0);
        mMediaCodecTextureId = textureIds[0];

        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

        mSurfaceTexture = new SurfaceTexture(mMediaCodecTextureId);
        mSurface = new Surface(mSurfaceTexture);
        mSurfaceTexture.setOnFrameAvailableListener(this);

        if (mSurfaceCreateListener != null) {
            mSurfaceCreateListener.onSurfaceCreate(mSurface);
        }
    }

    private void renderMediaCodec() {
        mSurfaceTexture.updateTexImage();
        GLES20.glUseProgram(mMediaCodecProgram);

        GLES20.glEnableVertexAttribArray(mMediaCodecAvPosition);
        GLES20.glVertexAttribPointer(mMediaCodecAvPosition, 2, GLES20.GL_FLOAT, false, 8, mVertexBuffer);

        GLES20.glEnableVertexAttribArray(mMediaCodecAfPosition);
        GLES20.glVertexAttribPointer(mMediaCodecAfPosition, 2, GLES20.GL_FLOAT, false, 8, mTextureBuffer);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, mMediaCodecTextureId);
        GLES20.glUniform1i(mMediaCodecSamplerOES, 0);
    }

    public interface SurfaceCreateListener {
        void onSurfaceCreate(Surface surface);
    }

    public interface RenderListener {
        void onRender();
    }
}
