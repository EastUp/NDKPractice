package com.east.record

import android.Manifest
import android.os.Build
import android.os.Bundle
import android.os.Environment
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import com.east.camera.widget.CameraView
import com.east.ffmpeg88livepush.R
import com.east.permission.PermissionCheckUtils
import com.east.permission.PermissionListener
import com.east.record.widget.RecordProgressButton
import kotlinx.android.synthetic.main.activity_test.*

/**
 * Created by hcDarren on 2019/7/13.
 */
class VideoRecordActivity : AppCompatActivity(), BaseVideoRecorder.RecordListener {
    private var mVideoRecorder: BaseVideoRecorder? = null
    private var permissions = arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE
        ,Manifest.permission.READ_EXTERNAL_STORAGE,Manifest.permission.CAMERA)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        if(PermissionCheckUtils.checkeHasPermission(this,permissions)) {
            setContentView(R.layout.activity_test)
            record_button.setMaxProgress(30000) // 录制30s

            camera_view.setOnFocusListener(object : CameraView.FocusListener {
                override fun beginFocus(x: Int, y: Int) {
                    camera_focus_view.beginFocus(x, y)
                }

                override fun endFocus(success: Boolean) {
                    camera_focus_view.endFocus(success)
                }
            })

            record_button.setOnRecordListener(object : RecordProgressButton.RecordListener {
                override fun onStart() {
                    mVideoRecorder = DefaultVideoRecorder(
                        this@VideoRecordActivity,
                        camera_view.eglContext, camera_view.textureId
                    )
                    mVideoRecorder!!.initVideo(
                        Environment.getExternalStorageDirectory().absolutePath + "/我的资源/搁浅 - 周杰伦.mp3",
                        Environment.getExternalStorageDirectory().absolutePath + "/live_push.mp4",
                        720, 1280
                    )
                    mVideoRecorder!!.setOnRecordListener(this@VideoRecordActivity)
                    mVideoRecorder!!.startRecord()
                }

                override fun onEnd() {
                    mVideoRecorder!!.stopRecord()
                }
            })
        }else{
            PermissionCheckUtils.checkPermission(this,permissions,object :PermissionListener{
                override fun onGranted() {
                    recreate()
                }

                override fun onCancel() {
                    finish()
                }
            })
        }
    }

    override fun onTime(times: Long) {
        record_button!!.setCurrentProgress(times)
    }
}