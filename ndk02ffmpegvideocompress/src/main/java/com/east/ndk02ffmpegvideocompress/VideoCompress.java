package com.east.ndk02ffmpegvideocompress;

/**
 * Created by hcDarren on 2018/1/27.
 */

public class VideoCompress {
    // 加载 so 库文件
    static {
        System.loadLibrary("native-lib");
        // 不需要全部 load 相当于 android 调用其他方法类型，不需要全部 load
    }
    // native ffmpeg 压缩视频
    public native void compressVideo(String[] compressCommand,CompressCallback callback);

    public interface CompressCallback{
        public void onCompress(int current, int total);
    }
}
