#include <jni.h>
#include <string>
#include <android/log.h>
#include "ArrayUtil.cpp"
// 结构体别名
#define TAG "TAG"
// 方法进行 define LOGE(...) -> __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
// 重要一个点
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
// 其他方法的定义 __android_log_print 取了一个别名（参数固定，可变）

using namespace std;

// 冒泡排序：思想：相邻两个数进行比较，如果前面的比后面的打，就进行交换，否则不需要交换
// 能写，时间复杂度，空间复杂度 O(n的平方)
// 能优化，下次写 网上有两种优化方法 (鸡尾酒排序，标志位)
// 第三种思路：遍历的过程中可以记录一下位置，了解思想
void bubbleSort(int arr[],int len){
    for (int i = 0; i < len - 1; ++i) { // 外循环代表循环次数 n - 1 步
        for (int j = 0; j < len - i - 1; ++j) { // 内循环代表一次循环对比的次数 n-1,n-2,n-3,1
            if(arr[j] > arr[j+1]){
                // 交换 一次交换是三次赋值
                swap(arr[j],arr[j+1]);
            }
        }
    }
}

// 选择：思想： 遍历找出最小的位置，最后与第一个位置交换  空间复杂度是：O(1)
void selectSort(int arr[],int len){
    for (int i = 0; i < len - 1; ++i) { // 外循环代表循环次数 n - 1 步
        int min = i;
        for (int j = i+1; j < len; ++j) {  // 内循环代表一次循环对比的次数
            if(arr[min] > arr[j]){
                min = j;
            }
        }
        swap(arr[min],arr[i]);
    }
}

void print(int arr[],int len){
    for (int i = 0; i < len; ++i) {
        // 这个方法比较复杂
        LOGE("%d",arr[i]);
    }
}


extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure28bubbkeselectsort_MainActivity_stringFromJNI
        (JNIEnv *env, jobject jobj) {

    // 测试，取时间，两个算法
    int len = 20000;
    int *arr = ArrayUtil::create_random_array(len,20,100000);
    int *arr1 = ArrayUtil::copy_random_array(arr,len);
    ArrayUtil::sort_array("bubbleSort",bubbleSort,arr,len); // 3.299840
    ArrayUtil::sort_array("selectSort",selectSort,arr,len); // 0.876889

    // 思考：冒泡的优化，插入排序，希尔排序，怎样去看 log 日志  O(n平方)
    delete[](arr);
    delete[](arr1);
    // 对性能进行测试  看错误日志
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

