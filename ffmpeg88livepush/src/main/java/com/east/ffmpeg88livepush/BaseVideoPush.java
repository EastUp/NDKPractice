package com.east.ffmpeg88livepush;

import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaRecorder;
import android.opengl.GLSurfaceView;
import android.view.Surface;

import com.east.opengl.EglHelper;

import org.jetbrains.annotations.NotNull;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.opengles.GL10;


/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 * @description:
 * @author: jamin
 * @date: 2020/10/21 11:05
 * |---------------------------------------------------------------------------------------------------------------|
 */
public abstract class BaseVideoPush {
    private static final int AUDIO_SAMPLE_RATE = 44100;
    private static final int AUDIO_CHANNELS = 2;
    private LivePush mLivePush;
    private WeakReference<BaseVideoPush> mVideoRecorderWr = new WeakReference<>(this);
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

    private VideoRenderThread mRenderThread;
    private VideoEncoderThread mVideoThread;
    private AudioEncoderThread mAudioEncoderThread;
    private AudioRecordThread mAudioRecordThread;

    private MediaCodec mVideoCodec;
    private MediaCodec mAudioCodec;

    public void setRenderer(GLSurfaceView.Renderer renderer) {
        this.mRenderer = renderer;
        mRenderThread = new VideoRenderThread(mVideoRecorderWr);
    }

    public BaseVideoPush(Context context, EGLContext eglContext) {
        this.mContext = context;
        this.mEglContext = eglContext;
    }

