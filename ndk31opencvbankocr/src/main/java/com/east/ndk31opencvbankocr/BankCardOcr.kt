package com.east.ndk31opencvbankocr

import android.graphics.Bitmap

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description: 银行卡识别
 *  @author: jamin
 *  @date: 2020/6/30
 * |---------------------------------------------------------------------------------------------------------------|
 */
object BankCardOcr {
    init {
        System.loadLibrary("native-lib")
    }

    external fun carOcr(bitmap: Bitmap):String
}