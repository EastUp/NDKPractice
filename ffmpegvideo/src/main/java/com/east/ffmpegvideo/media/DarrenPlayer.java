package com.east.ffmpegvideo.media;

import android.media.MediaCodec;
import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;

import com.east.ffmpegvideo.media.listener.MediaCompleteListener;
import com.east.ffmpegvideo.media.listener.MediaErrorListener;
import com.east.ffmpegvideo.media.listener.MediaLoadListener;
import com.east.ffmpegvideo.media.listener.MediaPauseListener;
import com.east.ffmpegvideo.media.listener.MediaPreparedListener;
import com.east.ffmpegvideo.media.listener.MediaProgressListener;
import com.east.ffmpegvideo.media.render.DZRender;
import com.east.ffmpegvideo.media.render.DZVideoView;

import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;

public class DarrenPlayer {

    //--------------- 硬解码相关 start ---------------
    private Surface mSurface;
    private MediaCodec mMediaCodec;
    private MediaCodec.BufferInfo mOutBufferInfo;
    private static final Map<String, String> mCodecMap = new HashMap<>(2);

    static {
        /**
         "video/x-vnd.on2.vp8" - VP8 video (i.e. video in .webm)
         "video/x-vnd.on2.vp9" - VP9 video (i.e. video in .webm)
         "video/avc" - H.264/AVC video
         "video/hevc" - H.265/HEVC video
         "video/mp4v-es" - MPEG4 video
         "video/3gpp" - H.263 video
         "audio/3gpp" - AMR narrowband audio
         "audio/amr-wb" - AMR wideband audio
         "audio/mpeg" - MPEG1/2 audio layer III
         "audio/mp4a-latm" - AAC audio (note, this is raw AAC packets, not packaged in LATM!)
         "audio/vorbis" - vorbis audio
         "audio/g711-alaw" - G.711 alaw audio
         "audio/g711-mlaw" - G.711 ulaw audio
         */
        mCodecMap.put("h264", "video/avc");
        mCodecMap.put("h265", "video/hevc");
    }

    private static final String findMediaCodecName(String codeName) {
        if (mCodecMap.containsKey(codeName)) {
            return mCodecMap.get(codeName);
        }
        return "";
    }

    /**
     * 是否支持硬解码，Call from jni
     *
     * @param codeName ffmpeg 的解码器名字
     * @return 是否支持硬解码
     */
    private final boolean isSupportStiffCodec(String codeName) {
        int codecCount = MediaCodecList.getCodecCount();
        for (int i = 0; i < codecCount; i++) {
            String[] supportedTypes = MediaCodecList.getCodecInfoAt(i).getSupportedTypes();
            for (String type : supportedTypes) {
                if (type.equals(findMediaCodecName(codeName))) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * 初始化 MediaCodec
     * Call from jni
     */
    private void initMediaCodec(String codecName, int width, int height, byte[] csd0, byte[] csd1) {
        if (mSurface == null) {
            Log.d("TAG", "surface is null");
            return;
        }
        try {
            mOutBufferInfo = new MediaCodec.BufferInfo();
            mVideoView.getWlRender().setRenderType(DZRender.RENDER_MEDIA_CODEC);
            String mime = findMediaCodecName(codecName);
            MediaFormat mediaFormat = MediaFormat.createVideoFormat(mime, width, height);
            mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
            mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd0));
            mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd1));
            mMediaCodec = MediaCodec.createDecoderByType(mime);
            mMediaCodec.configure(mediaFormat, mSurface, null, 0);
            mMediaCodec.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void decodePacket(int dataSize, byte[] data) {
        if (mSurface != null && dataSize > 0 && data != null && mMediaCodec != null) {
            int intputBufferIndex = mMediaCodec.dequeueInputBuffer(10);
            if (intputBufferIndex >= 0) {
                ByteBuffer byteBuffer = mMediaCodec.getInputBuffers()[intputBufferIndex];
                byteBuffer.clear();
                byteBuffer.put(data);
                mMediaCodec.queueInputBuffer(intputBufferIndex, 0, dataSize, 0, 0);
            }
            int outputBufferIndex = mMediaCodec.dequeueOutputBuffer(mOutBufferInfo, 10);
            while (outputBufferIndex >= 0) {
                mMediaCodec.releaseOutputBuffer(outputBufferIndex, true);
                outputBufferIndex = mMediaCodec.dequeueOutputBuffer(mOutBufferInfo, 10);
            }
        }
    }

    private void releaseMediaCodec() {
        if (mMediaCodec != null) {
            mMediaCodec.flush();
            mMediaCodec.stop();
            mMediaCodec.release();
            mMediaCodec = null;
            mOutBufferInfo = null;
        }
    }
    //--------------- 硬解码相关  end  ---------------


    private DZVideoView mVideoView;

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

    // Called from jni
    private void onRenderYUV420P(int width, int height, byte[] y, byte[] u, byte[] v) {
        if (mVideoView != null) {
            mVideoView.setYUVData(width, height, y, u, v);
        }
    }

    public void setVideoView(DZVideoView videoView) {
        this.mVideoView = videoView;
        mVideoView.getWlRender().setOnSurfaceCreateListener(new DZRender.SurfaceCreateListener() {
            @Override
            public void onSurfaceCreate(Surface surface) {
                mSurface = surface;
            }
        });
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
        releaseMediaCodec();
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
