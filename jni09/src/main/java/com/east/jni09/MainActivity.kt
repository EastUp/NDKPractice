package com.east.jni09

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        tv.text = stringFromJNI()

        var parcel = Parcel()
        parcel.writeInt(12)
        parcel.writeInt(24)

        parcel.setDataPosition(0)

        val number1 = parcel.readInt()
        val number2 = parcel.readInt()

        Log.e("TAG","number1 = $number1 , number2 = $number2")

    }

    external fun stringFromJNI():String

    companion object{
        init {
            System.loadLibrary("native-lib")
        }
    }

}
