//
// Created by 123 on 2020/9/10.
//

#ifndef NDKPRACTICE_BITMAP_UTIL_H
#define NDKPRACTICE_BITMAP_UTIL_H

#include <jni.h>

class bitmap_util {
public:
    static jobject create_bitmap(JNIEnv *env, int width, int height, int type);
};


#endif //NDKPRACTICE_BITMAP_UTIL_H
