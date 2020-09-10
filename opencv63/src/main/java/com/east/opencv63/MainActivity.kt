package com.east.opencv63

import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        btnRotation.setOnClickListener {
            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.mx)
            var resBitmap = OpenCvUtils.rotation(bitmap)
            resImage.setImageBitmap(resBitmap)
        }

        btnWarpAffine.setOnClickListener {
            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.mx)
            var resBitmap = OpenCvUtils.warpAffine(bitmap)
            resImage.setImageBitmap(resBitmap)
        }

        btnResize.setOnClickListener {
            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.mx)
            var resBitmap = OpenCvUtils.resize(bitmap,bitmap.width * 2,bitmap.height * 2)
            resImage.setImageBitmap(resBitmap)
        }

        btnRemap.setOnClickListener {
            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.mx)
            var resBitmap = OpenCvUtils.reMap(bitmap)
            resImage.setImageBitmap(resBitmap)
        }

    }
}
