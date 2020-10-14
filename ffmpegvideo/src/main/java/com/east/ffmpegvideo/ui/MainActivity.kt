package com.east.ffmpegvideo.ui

import android.Manifest
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.View
import android.widget.SeekBar
import android.widget.SeekBar.OnSeekBarChangeListener
import androidx.appcompat.app.AppCompatActivity
import com.east.ffmpegvideo.R
import com.east.ffmpegvideo.media.DarrenPlayer
import com.east.permission.PermissionCheckUtils
import com.east.permission.PermissionListener
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File

class MainActivity : AppCompatActivity() {
    private var permissions = arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE)
    private var mPlayer: DarrenPlayer? = null
    private val mVideoFile = File(Environment.getExternalStorageDirectory(), "我的资源/一禅小和尚.mp4")
    var url =
        "http://v1-default.bytecdn.cn/86c7ca6fa771c8968b1a72fd7a90f1a5/5d15bc0a/video/m/2209a3e637ccbe24258a3725e0a30134e5911619251f00004a66592c6771/?rc=ajZvdjgzOmd2azMzZTczM0ApQHRAbzY7NDo4MzgzMzc4NDUzNDVvQGg1dilAZzN3KUBmM3UpZHNyZ3lrdXJneXJseHdmNzZAcG0vYG9yLzJxXy0tMS0vc3MtbyNvIzAvMy0xLy4uLjYwNTM2LTojbyM6YS1vIzpgLXAjOmB2aVxiZitgXmJmK15xbDojMy5e&vfrom=xgplayer"
    private var mTotalTime = 0
    private var mProgress = 0
    private var seeking = false
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        if(PermissionCheckUtils.checkeHasPermission(this,permissions)){
            mPlayer = DarrenPlayer()
            mPlayer!!.setVideoView(video_view)
            mPlayer!!.setDataSource(mVideoFile.absolutePath)
            mPlayer!!.setOnPreparedListener { mPlayer!!.start() }
            mPlayer!!.prepareAsync()
            mPlayer!!.setOnProgressListener { current, total ->
                mTotalTime = total
                if (!seeking) {
                    seek_bar.progress = current * 100 / total
                }
                Log.e("TAG", "$current/$total")
            }
            seek_bar.setOnSeekBarChangeListener(object : OnSeekBarChangeListener {
                override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                    mProgress = progress * mTotalTime / 100
                }

                override fun onStartTrackingTouch(seekBar: SeekBar) {
                    seeking = true
                }

                override fun onStopTrackingTouch(seekBar: SeekBar) {
                    mPlayer!!.seekTo(mProgress)
                    seeking = false
                    // Log.e("TAG", mProgress+"/"+mProgress);
                }
            })
        }else{
            PermissionCheckUtils.checkPermission(this,permissions,object : PermissionListener {
                override fun onGranted() {
                    recreate()
                }

                override fun onCancel() {
                    finish()
                }
            })
        }
    }

    fun pause(view: View?) {
        mPlayer!!.pause()
    }

    fun resume(view: View?) {
        mPlayer!!.resume()
    }

    fun seek(view: View?) {
        mPlayer!!.seekTo(30)
    }

    companion object {
        private const val TAG = "MainActivity"
    }
}