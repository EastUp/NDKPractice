package com.east.opencv63

import android.graphics.Bitmap
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

    }
}
