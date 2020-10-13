package com.east.ffmpeg86.video.player

import android.Manifest
import android.os.Bundle
import android.os.Environment
import android.util.TypedValue
import android.view.Surface
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.east.ffmpeg86.R
import com.east.permission.PermissionCheckUtils
import com.east.permission.PermissionListener
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File


/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description:
 *  @author: jamin
 *  @date: 2020/10/9
 * |---------------------------------------------------------------------------------------------------------------|
 */
class MainActivity : AppCompatActivity() {

    val mVideoFile by lazy {
        File(Environment.getExternalStorageDirectory(), "我的资源/一禅小和尚.mp4")
    }

    val permissions = arrayOf(
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.READ_EXTERNAL_STORAGE
    )

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
    }

    fun play(view: View) {
        var display = windowManager.defaultDisplay
        var width = display.width
        var height = display.height

        PermissionCheckUtils.checkPermission(this, permissions, object : PermissionListener {
            override fun onGranted() {
                jaminVideoView.play(mVideoFile.absolutePath)
            }

            override fun onCancel() {
                finish()
            }
        })
    }


    fun dp2px(dp: Float) = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP,dp,resources.displayMetrics)
}