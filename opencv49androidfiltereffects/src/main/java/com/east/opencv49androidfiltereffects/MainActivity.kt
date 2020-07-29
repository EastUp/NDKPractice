package com.east.opencv49androidfiltereffects

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        var options = BitmapFactory.Options()
        options.inMutable = true // 是否能修改
        options.inPreferredConfig = Bitmap.Config.RGB_565 // 将图片转为 rgb_565格式
        // RGB_565 747  ARGB_8888 1494  1 倍
        // ARGB_8888 -> RGB 565 RGB 5位R 6位G 5位B   16位
        var src  = BitmapFactory.decodeResource(resources,R.mipmap.copy,options)

        Log.e("TAG","${src.byteCount / 1024}k")


//        var gray = BitmapUtils.gray2(bitmap)
        var result = BitmapUtils.gray3(src)
        image2.setImageBitmap(src)

    }


    companion object{
        init {
            System.loadLibrary("native-lib")
        }
    }
}
