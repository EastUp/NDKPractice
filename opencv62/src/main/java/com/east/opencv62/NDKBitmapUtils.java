package com.east.opencv62;

import android.graphics.Bitmap;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 *  @description: NDK 层处理一些 Bitmap 效果
 *  @author: jamin
 *  @date: 2020/9/9
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class NDKBitmapUtils {
    static {
        System.loadLibrary("native-lib");
    }

    /**
     * 实现逆世界效果
     *
     * @param bitmap 原图像
     * @return 逆世界
     */
    public static native Bitmap againstWorld(Bitmap bitmap);

    /**
     * 浮雕效果
     *
     * @param bitmap 原图像
     * @return 浮雕效果图像
     */
    public static native Bitmap anaglyph(Bitmap bitmap);

    /**
     * 实现马赛克效果
     *
     * @param bitmap 原图像
     * @return 马赛克图片
     */
    public static native Bitmap mosaic(Bitmap bitmap);

    /**
     * 实现图片毛玻璃效果
     *
     * @param bitmap 原图像
     * @return 毛玻璃效果
     */
    public static native Bitmap groundGlass(Bitmap bitmap);

    /**
     * 实现图像油画效果
     *
     * @param bitmap 原图像
     * @return  油画效果图像
     */
    public static native Bitmap oilPainting(Bitmap bitmap);


    /**
     * 灰度图像处理效果
     *
     * @param bitmap 原图像
     * @return 优化后的灰度图像
     */
    public static native Bitmap grayOptimize(Bitmap bitmap);
}
