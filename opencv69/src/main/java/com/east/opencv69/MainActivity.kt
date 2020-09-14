package com.east.opencv69

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        btnHOG.setOnClickListener {
            image_src.setImageResource(R.drawable.peoples)
            var bitmap = BitmapFactory.decodeResource(resources,R.drawable.peoples)
            image_dst.setImageBitmap(hogBitmap(bitmap))
        }

        btnLBP.setOnClickListener {
            image_src.setImageResource(R.drawable.lena)
            var bitmap = BitmapFactory.decodeResource(resources,R.drawable.lena)
            image_dst.setImageBitmap(lbpBitmap(bitmap))
        }

    }

    // hog 特征人脸检测
    private external fun hogBitmap(bitmap: Bitmap):Bitmap

    // lbg 特征人脸检测
    private external fun lbpBitmap(bitmap: Bitmap):Bitmap

    companion object{
        init {
            System.loadLibrary("native-lib")
        }
    }

}
