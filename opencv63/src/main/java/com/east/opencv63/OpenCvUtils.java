package com.east.opencv63;

import android.graphics.Bitmap;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 * @description:
 * @author: jamin
 * @date: 2020/9/10
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class OpenCvUtils {
    static {
        System.loadLibrary("native-lib");
    }

    /**
     * 图片旋转 90 度
     */
    public static native Bitmap rotation(Bitmap bitmap);

    /**
     *  图片仿射变换
     */
    public static native Bitmap warpAffine(Bitmap bitmap);


    /**
     * 图片缩放
     */
    public static native Bitmap resize(Bitmap bitmap,int width,int height);

    /**
     *  手写 Remap 重映射
     */
    public static native Bitmap reMap(Bitmap bitmap);

}
