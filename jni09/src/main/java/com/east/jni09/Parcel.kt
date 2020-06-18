package com.east.jni09

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description: 仿照系统的 Parcel 使用共享内存进行数据的读取，加快效率
 *  @author: jamin
 *  @date: 2020/6/18
 * |---------------------------------------------------------------------------------------------------------------|
 */
class Parcel {
    private var mNativePtr : Long = 0

    init{
        System.loadLibrary("native-lib")
        mNativePtr = nativeCreate()
    }

    fun writeInt(value:Int){
        nativeWriteInt(mNativePtr,value)
    }

    fun setDataPosition(value:Int){
        nativeSetDataPosition(mNativePtr,value)
    }

    fun readInt():Int{
        return nativeReadInt(mNativePtr)
    }

    // 创建 native 层的 Parcel 对象并把它的内存地址传递到 java 层
    private external fun nativeCreate(): Long

    // 写 int
    private external fun nativeWriteInt(mNativePtr: Long, value: Int)

    // 写完之后重新设置偏移位置
    private external fun nativeSetDataPosition(mNativePtr: Long, value: Int)

    // 读 int
    private external fun nativeReadInt(value: Long):Int

}