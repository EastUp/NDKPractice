package com.east.opencv70;

import org.opencv.core.Mat;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 * @description:
 * @author: jamin
 * @date: 2020/9/15
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class FaceDetection {
    static {
        System.loadLibrary("native-lib");
    }

    /**
     *  检测人脸并保存人脸信息
     *
     * @param mat 当前帧
     */
    public void faceDetection(Mat mat){
        faceDetection(mat.nativeObj);
    }

    /**
     * 加载人脸识别的级联分类器文件 文件为训练样本（.xml lbp haar 特征文件）
     *
     * @param filePath
     */
    public native void loadCascade(String filePath);

    private native void faceDetection(long nativeObj);

}
