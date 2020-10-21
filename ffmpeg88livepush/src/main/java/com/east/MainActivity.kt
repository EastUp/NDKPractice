package com.east

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.east.ffmpeg88livepush.LivePushActivity
import com.east.ffmpeg88livepush.R
import com.east.record.VideoRecordActivity
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        btnVideoRecorder.setOnClickListener {
            startActivity(Intent(this,VideoRecordActivity::class.java))
        }

        btnLivePush.setOnClickListener {
            startActivity(Intent(this,LivePushActivity::class.java))
        }
    }
}