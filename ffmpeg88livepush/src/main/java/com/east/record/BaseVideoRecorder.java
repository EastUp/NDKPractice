package com.east.record;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.Surface;

import com.east.ffmpeg83.media.JaminPlayer;
import com.east.ffmpeg83.media.listener.MediaErrorListener;
import com.east.ffmpeg83.media.listener.MediaInfoListener;
import com.east.ffmpeg83.media.listener.MediaPreparedListener;
import com.east.opengl.EglHelper;

import org.jetbrains.annotations.NotNull;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.concurrent.CyclicBarrier;

import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.opengles.GL10;


/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description:
 *  @author: jamin
 *  @date: 2020/10/20 13:38
 * |---------------------------------------------------------------------------------------------------------------|
 */
public abstract class BaseVideoRecorder {

    private WeakReference<BaseVideoRecorder> mVideoRecorderWr = new WeakReference<>(this);
    /**
     * 硬编码 MediaCodec 的 surface
     */
    private Surface mSurface;
    /**
     * 相机共享的 egl 上下文
     */
    private EGLContext mEglContext;
    private Context mContext;

    private GLSurfaceView.Renderer mRenderer;

    private MediaMuxer mMediaMuxer;
    private VideoRenderThread mRenderThread;
    private VideoEncoderThread mVideoThread;
    private AudioEncoderThread mAudioThread;

    private MediaCodec mVideoCodec;
    private MediaCodec mAudioCodec;
    private CyclicBarrier mStartCb = new CyclicBarrier(2);
    private CyclicBarrier mDestroyCb = new CyclicBarrier(2);

    private JaminPlayer mMusicPlayer;

    public void setRenderer(GLSurfaceView.Renderer renderer) {
        this.mRenderer = renderer;
        mRenderThread = new VideoRenderThread(mVideoRecorderWr);
    }

    public BaseVideoRecorder(Context context, EGLContext eglContext) {
        this.mContext = context;
        this.mEglContext = eglContext;
        mMusicPlayer = new JaminPlayer();

        mMusicPlayer.setOnPreParedListener(new MediaPreparedListener() {
            @Override
            public void onPrepared() {
                mMusicPlayer.play();
                start();
            }
        });
        mMusicPlayer.setOnErrorListener(new MediaErrorListener() {
            @Override
            public void onError(int code, @NotNull String msg) {
                Log.e("TAG","音乐播放错误："+msg);
            }
        });

        mMusicPlayer.setOnInfoListener(mMediaInfoListener);
    }

    private void start() {
        mRenderThread.start();
        mVideoThread.start();
        mAudioThread.start();
    }

    private MediaInfoListener mMediaInfoListener = new MediaInfoListener() {
        private long mAudioPts = 0;
        private int mSampleRate = 0;
        private int mChannels = 0;

        @Override
        public void musicInfo(int sampleRate, int channels) {
            // 获取了音频的信息
            try {
                initAudioCodec(sampleRate, channels);
            } catch (Exception e) {
                e.printStackTrace();
            }
            this.mSampleRate = sampleRate;
            this.mChannels = channels;
        }

        @Override
        public void callbackPcm(byte[] pcmData, int size) {
            // 把数据写入到 mAudioCodec 的 InputBuffer
            int inputBufferIndex = mAudioCodec.dequeueInputBuffer(0);
            if (inputBufferIndex >= 0) {
                ByteBuffer byteBuffer = mAudioCodec.getInputBuffers()[inputBufferIndex];
                byteBuffer.clear();
                byteBuffer.put(pcmData);

                // pts  44100 * 2 *2
                mAudioPts += size * 1000000 / mSampleRate * mChannels * 2;
                // size 22050*2*2
                mAudioCodec.queueInputBuffer(inputBufferIndex, 0, size, mAudioPts, 0);
            }
        }
    };