    /**
     *  初始化音频的 MediaCodec
     * @param sampleRate
     * @param channels
     * @throws IOException
     */
    private void initAudioCodec(int sampleRate, int channels) throws IOException {
        MediaFormat audioFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, channels);
        audioFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);
        audioFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
        audioFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, sampleRate * channels * 2);
        // 创建音频编码器
        mAudioCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC);
        mAudioCodec.configure(audioFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);

        mAudioRecordThread = new AudioRecordThread(mVideoRecorderWr);
        // 开启一个编码采集 音乐播放器回调的 PCM 数据，合成视频
        mAudioEncoderThread = new AudioEncoderThread(mVideoRecorderWr);
    }

    /**
     * 初始化参数
     *
     * @param videoWidth  录制的宽度
     * @param videoHeight 录制的高度
     */
    public void initVideo(String liveUrl, int videoWidth, int videoHeight) {
        try {

            mLivePush = new LivePush(liveUrl);

            mLivePush.setConnectListener(new ConnectListener() {
                @Override
                public void onConnectError(int code, @NotNull String msg) {
                    if (mConnectListener != null)
                        mConnectListener.onConnectError(code, msg);
                }

                @Override
                public void onConnectSuccesss() {
                    start();
                    if (mConnectListener != null)
                        mConnectListener.onConnectSuccesss();
                }
            });

            mRenderThread.setSize(videoWidth, videoHeight);
            initVideoCodec(videoWidth, videoHeight);
            initAudioCodec(AUDIO_SAMPLE_RATE,AUDIO_CHANNELS);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void start() {
        mRenderThread.start();
        mVideoThread.start();
        mAudioEncoderThread.start();
        mAudioRecordThread.start();
    }

    public void startPush() {
        mLivePush.initConnect();
    }

    public void stopPush() {
        mRenderThread.requestExit();
        mVideoThread.requestExit();
        mAudioEncoderThread.requestExit();
        mAudioRecordThread.requestExit();
        mLivePush.stop();
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
        private WeakReference<BaseVideoPush> mVideoRecorderWr;
        private volatile boolean mShouldExit = false;
        private MediaCodec mVideoCodec;
        private MediaCodec.BufferInfo mBufferInfo;
        private long mVideoPts = 0;
        private byte[] mVideoSps, mVideoPps;

        public VideoEncoderThread(WeakReference<BaseVideoPush> videoRecorderWr) {
            this.mVideoRecorderWr = videoRecorderWr;
            mVideoCodec = videoRecorderWr.get().mVideoCodec;
            mBufferInfo = new MediaCodec.BufferInfo();
        }

        @Override
        public void run() {
            try {
                mVideoCodec.start();

                while (true) {
                    if (mShouldExit) {
                        return;
                    }

                    BaseVideoPush videoRecorder = mVideoRecorderWr.get();
                    if (videoRecorder == null) {
                        return;
                    }

                    // 代码先不写，先测试，从 surface 上获取数据，编码成 h264 ,通过 MediaMuxer 合成 mp4
                    int outputBufferIndex = mVideoCodec.dequeueOutputBuffer(mBufferInfo, 0);
                    if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {

                        // 获取 sps 和 pps
//                        Log.e("TAG", "获取 sps 和 pps");
                        ByteBuffer byteBuffer = mVideoCodec.getOutputFormat().getByteBuffer("csd-0");
                        mVideoSps = new byte[byteBuffer.remaining()];
                        byteBuffer.get(mVideoSps, 0, mVideoSps.length);

//                        Log.e("sps", bytesToHexString(mVideoSps));

                        byteBuffer = mVideoCodec.getOutputFormat().getByteBuffer("csd-1");
                        mVideoPps = new byte[byteBuffer.remaining()];
                        byteBuffer.get(mVideoPps, 0, mVideoPps.length);

//                        Log.e("pps", bytesToHexString(mVideoPps));
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

                            // 在关键帧之前先把 sps 和 pps 推到流媒体服务器
                            if(mBufferInfo.flags == MediaCodec.BUFFER_FLAG_KEY_FRAME){
                                mVideoRecorderWr.get().mLivePush.pushSpsPps(mVideoSps,mVideoSps.length,
                                        mVideoPps,mVideoPps.length);
                            }

                            byte[] data = new byte[outBuffer.remaining()];
                            outBuffer.get(data, 0, data.length);
                            // 推送视频到流媒体服务器上
                            mVideoRecorderWr.get().mLivePush.pushVideo(data,data.length,
                                    mBufferInfo.flags == MediaCodec.BUFFER_FLAG_KEY_FRAME);

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
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        private void requestExit() {
            mShouldExit = true;
        }
    }

    /**
     * 音频的编码线程
     */
    private static final class AudioEncoderThread extends Thread {
        private WeakReference<BaseVideoPush> mVideoRecorderWr;
        private volatile boolean mShouldExit = false;
        private MediaCodec mAudioCodec;
        private MediaCodec.BufferInfo mBufferInfo;
        private long mAudioPts = 0;

        public AudioEncoderThread(WeakReference<BaseVideoPush> videoRecorderWr) {
            this.mVideoRecorderWr = videoRecorderWr;
            mAudioCodec = videoRecorderWr.get().mAudioCodec;
            mBufferInfo = new MediaCodec.BufferInfo();
        }

        @Override
        public void run() {
            try {
                // 开启 start AudioCodec
                mAudioCodec.start();

                while (true) {
                    if (mShouldExit) {
                        return;
                    }

                    BaseVideoPush videoRecorder = mVideoRecorderWr.get();
                    if (videoRecorder == null) {
                        return;
                    }

                    // 获取音频数据，那这个音频数据从哪里来？音乐播放器里面来，pcm 数据
                    int outputBufferIndex = mAudioCodec.dequeueOutputBuffer(mBufferInfo, 0);

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

                        // 打印一下音频的 AAC 数据
                        byte[] data = new byte[outBuffer.remaining()];
                        outBuffer.get(data, 0, data.length);
                        // 推送音频数据到流媒体
                        mVideoRecorderWr.get().mLivePush.pushAudio(data,data.length);

                        mAudioCodec.releaseOutputBuffer(outputBufferIndex, false);
                        outputBufferIndex = mAudioCodec.dequeueOutputBuffer(mBufferInfo, 0);
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
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        private void requestExit() {
            mShouldExit = true;
        }
    }

    /**
     * 音频的采集线程
     */
    private static final class AudioRecordThread extends Thread {
        private int mMinBufferSize;
        private volatile boolean mShouldExit = false;
        private MediaCodec mAudioCodec;
        private long mAudioPts = 0;
        private AudioRecord mAudioRecord; //录音的类
        private byte[] mAudioData;// 这是 pcm 数据

        public AudioRecordThread(WeakReference<BaseVideoPush> videoRecorderWr) {
            mAudioCodec = videoRecorderWr.get().mAudioCodec;

            mMinBufferSize = AudioRecord.getMinBufferSize(
                    BaseVideoPush.AUDIO_SAMPLE_RATE,
                    AudioFormat.CHANNEL_IN_STEREO,//立体声
                    AudioFormat.ENCODING_PCM_16BIT);

            mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                    BaseVideoPush.AUDIO_SAMPLE_RATE,
                    AudioFormat.CHANNEL_IN_STEREO,//立体声
                    AudioFormat.ENCODING_PCM_16BIT,
                    mMinBufferSize);

            mAudioData = new byte[mMinBufferSize];
        }

        @Override
        public void run() {
            try {
                // 开启录制
                mAudioRecord.startRecording();

                while (true) {
                    if (mShouldExit) {
                        return;
                    }

                    // 不断读取 pcm 数据
                    mAudioRecord.read(mAudioData, 0, mMinBufferSize);

                    // 把数据写入到 mAudioCodec 的 InputBuffer
                    int inputBufferIndex = mAudioCodec.dequeueInputBuffer(0);
                    if (inputBufferIndex >= 0) {
                        ByteBuffer byteBuffer = mAudioCodec.getInputBuffers()[inputBufferIndex];
                        byteBuffer.clear();
                        byteBuffer.put(mAudioData);

                        // pts  44100 * 2 *2
                        mAudioPts += mMinBufferSize * 1000000 / BaseVideoPush.AUDIO_SAMPLE_RATE * BaseVideoPush.AUDIO_CHANNELS * 2;
                        // size 22050*2*2
                        mAudioCodec.queueInputBuffer(inputBufferIndex, 0, mMinBufferSize, mAudioPts, 0);
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
        private WeakReference<BaseVideoPush> mVideoRecorderWr;
        private volatile boolean mShouldExit = false;
        private EglHelper mEglHelper;
        private boolean mHashCreateContext = false;
        private boolean mHashSurfaceCreated = false;
        private boolean mHashSurfaceChanged = false;
        private int mWidth;
        private int mHeight;

        public VideoRenderThread(WeakReference<BaseVideoPush> videoRecorderWr) {
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

                    BaseVideoPush videoRecorder = mVideoRecorderWr.get();
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

    private ConnectListener mConnectListener;

    public void setConnectListener(ConnectListener connectListener) {
        this.mConnectListener = connectListener;
    }
}
