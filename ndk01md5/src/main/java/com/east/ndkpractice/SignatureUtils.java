package com.east.ndkpractice;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 *  @description:
 *  @author: jamin
 *  @date: 2020/6/9
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class SignatureUtils {

    static {
        System.loadLibrary("native-lib");
    }


    public static native String signatureParams(String params);

}
