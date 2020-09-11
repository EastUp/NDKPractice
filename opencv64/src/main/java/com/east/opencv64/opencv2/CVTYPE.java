package com.east.opencv64.opencv2;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 * @description: Type 类型，value对应 Mat.cpp 的type类型
 * @author: jamin
 * @date: 2020/9/11
 * |---------------------------------------------------------------------------------------------------------------|
 */
public enum CVTYPE {
    CV_8UC1(0),CV_8UC2(8),CV_8UC4(24),CV_32FC1(5);

    final int value;
    CVTYPE(int value){
        this.value = value;
    }
}
