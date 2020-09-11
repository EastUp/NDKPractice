package com.east.opencv64

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.east.opencv64.opencv2.CVTYPE
import com.east.opencv64.opencv2.Imgproc
import com.east.opencv64.opencv2.Mat
import com.east.opencv64.opencv2.Utils
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        btnYanmo.setOnClickListener {
            var src = Mat()
            var dst = Mat()
            var kernel = Mat(3,3,CVTYPE.CV_32FC1)
            kernel.put(0,0,0f)
            kernel.put(0,1,-1f)
            kernel.put(0,2,0f)
            kernel.put(1,0,-1f)
            kernel.put(1,1,5f)
            kernel.put(1,2,-1f)
            kernel.put(2,0,0f)
            kernel.put(2,1,-1f)
            kernel.put(2,2,0f)
            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.copy)
            Utils.bitmap2mat(bitmap,src)
            Imgproc.filter2D(src,dst,kernel)
            Utils.mat2Bitmap(dst,bitmap)
            resImage.setImageBitmap(bitmap)
//            resImage.setImageBitmap(yanmo(bitmap))
        }

        btnBlur.setOnClickListener {
            var src = Mat()
            var dst = Mat()
            var size = 15
            var kernel = Mat(size,size,CVTYPE.CV_32FC1)
            for (i in 0 until size){
                for (j in 0 until size){
                    kernel.put(i,j,1f/(size*size))
                }
            }


            var bitmap = BitmapFactory.decodeResource(resources,R.mipmap.copy)
            Utils.bitmap2mat(bitmap,src)
            Imgproc.filter2D(src,dst,kernel)
            Utils.mat2Bitmap(dst,bitmap)
            resImage.setImageBitmap(bitmap)
//            resImage.setImageBitmap(blur(bitmap))
        }
    }

    companion object{
        init {
            System.loadLibrary("native-lib")
        }
    }

    external fun yanmo(bitmap:Bitmap): Bitmap
    external fun blur(bitmap:Bitmap): Bitmap

}
