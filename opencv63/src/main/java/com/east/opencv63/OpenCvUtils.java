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
     *
     * @param bitmap
     * @return
     */
    public static native Bitmap rotation(Bitmap bitmap);

}
