package com.east.ndkpractice;

import android.app.Application;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 *  @description:
 *  @author: jamin
 *  @date: 2020/6/9
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class BaseApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();

        SignatureUtils.signatureVerify(this);
    }
}