    private void initAudioCodec(int sampleRate, int channels) throws IOException {
        MediaFormat audioFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, channels);
        audioFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);
        audioFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
        audioFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, sampleRate * channels * 2);
        // 创建音频编码器
        mAudioCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC);
        mAudioCodec.configure(audioFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        // 开启一个编码采集 音乐播放器回调的 PCM 数据，合成视频
        mAudioThread = new AudioEncoderThread(mVideoRecorderWr);
        // 开启 start AudioCodec
        mAudioCodec.start();
    }

    /**
     * 初始化参数
     *
     * @param audioPath   背景音乐的地址
     * @param outPath     输出文件的路径
     * @param videoWidth  录制的宽度
     * @param videoHeight 录制的高度
     */
    public void initVideo(String audioPath, String outPath, int videoWidth, int videoHeight) {
        try {
            mRenderThread.setSize(videoWidth, videoHeight);
            mMediaMuxer = new MediaMuxer(outPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
            initVideoCodec(videoWidth, videoHeight);
            mMusicPlayer.setDataSource(audioPath);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void startRecord() {
        mMusicPlayer.prepareAsync();
    }

    public void stopRecord() {
        mMusicPlayer.stop();
        mRenderThread.requestExit();
        mVideoThread.requestExit();
        mAudioThread.requestExit();
    }

    /**
     * 初始化视频的 MediaCodec
     *
     * @param width
     * @param height
     */
    private void initVideoCodec(int width, int height) throws IOException {
        MediaFormat videoFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, width, height);
        // 设置颜色格式
        videoFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        videoFormat.setInteger(MediaFormat.KEY_BIT_RATE, width * height * 4);
        // 设置帧率
        videoFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 24);
        // 设置 I 帧的间隔时间
        videoFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1);

        // 创建编码器
        mVideoCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC);
        mVideoCodec.configure(videoFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);

        mSurface = mVideoCodec.createInputSurface();
        // 开启一个编码采集 InputSurface 上的数据，合成视频
        mVideoThread = new VideoEncoderThread(mVideoRecorderWr);
    }

    /**
     * 视频的编码线程
     */
    private static final class VideoEncoderThread extends Thread {
        private final MediaMuxer mMediaMuxer;
        private WeakReference<BaseVideoRecorder> mVideoRecorderWr;
        private volatile boolean mShouldExit = false;
        private MediaCodec mVideoCodec;
        private MediaCodec.BufferInfo mBufferInfo;
        private int mVideoTrackIndex = -1;
        private long mVideoPts = 0;
        private final CyclicBarrier mStartCb, mDestroyCb;

        public VideoEncoderThread(WeakReference<BaseVideoRecorder> videoRecorderWr) {
            this.mVideoRecorderWr = videoRecorderWr;
            mVideoCodec = videoRecorderWr.get().mVideoCodec;
            mMediaMuxer = videoRecorderWr.get().mMediaMuxer;
            mBufferInfo = new MediaCodec.BufferInfo();
            mStartCb = videoRecorderWr.get().mStartCb;
            mDestroyCb = videoRecorderWr.get().mDestroyCb;
        }

        @Override
        public void run() {
            try {
                mVideoCodec.start();

                while (true) {
                    if (mShouldExit) {
                        return;
                    }

                    BaseVideoRecorder videoRecorder = mVideoRecorderWr.get();
                    if (videoRecorder == null) {
                        return;
                    }

                    // 代码先不写，先测试，从 surface 上获取数据，编码成 h264 ,通过 MediaMuxer 合成 mp4
                    int outputBufferIndex = mVideoCodec.dequeueOutputBuffer(mBufferInfo, 0);
                    if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                        mVideoTrackIndex = mMediaMuxer.addTrack(mVideoCodec.getOutputFormat());
                        mMediaMuxer.start();
                        mStartCb.await();
                    } else {
                        while (outputBufferIndex >= 0) {
                            // 获取数据
                            ByteBuffer outBuffer = mVideoCodec.getOutputBuffers()[outputBufferIndex];
                            outBuffer.position(mBufferInfo.offset);
                            outBuffer.limit(mBufferInfo.offset + mBufferInfo.size);

                            // 修改 pts
                            if (mVideoPts == 0) {
                                mVideoPts = mBufferInfo.presentationTimeUs;
                            }
                            mBufferInfo.presentationTimeUs -= mVideoPts;

                            // 写入数据
                            mMediaMuxer.writeSampleData(mVideoTrackIndex, outBuffer, mBufferInfo);

                            // 回调当前录制的时间
                            if (videoRecorder.mRecordListener != null) {
                                videoRecorder.mRecordListener.onTime(mBufferInfo.presentationTimeUs / 1000);
                            }

                            mVideoCodec.releaseOutputBuffer(outputBufferIndex, false);
                            outputBufferIndex = mVideoCodec.dequeueOutputBuffer(mBufferInfo, 0);
                        }
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                onDestroy();
            }
        }

        private void onDestroy() {
            try {
                mVideoCodec.stop();
                mVideoCodec.release();
                mDestroyCb.await();
                mMediaMuxer.stop();
                mMediaMuxer.release();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        private void requestExit() {
            mShouldExit = true;
        }
    }

    /**
     * 视频的编码线程
     */
    private static final class AudioEncoderThread extends Thread {
        private final MediaMuxer mMediaMuxer;
        private WeakReference<BaseVideoRecorder> mVideoRecorderWr;
        private volatile boolean mShouldExit = false;
        private MediaCodec mAudioCodec;
        private MediaCodec.BufferInfo mBufferInfo;
        private int mAudioTrackIndex = -1;
        private long mAudioPts = 0;
        private final CyclicBarrier mStartCb, mDestroyCb;

        public AudioEncoderThread(WeakReference<BaseVideoRecorder> videoRecorderWr) {
            this.mVideoRecorderWr = videoRecorderWr;
            mAudioCodec = videoRecorderWr.get().mAudioCodec;
            mMediaMuxer = videoRecorderWr.get().mMediaMuxer;
            mBufferInfo = new MediaCodec.BufferInfo();
            mStartCb = videoRecorderWr.get().mStartCb;
            mDestroyCb = videoRecorderWr.get().mDestroyCb;
        }

        @Override
        public void run() {
            try {
                while (true) {
                    if (mShouldExit) {
                        return;
                    }

                    BaseVideoRecorder videoRecorder = mVideoRecorderWr.get();
                    if (videoRecorder == null) {
                        return;
                    }

                    // 获取音频数据，那这个音频数据从哪里来？音乐播放器里面来，pcm 数据
                    int outputBufferIndex = mAudioCodec.dequeueOutputBuffer(mBufferInfo, 0);
                    if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                        mAudioTrackIndex = mMediaMuxer.addTrack(mAudioCodec.getOutputFormat());
                        mStartCb.await();
                        // mMediaMuxer.start();
                    } else {
                        while (outputBufferIndex >= 0) {
                            // 获取数据
                            ByteBuffer outBuffer = mAudioCodec.getOutputBuffers()[outputBufferIndex];
                            outBuffer.position(mBufferInfo.offset);
                            outBuffer.limit(mBufferInfo.offset + mBufferInfo.size);

                            // 修改 pts
                            if (mAudioPts == 0) {
                                mAudioPts = mBufferInfo.presentationTimeUs;
                            }
                            mBufferInfo.presentationTimeUs -= mAudioPts;

                            // 写入数据
                            mMediaMuxer.writeSampleData(mAudioTrackIndex, outBuffer, mBufferInfo);

                            mAudioCodec.releaseOutputBuffer(outputBufferIndex, false);
                            outputBufferIndex = mAudioCodec.dequeueOutputBuffer(mBufferInfo, 0);
                        }
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                onDestroy();
            }
        }

        private void onDestroy() {
            try {
                mAudioCodec.stop();
                mAudioCodec.release();
                mDestroyCb.await();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        private void requestExit() {
            mShouldExit = true;
        }
    }

    private RecordListener mRecordListener;

    public void setOnRecordListener(RecordListener recordListener) {
        this.mRecordListener = recordListener;
    }

    public interface RecordListener {
        void onTime(long times);
    }

    /**
     * 视频的渲染线程
     */
    private static final class VideoRenderThread extends Thread {
        private WeakReference<BaseVideoRecorder> mVideoRecorderWr;
        private volatile boolean mShouldExit = false;
        private EglHelper mEglHelper;
        private boolean mHashCreateContext = false;
        private boolean mHashSurfaceCreated = false;
        private boolean mHashSurfaceChanged = false;
        private int mWidth;
        private int mHeight;

        public VideoRenderThread(WeakReference<BaseVideoRecorder> videoRecorderWr) {
            this.mVideoRecorderWr = videoRecorderWr;
            mEglHelper = new EglHelper();
        }

        @Override
        public void run() {

            try {
                while (true) {
                    if (mShouldExit) {
                        return;
                    }

                    BaseVideoRecorder videoRecorder = mVideoRecorderWr.get();
                    if (videoRecorder == null) {
                        return;
                    }

                    // 1. 创建 EGL 上下文
                    if (!mHashCreateContext) {
                        mEglHelper.initCreateEgl(videoRecorder.mSurface, videoRecorder.mEglContext);
                        mHashCreateContext = true;
                    }

                    // 回调 Render
                    GL10 gl = (GL10) mEglHelper.getEglContext().getGL();
                    if (!mHashSurfaceCreated) {
                        videoRecorder.mRenderer.onSurfaceCreated(gl, mEglHelper.getEGLConfig());
                        mHashSurfaceCreated = true;
                    }

                    if (!mHashSurfaceChanged) {
                        videoRecorder.mRenderer.onSurfaceChanged(gl, mWidth, mHeight);
                        mHashSurfaceChanged = true;
                    }

                    videoRecorder.mRenderer.onDrawFrame(gl);

                    mEglHelper.swapBuffers();

                    // 60 fps
                    Thread.sleep(16);
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                onDestroy();
            }
        }

        private void onDestroy() {
            mEglHelper.destroy();
        }

        private void requestExit() {
            mShouldExit = true;
        }

        public void setSize(int width, int height) {
            this.mWidth = width;
            this.mHeight = height;
        }
    }
}
