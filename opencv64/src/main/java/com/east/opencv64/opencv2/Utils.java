package com.east.opencv64.opencv2;

import android.graphics.Bitmap;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 * @description:
 * @author: jamin
 * @date: 2020/9/11
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class Utils {

    public static void bitmap2mat(Bitmap bitmap,Mat mat){
        nbitmap2mat(bitmap,mat.mNativePtr);
    }

    public static void mat2Bitmap(Mat mat,Bitmap bitmap){
        nmat2bitmap(mat.mNativePtr,bitmap);
    }

    private native static void nbitmap2mat(Bitmap bitmap, long nativePtr);
    private native static void nmat2bitmap( long nativePtr,Bitmap bitmap);

}
