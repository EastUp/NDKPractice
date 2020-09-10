package com.east.opencv62

import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)


        // 逆世界
        btnAgainstWorld.setOnClickListener {
            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.mx)
            var resBitmap = NDKBitmapUtils.againstWorld(bitmap)
            resImage.setImageBitmap(resBitmap)
        }

        // 浮雕
        btnAnaglyph.setOnClickListener {
            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.mx)
            var resBitmap = NDKBitmapUtils.anaglyph(bitmap)
            resImage.setImageBitmap(resBitmap)
        }

        // 马赛克
        btnMosaic.setOnClickListener {
            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.mx)
            var resBitmap = NDKBitmapUtils.mosaic(bitmap)
            resImage.setImageBitmap(resBitmap)
        }

        // 毛玻璃效果
        btnGroundGlass.setOnClickListener {
            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.mx)
            var resBitmap = NDKBitmapUtils.groundGlass(bitmap)
            resImage.setImageBitmap(resBitmap)
        }

        // 油画效果
        btnOilPainting.setOnClickListener {
            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.mx)
            Thread(Runnable {
                var resBitmap = NDKBitmapUtils.oilPainting(bitmap)
                runOnUiThread {
                    resImage.setImageBitmap(resBitmap)
                }
            }).start()
        }


        // 灰度图像处理效果
        btnGaryOptimize.setOnClickListener {
            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.mx)
            var resBitmap = NDKBitmapUtils.grayOptimize(bitmap)
            resImage.setImageBitmap(resBitmap)
        }

    }
}
