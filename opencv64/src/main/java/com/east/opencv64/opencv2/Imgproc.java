package com.east.opencv64.opencv2;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 *  @description:
 *  @author: jamin
 *  @date: 2020/9/11
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class Imgproc {

    // 将 native 层的 Filter2D 搬到 java 上
    public static void filter2D(Mat src,Mat dst,Mat kernel){
        nFilter2D(src.mNativePtr,dst.mNativePtr,kernel.mNativePtr);
    }

    private static native void nFilter2D(long srcPtr,long dstPtr,long kernelPtr);
}
