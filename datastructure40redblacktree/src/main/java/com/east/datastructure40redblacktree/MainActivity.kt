package com.east.datastructure40redblacktree

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        tv.text = stringFromJNI()
    }

    external fun stringFromJNI():String

    companion object{
        init {
            System.loadLibrary("native-lib")
        }
    }
}
