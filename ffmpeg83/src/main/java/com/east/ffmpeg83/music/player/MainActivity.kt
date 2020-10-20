package com.east.ffmpeg83.music.player

import android.Manifest
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.east.ffmpeg83.R
import com.east.ffmpeg83.media.JaminPlayer
import com.east.ffmpeg83.media.listener.MediaErrorListener
import com.east.ffmpeg83.media.listener.MediaInfoListener
import com.east.ffmpeg83.media.listener.MediaPreparedListener
import com.east.permission.PermissionCheckUtils
import com.east.permission.PermissionListener
import java.io.File

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description:
 *  @author: jamin
 *  @date: 2020/10/9
 * |---------------------------------------------------------------------------------------------------------------|
 */
class MainActivity : AppCompatActivity() {

    val mMusicFile by lazy {
        File(Environment.getExternalStorageDirectory(), "我的资源/搁浅 - 周杰伦.mp3")
    }

    val mPlayer by lazy {
        JaminPlayer()
    }

    val permissions = arrayOf(
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.READ_EXTERNAL_STORAGE
    )

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        if(PermissionCheckUtils.checkeHasPermission(this, permissions)){
            Log.e("TAG", mMusicFile.absolutePath)
            mPlayer.setDataSource(mMusicFile.absolutePath)

            mPlayer.setOnErrorListener(object : MediaErrorListener {
                override fun onError(code: Int, msg: String) {
                    Log.e("TAG", "error code: $code")
                    Log.e("TAG", "error msg: $msg")
                }
            })

            mPlayer.setOnPreParedListener(object : MediaPreparedListener {
                override fun onPrepared() {
                    Log.e("TAG", "准备完毕")
                    mPlayer.play()
                }
            })

            mPlayer.setOnInfoListener(object :MediaInfoListener{
                override fun musicInfo(sampleRate: Int, channels: Int) {
                    Log.e("TAG", "采样率：$sampleRate，通道：$channels")
                }

                override fun callbackPcm(pcmData: ByteArray, size: Int) {
                    Log.e("TAG", "pcm数据：$pcmData\n，大小：$size")
                }

            })

        }else{
            PermissionCheckUtils.checkPermission(this, permissions, object : PermissionListener {
                override fun onGranted() {
                    recreate()
                }

                override fun onCancel() {
                    finish()
                }
            })
        }
    }

    fun onStart(view: View) {
        mPlayer.prepareAsync()
    }
    fun onStop(view: View) {
        mPlayer.stop()
    }
}