package com.east.ndkpractice;

import android.content.Context;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 *  @description: 参数工具类
 *  @author: jamin
 *  @date: 2020/6/9
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class SignatureUtils {

    static {
        System.loadLibrary("native-lib");
    }


    /**
     * 参数md5加密
     * @param params
     * @return
     */
    public static native String params2Md5(String params);

    /**
     * 校验签名 ，只允许自己 App 可以使用这个 so
     * @param context
     */
    public static native void signatureVerify(Context context);

}
