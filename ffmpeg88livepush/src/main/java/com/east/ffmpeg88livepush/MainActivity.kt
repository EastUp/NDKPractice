package com.east.ffmpeg88livepush

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    private lateinit var mLivePush :LivePush

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        mLivePush = LivePush("rtmp://192.168.1.20/myapp/mystream")

        mLivePush.setConnectListener(object : ConnectListener {
            override fun onConnectError(code: Int, msg: String) {
                Log.e("TAG", "errorCode:$code")
                Log.e("TAG", "errorMsg:$msg")
            }

            override fun onConnectSuccesss() {
                Log.e("TAG", "connect success 可以推流")
            }
        })

        mLivePush.initConnect();
    }

    override fun onDestroy() {
        super.onDestroy()
        mLivePush.stop()
    }
}