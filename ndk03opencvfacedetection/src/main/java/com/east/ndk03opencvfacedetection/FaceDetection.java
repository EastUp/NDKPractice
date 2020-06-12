package com.east.ndk03opencvfacedetection;

import android.graphics.Bitmap;

import java.io.File;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 *  @description: 人脸检查工具类
 *  @author: jamin
 *  @date: 2020/6/12
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class FaceDetection {
    static {
        System.loadLibrary("native-lib");
    }

    /**
     *
     * 检查人脸并保存信息
     */
    public native int faceDetectionSaveInfo(Bitmap bitmap);

    /**
     * 加载人脸识别的分类器文件
     * @param filePath
     */
    public native void loadCascade(String filePath);
}
