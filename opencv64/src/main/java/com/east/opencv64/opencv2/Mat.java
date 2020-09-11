package com.east.opencv64.opencv2;

import android.util.Log;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 *  @description: Java Mat -> native Mat
 *  @author: jamin
 *  @date: 2020/9/11
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class Mat {
    private int rows;
    private int cols;
    private CVTYPE cvtype;
    public final long mNativePtr; // native 层的 mat(通过首地址来转的)

    public Mat(int rows,int cols,CVTYPE type){
        this.rows = rows;
        this.cols = cols;
        this.cvtype = type;
        // 创建 Mat.cpp 对象 关键下次还能操作
        mNativePtr = nMatIII(rows,cols,type.value);
    }

    public Mat() {
        mNativePtr = nMat();
    }

    public void put(int row, int col, float value) {
        Log.e("tag","Java"+row+"--"+col+"--"+value);
        if(cvtype != CVTYPE.CV_32FC1){
            throw new UnsupportedOperationException("Provider value not supported,Please check CVTYPE");
        }
        nPutF(mNativePtr,row,col,value);
    }

    private native long nMatIII(int rows, int cols, int value);

    private native long nMat();

    private native void nPutF(long nativePtr,int row, int col, float value);
}
