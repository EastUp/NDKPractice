package com.east.ffmpeg88livepush

import android.Manifest
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.east.camera.widget.CameraView
import com.east.opengl.Utils
import com.east.permission.PermissionCheckUtils
import com.east.permission.PermissionListener
import kotlinx.android.synthetic.main.activity_livepush.*
import kotlinx.android.synthetic.main.activity_test.camera_focus_view
import kotlinx.android.synthetic.main.activity_test.camera_view

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description:  推流到服务器
 *  @author: jamin
 *  @date: 2020/10/21 11:21
 * |---------------------------------------------------------------------------------------------------------------|
 */
class LivePushActivity : AppCompatActivity() {
//    private lateinit var mLivePush :LivePush
    private lateinit var mVideoPush: DefaultVideoPush
    private var permissions = arrayOf(
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.READ_EXTERNAL_STORAGE,
        Manifest.permission.CAMERA,
        Manifest.permission.RECORD_AUDIO
    )

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        if(PermissionCheckUtils.checkeHasPermission(this, permissions)) {
            setContentView(R.layout.activity_livepush)

            live_bt.setOnClickListener { // 开始推送
                startLivePush()
            }

            camera_view.setOnFocusListener(object : CameraView.FocusListener {
                override fun beginFocus(x: Int, y: Int) {
                    camera_focus_view.beginFocus(x, y)
                }

                override fun endFocus(success: Boolean) {
                    camera_focus_view.endFocus(success)
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

    private fun startLivePush() {
        Log.e("TAG", "开始推流")

        mVideoPush = DefaultVideoPush(
            this@LivePushActivity,
            camera_view.eglContext, camera_view.textureId
        )
        mVideoPush.initVideo(
            "rtmp://192.168.1.20/myapp/mystream",
            /*Utils.getScreenWidth(this@LivePushActivity)*/ 720 /2,
            /*Utils.getScreenHeight(this@LivePushActivity)*/ 1280 / 2
        )


        mVideoPush.setConnectListener(object :ConnectListener{
            override fun onConnectError(code: Int, msg: String) {
                Log.e("TAG", "errorCode:$code")
                Log.e("TAG", "errorMsg:$msg")
            }

            override fun onConnectSuccesss() {
                Log.e("TAG", "connect success 可以推流")
            }
        })

        mVideoPush.startPush()
    }


    override fun onDestroy() {
        super.onDestroy()
        mVideoPush.stopPush()
    }
}