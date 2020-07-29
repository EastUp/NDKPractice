#include <jni.h>
#include <string>
#include <android/bitmap.h>

#define TAG "TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;


extern "C"
JNIEXPORT jint JNICALL
Java_com_east_opencv49androidfiltereffects_BitmapUtils_gray3(JNIEnv *env, jclass clazz,
                                                             jobject src) {

    AndroidBitmapInfo info;

    int result = AndroidBitmap_getInfo(env,src,&info);
    if(result!=0)
        return -1;

    void *pixels;
    // 锁定画布
    AndroidBitmap_lockPixels(env,src,&pixels);

    if(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888){
        uint32_t *pixels_p = reinterpret_cast<uint32_t*>(pixels);
        for (int i = 0; i < info.width*info.height; ++i) {
            if(i != 0)
                pixels_p+=1;

            int a = *pixels_p >> 24 & 0xFF;
            int r = *pixels_p >> 16 & 0xFF;
            int g = *pixels_p >> 8 & 0xFF;
            int b = *pixels_p & 0xFF;

            int gray = (int) (0.213f * r + 0.715f * g + 0.072f * b);

            *pixels_p = a<<24 | gray << 16 | gray << 8 | gray;
        }
    }else if(info.format == ANDROID_BITMAP_FORMAT_RGB_565){
        uint16_t *pixels_p = reinterpret_cast<uint16_t*>(pixels);
        for (int i = 0; i < info.width*info.height; ++i) {
            if(i != 0)
                pixels_p+=1;
            // 8888 -> 565
            int r = *pixels_p >> 11 << 3 & 0xFF;
            int g = *pixels_p >> 5 << 2 & 0xFF;
            int b = *pixels_p << 3 & 0xFF;

            // f = 0.213f * r + 0.715f * g + 0.072f * b  (32位来讲的  255,255,255),所以需要补齐8位
            int gray = (int) (0.213f * r + 0.715f * g + 0.072f * b);// 8位

            *pixels_p = gray >>3 << 11 | gray >>2 <<5 | gray >> 3;
        }
    }

    // 解锁画布
    AndroidBitmap_unlockPixels(env,src);

    return 1;
}
