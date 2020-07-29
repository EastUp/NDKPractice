package com.east.opencv49androidfiltereffects;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Paint;

import org.jetbrains.annotations.Nullable;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 * @description: ColorMatrix 使用 和修改像素 转灰度图或黑白图
 * @author: jamin
 * @date: 2020/7/29
 * |---------------------------------------------------------------------------------------------------------------|
 */
class BitmapUtils {

    // 第三种最好的方法
    public static native int gray3(@Nullable Bitmap src);

    // 第一种使用 ColorMatrix 写滤镜
    @Nullable
    public static Bitmap gray(@Nullable Bitmap src) {

        Bitmap dst = Bitmap.createBitmap(src.getWidth(), src.getHeight(), src.getConfig());
        Canvas canvas = new Canvas(dst);

        // 通过 ColorMatrix 改变颜色矩阵
        Paint paint = new Paint();
        paint.setDither(true);
        paint.setAntiAlias(true);

        // ColorMatrix colorMatrix = new ColorMatrix();
        // colorMatrix.setSaturation(0); // 修改饱和度为0 也就是把颜色矩阵转成了灰度图,相当于下面的矩阵

        /*ColorMatrix colorMatrix = new ColorMatrix(new float[]{ // 灰度图效果
                0.213f,0.715f,0.072f,0,0,
                0.213f,0.715f,0.072f,0,0,
                0.213f,0.715f,0.072f,0,0,
                0,0,0,1,0
        });*/

        /*ColorMatrix colorMatrix = new ColorMatrix(new float[]{ // 原图效果
                1,0,0,0,0,
                0,1,0,0,0,
                0,0,1,0,0,
                0,0,0,1,0
        });*/

        /*ColorMatrix colorMatrix = new ColorMatrix(new float[]{ // 底片效果
                -1,0,0,0,255,
                0,-1,0,0,255,
                0,0,-1,0,255,
                0,0,0,1,0
        });*/

        ColorMatrix colorMatrix = new ColorMatrix(new float[]{ // 提高亮度
                1.2f, 0, 0, 0, 0,
                0, 1.2f, 0, 0, 0,
                0, 0, 1.2f, 0, 0,
                0, 0, 0, 1, 0
        });

        paint.setColorFilter(new ColorMatrixColorFilter(colorMatrix)); // 设置颜色矩阵

        canvas.drawBitmap(src, 0, 0, paint);
        return dst;
    }

    // 第二种
    public static Bitmap gray2(@Nullable Bitmap src) {
        // Bitmap dst = Bitmap.createBitmap(src.getWidth(), src.getHeight(), src.getConfig());

        // java层像素操作
        int[] pixels = new int[src.getWidth() * src.getHeight()];

        src.getPixels(pixels, 0, src.getWidth(), 0, 0, src.getWidth(), src.getHeight());

        for (int i = 0; i < pixels.length; i++) {
            int pixel = pixels[i];

            int a = pixel >> 24 & 0xFF;
            int r = pixel >> 16 & 0xFF;
            int g = pixel >> 8 & 0xFF;
            int b = pixel & 0xFF;

            // 灰色
            int gray = (int) (0.213f * r + 0.715f * g + 0.072f * b);
            // pixels[i] = a << 24 | gray << 16 | gray << 8 | gray;


            // 黑白
            int bw = (r + g + b) / 3 > 125 ? 255 : 0;
            pixels[i] = a << 24 | bw << 16 | bw << 8 | bw;
        }

        // dst.setPixels(pixels, 0, src.getWidth(), 0, 0, src.getWidth(), src.getHeight());
        // return dst;

        // 自己设置给自己，能复用不？,需要 BitmapFactory.Options.inMutable = true
        src.setPixels(pixels, 0, src.getWidth(), 0, 0, src.getWidth(), src.getHeight());
        return src;
    }
}
