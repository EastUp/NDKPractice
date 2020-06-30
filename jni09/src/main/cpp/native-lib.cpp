#include <jni.h>
#include <string>
#include <malloc.h>
#include "com_east_jni09_Parcel.h"
extern "C"

JNIEXPORT jstring JNICALL Java_com_east_jni09_MainActivity_stringFromJNI
(JNIEnv* env,jobject jobj){
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

// 结构体 == C++ 中对象 Parcel.cpp
class Parcel{
    char* mData; // char 1 共享内存的首地址
    int mDataPos = 0; // 必须初始化 结构体没有默认值，除非在 } 后跟上几个默认的子项

public:
    Parcel(){
        mData = static_cast<char *>(malloc(1024));
    }


    void writeInt(jint value) {
        *reinterpret_cast<int*>(mData+mDataPos) = value; // 0 ,4
        mDataPos += sizeof(int);
    }

    void setDataPosition(jint value) {
        mDataPos = value;
    }

    jint readInt() {
        int result = *reinterpret_cast<int*>(mData+mDataPos);
        mDataPos += sizeof(int);
        return result;
    }
};


extern "C" JNIEXPORT jlong JNICALL Java_com_east_jni09_Parcel_nativeCreate
        (JNIEnv * env, jobject jobj){
    Parcel* parcel = new Parcel();
    return reinterpret_cast<jlong>(parcel);
}

extern "C" JNIEXPORT void JNICALL Java_com_east_jni09_Parcel_nativeWriteInt
        (JNIEnv * env, jobject jobj,jlong nativePtr,jint value){
    Parcel* parcel = reinterpret_cast<Parcel*>(nativePtr);
    parcel -> writeInt(value);
}

extern "C" JNIEXPORT void JNICALL Java_com_east_jni09_Parcel_nativeSetDataPosition
        (JNIEnv * env, jobject jobj,jlong nativePtr,jint value){
    Parcel* parcel = reinterpret_cast<Parcel*>(nativePtr);
    parcel -> setDataPosition(value);
}

extern "C"  JNIEXPORT jint JNICALL Java_com_east_jni09_Parcel_nativeReadInt
        (JNIEnv * env, jobject jobj,jlong nativePtr){
    Parcel* parcel = reinterpret_cast<Parcel*>(nativePtr);
    return parcel->readInt();
}

