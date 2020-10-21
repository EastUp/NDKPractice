package com.east.ffmpeg83.media.listener

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description: 媒体信息
 *  @author: jamin
 *  @date: 2020/10/9
 * |---------------------------------------------------------------------------------------------------------------|
 */
interface MediaInfoListener {
    fun musicInfo(sampleRate: Int, channels: Int)

    fun callbackPcm(pcmData: ByteArray, size: Int)
}