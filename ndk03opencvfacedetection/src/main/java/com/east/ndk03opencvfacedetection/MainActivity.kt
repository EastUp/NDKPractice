package com.east.ndk03opencvfacedetection

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File
import java.io.FileOutputStream
import java.io.IOException

class MainActivity : AppCompatActivity() {

    private lateinit var mFaceBitmap : Bitmap
    private lateinit var mFaceDetection: FaceDetection
    private lateinit var mCascadeFile: File

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        mFaceBitmap = BitmapFactory.decodeResource(resources,R.drawable.face)
        face_image.setImageBitmap(mFaceBitmap)

        copyCascadeFile()

        mFaceDetection = FaceDetection()
        mFaceDetection.loadCascade(mCascadeFile.absolutePath)

    }

    fun faceDetection(view: View) {
        //识别人脸并保存人脸特征信息
        mFaceDetection.faceDetectionSaveInfo(mFaceBitmap)
        face_image.setImageBitmap(mFaceBitmap)
    }

    private fun copyCascadeFile() {
        try {
            // load cascade file from application resources
            val `is` =
                resources.openRawResource(R.raw.lbpcascade_frontalface)
            val cascadeDir = getDir("cascade", Context.MODE_PRIVATE)
            mCascadeFile = File(cascadeDir, "lbpcascade_frontalface.xml")
            if (mCascadeFile.exists()) return
            val os = FileOutputStream(mCascadeFile)
            val buffer = ByteArray(4096)
            var bytesRead: Int
            while (`is`.read(buffer).also { bytesRead = it } != -1) {
                os.write(buffer, 0, bytesRead)
            }
            `is`.close()
            os.close()
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }
}
