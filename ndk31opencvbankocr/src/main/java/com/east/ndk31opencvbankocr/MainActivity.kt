package com.east.ndk31opencvbankocr

import android.Manifest
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.View
import com.east.permission.PermissionCheckUtils
import com.east.permission.PermissionListener
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    lateinit var cardBitmap: Bitmap
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        cardBitmap = BitmapFactory.decodeResource(resources,R.drawable.card_n)
        card_iv.setImageBitmap(cardBitmap)

        PermissionCheckUtils.checkPermission(this,
        arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE,Manifest.permission.READ_EXTERNAL_STORAGE),
        object :PermissionListener{
            override fun onCancel() {
                finish()
            }
        })
    }

    fun cardOcr(view: View) {
        val carNumber = BankCardOcr.carOcr(cardBitmap)
        card_number.text = carNumber
    }
}
