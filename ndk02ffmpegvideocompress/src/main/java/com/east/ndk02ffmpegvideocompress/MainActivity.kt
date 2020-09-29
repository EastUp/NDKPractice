package com.east.ndk02ffmpegvideocompress

import android.Manifest
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.east.permission.PermissionCheckUtils
import com.east.permission.PermissionListener
import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers
import io.reactivex.rxjava3.core.Observable
import io.reactivex.rxjava3.schedulers.Schedulers
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File


class MainActivity : AppCompatActivity() {
    private val mInFile: File = File(Environment.getExternalStorageDirectory(), "test.mp4")
    private val mOutFile: File = File(Environment.getExternalStorageDirectory(), "out.mp4")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // ffmpeg -i test.mp4 -b:v 1024k out.mp4
        // -b:v 码率是什么？ 码率越高视频越清晰，而且视频越大
        // 1M  1024K
        // test.mp4 需要压缩的视频路径
        // out.mp4 压缩后的路径

        PermissionCheckUtils.checkPermission(this, arrayOf(
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE
        ), object : PermissionListener {
            override fun onGranted() {

            }

            override fun onCancel() {
                finish()
            }
        })

        tv.text = "avutil的版本号为：${stringFromJNI()}"
    }

    fun compressVideo(view: View) {
        compressVideo()
    }

    private fun compressVideo() {
        val compressCommand =
            arrayOf("ffmpeg", "-i", mInFile.absolutePath, "-b:v", "1024k", mOutFile.absolutePath)

        Observable.just(compressCommand).map {
            // 压缩是耗时的，子线程，处理权限
            val videoCompress = VideoCompress()
            try {
                videoCompress.compressVideo(
                    it
                ) { current, total -> Log.e("TAG", "压缩进度：$current/$total") }
            } catch (e: Exception) {
                Log.e("TAG",e.toString())
            }
            mOutFile
        }.subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .subscribe {
                // 压缩完成
                Log.e("TAG", "压缩完成")
            }
    }

    external fun stringFromJNI():String

    companion object{
        init {
            System.loadLibrary("native-lib")
        }
    }

}
