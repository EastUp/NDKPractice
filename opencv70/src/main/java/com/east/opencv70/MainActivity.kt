package com.east.opencv70

import android.Manifest
import android.content.Context
import android.content.pm.ActivityInfo
import android.content.pm.PackageManager
import android.hardware.Camera
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.east.permission.PermissionCheckUtils
import com.east.permission.PermissionListener
import kotlinx.android.synthetic.main.activity_main.*
import org.opencv.android.CameraBridgeViewBase
import org.opencv.core.Core
import org.opencv.core.Mat
import java.io.File
import java.io.FileOutputStream


class MainActivity : AppCompatActivity(), CameraBridgeViewBase.CvCameraViewListener {
    private lateinit var mFaceDetection: FaceDetection
    private lateinit var mCascadeFile: File
    private var permission = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        permission = PermissionCheckUtils.checkeHasPermission(this,Manifest.permission.CAMERA)
        if (permission) {
            Log.e("TAG", "有这个权限")
            cameraView.cameraIndex = Camera.CameraInfo.CAMERA_FACING_FRONT //前置摄像头
            cameraView.setCvCameraViewListener(this@MainActivity)
            copyCascadeFile()
            mFaceDetection = FaceDetection()
            mFaceDetection.loadCascade(mCascadeFile.absolutePath)
        } else {
            Log.e("TAG", "没有这个权限")
            PermissionCheckUtils.checkPermission(
                this,
                arrayOf(Manifest.permission.CAMERA),
                object : PermissionListener {
                    override fun onGranted() {
                        recreate()
                    }

                    override fun onCancel() {
                        finish()
                    }
                })
        }
    }

    private fun copyCascadeFile() {
        try {
            var isStream = resources.openRawResource(R.raw.haarcascade_frontalface_default)
            var cascadeDir = getDir("cascade", Context.MODE_PRIVATE)
            mCascadeFile = File(cascadeDir, "haarcascade_frontalface_default.xml")
            if (mCascadeFile.exists()) return
            var os = FileOutputStream(mCascadeFile)

            var buffer = ByteArray(4096)
            var byteRead = -1
            while (true) {
                byteRead = isStream.read(buffer)
                if (byteRead != -1)
                    os.write(buffer, 0, byteRead)
                else
                    break
            }
            isStream.close()
            os.close()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    override fun onResume() {
        super.onResume()
        if (permission)
            cameraView.enableView()
    }

    override fun onPause() {
        super.onPause()
        if (permission)
            cameraView.disableView()
    }

    override fun onCameraViewStarted(width: Int, height: Int) {
    }

    override fun onCameraViewStopped() {

    }

    override fun onCameraFrame(inputFrame: Mat): Mat {
        if (resources.configuration.orientation
            == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
            && cameraView.cameraIndex == Camera.CameraInfo.CAMERA_FACING_FRONT
        ) {
            Core.rotate(inputFrame, inputFrame, Core.ROTATE_90_COUNTERCLOCKWISE)
        } else if (resources.configuration.orientation
            == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
            && cameraView.cameraIndex == Camera.CameraInfo.CAMERA_FACING_BACK
        ) {
            Core.rotate(inputFrame, inputFrame, Core.ROTATE_90_CLOCKWISE)
        }
        mFaceDetection.faceDetection(inputFrame)
        return inputFrame
    }

}
