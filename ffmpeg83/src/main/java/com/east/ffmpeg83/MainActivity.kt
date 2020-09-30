package com.east.ffmpeg83

import android.Manifest
import android.os.Bundle
import android.os.Environment
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.east.ffmpeg83.media.DarrenPlayer
import com.east.permission.PermissionCheckUtils
import com.east.permission.PermissionListener
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File
import java.util.*

class MainActivity : AppCompatActivity(), View.OnClickListener, MusicProgressBar.ProgressListener {
    private var permissions = arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE,Manifest.permission.READ_EXTERNAL_STORAGE)
    private var mPlayer: DarrenPlayer? = null
    private val musicRootFile = File(Environment.getExternalStorageDirectory(), "我的资源")
    private val mMusicItems: MutableList<MusicItem> = ArrayList()
    private var mMusicAdapter: MusicAdapter? = null
    private var mCurrentItem: MusicItem? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val haspermission = PermissionCheckUtils.checkeHasPermission(this,permissions)
        if(!haspermission){
            PermissionCheckUtils.checkPermission(this,permissions,object :PermissionListener{
                override fun onGranted() {
                    recreate()
                }
                override fun onCancel() {
                    finish()
                }
            })
        }else{
            music_pb.setOnProgressListener(this)
            video_play_iv.setOnClickListener(this)
            video_last_iv.setOnClickListener(this)
            video_next_iv.setOnClickListener(this)
            video_next_15s_iv.setOnClickListener(this)
            video_last_15s_iv.setOnClickListener(this)
            val files = musicRootFile.listFiles()
            for (file in files) {
                val musicItem = MusicItem()
                musicItem.fileName = file.name
                musicItem.filePath = file.absolutePath
                mMusicItems.add(musicItem)
            }
            mMusicAdapter = MusicAdapter(mMusicItems, this)
            music_lv.adapter = mMusicAdapter
            mPlayer = DarrenPlayer()
            mMusicAdapter!!.setOnItemClickListener { item -> playMusic(item) }
            mPlayer!!.setOnPreparedListener { mPlayer!!.start() }
            mPlayer!!.setOnProgressListener { current, total -> // 显示进度
                music_pb.setProgress(current, total)
            }
            mPlayer!!.setOnCompleteListener { runOnUiThread { video_next_iv.performClick() } }
        }
    }

    private fun playMusic(item: MusicItem?) {
        mCurrentItem = item
        mPlayer!!.stop()
        mPlayer!!.setDataSource(mCurrentItem!!.filePath)
        mPlayer!!.prepareAsync()
        refreshPlayStatus(true)
    }

    private fun refreshPlayStatus(isPlay: Boolean) {
        if (isPlay) {
            video_play_iv!!.setImageResource(R.drawable.drawable_selector_video_pause)
        } else {
            video_play_iv!!.setImageResource(R.drawable.drawable_selector_video_play)
        }
    }

    override fun onClick(v: View) {
        when (v.id) {
            R.id.video_play_iv -> {
                if (mCurrentItem == null) {
                    mCurrentItem = mMusicItems[0]
                    playMusic(mCurrentItem)
                    mMusicAdapter!!.showCurrent(mCurrentItem)
                    return
                }
                val isPlaying = mPlayer!!.isPlaying
                if (isPlaying) {
                    mPlayer!!.pause()
                } else {
                    mPlayer!!.resume()
                }
                refreshPlayStatus(!isPlaying)
            }
            R.id.video_next_iv -> {
                var next = mMusicItems.indexOf(mCurrentItem) + 1
                if (next >= mMusicItems.size) {
                    next = 0
                }
                mCurrentItem = mMusicItems[next]
                playMusic(mCurrentItem)
                mMusicAdapter!!.showCurrent(mCurrentItem)
            }
            R.id.video_last_iv -> {
                var last = mMusicItems.indexOf(mCurrentItem) - 1
                if (last < 0) {
                    last = mMusicItems.size - 1
                }
                mCurrentItem = mMusicItems[last]
                playMusic(mCurrentItem)
                mMusicAdapter!!.showCurrent(mCurrentItem)
            }
            R.id.video_last_15s_iv -> music_pb!!.last(15)
            R.id.video_next_15s_iv -> music_pb!!.next(15)
        }
    }

    override fun onProgress(progress: Int) {
        mPlayer!!.seekTo(progress)
    }

    companion object {
        private const val TAG = "MainActivity"
    }
}