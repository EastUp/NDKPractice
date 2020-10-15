package com.east.ffmpeg88livepush

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 * @description: rtmp连接服务器的结果回调
 * @author: jamin
 * @date: 2020/10/15
 * |---------------------------------------------------------------------------------------------------------------|
 */
interface ConnectListener{
    fun onConnectError(code:Int,msg:String)

    fun onConnectSuccesss()
}