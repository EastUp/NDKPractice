package com.east.ndk02ffmpegvideocompress

import android.Manifest
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.east.permission.PermissionCheckUtils
import com.east.permission.PermissionListener
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        PermissionCheckUtils.checkPermission(this, arrayOf(
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE
        ), object : PermissionListener {
            override fun onGranted() {

            }
            override fun onCancel(){ finish()}
        })

        tv.text = ffmpegInfo()
    }

    external fun stringFromJNI(): String

    external fun ffmpegInfo():String

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }

}
