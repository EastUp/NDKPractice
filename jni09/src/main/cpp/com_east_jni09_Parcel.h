/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_darren_ndk_day13_Simple1 */

#ifndef _Included_com_east_jni09_Parcel
#define _Included_com_east_jni09_Parcel
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_darren_ndk_day13_Simple1
 * Method:    callStaticMethod
 * Signature: ()V
 */
JNIEXPORT jlong JNICALL Java_com_east_jni09_Parcel_nativeCreate
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_com_east_jni09_Parcel_nativeWriteInt
        (JNIEnv *, jobject,jlong,jint);

JNIEXPORT void JNICALL Java_com_east_jni09_Parcel_nativeSetDataPosition
        (JNIEnv *, jobject,jlong,jint);

JNIEXPORT jint JNICALL Java_com_east_jni09_Parcel_nativeReadInt
        (JNIEnv *, jobject,jlong);

// 读一些 String ? 怎么实现

#ifdef __cplusplus
}
#endif
#endif
