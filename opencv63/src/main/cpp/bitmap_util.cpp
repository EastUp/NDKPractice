//
// Created by 123 on 2020/9/10.
//

#include "bitmap_util.h"
#include "opencv2/opencv.hpp"
#include <android/log.h>

jobject bitmap_util::create_bitmap(JNIEnv *env, int width, int height, int type) {
    // 根据 type 来获取 Config
    // Bitmap.Config.valueOf("ARGB_8888"); 等价于 Enum.valueOf(Bitmap.Config.class, "ARGB_8888");
    char *config_name;
    if(type == CV_8UC4)
        config_name = "ARGB_8888";
    else if(type == CV_8UC2)
        config_name == "RGB_565";

    jstring configName = env->NewStringUTF(config_name);
    jclass bitmap_config_class = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID create_bitmap_config_mid = env->GetStaticMethodID(bitmap_config_class,"valueOf", "(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/Enum;");
    jobject config = env->CallStaticObjectMethod(bitmap_config_class,create_bitmap_config_mid,bitmap_config_class,configName);

    // public static Bitmap createBitmap(int width, int height, @NonNull Config config)
    jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
    jmethodID create_bitmap_mid = env->GetStaticMethodID(bitmap_class,"createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jobject bitmap = env->CallStaticObjectMethod(bitmap_class,create_bitmap_mid,width,height,config);
    return bitmap;
}