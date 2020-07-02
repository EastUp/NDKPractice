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

// 冒泡排序的优化（适用于数组中大部分是排好序的数组）
void optimizeBubbleSort(int arr[], int len){
    // 记录上一次最后遍历的位置
    int n = len;
    int lastchangeIndex = 0; //  最后交换的位置，控制位置
    do{
        lastchangeIndex = 0;
        for (int i = 1; i < n; ++i) {
            if(arr[i-1] > arr[i]){ // 后一个跟前一个对比
                swap(arr[i-1],arr[i]);
                // 记录交换的位置，
                lastchangeIndex = i;
            }
        }
        n = lastchangeIndex; // 记录最后一次交换的位置，证明了后面的不用交换，顺序是对的，可以避免下次循环的判断
    }while (n > 1);
}

// 插入排序 - 前身
/*void insertSort(int arr[],int len){
    for (int i = 1; i < len; ++i) {
        for (int j = i; j >0 && arr[j] < arr[j-1] ; --j) {
            swap(arr[j],arr[j-1]);
        }
    }
}*/

void insertSort(int arr[],int len){
    int j,i;
    for (i = 1; i < len; ++i) {
        // 当前的位置
        int temp = arr[i];
        for (j = i; j >0 && arr[j-1] >temp ; --j) {
            arr[j] = arr[j-1];
        }
        // 插入合适的位置
        arr[j] = temp;
    }
}

// 希尔排序思想：对插入排序分组
void shellInsertSort(int arr[],int len){ // 8
    // 思考 ： 求算法的复杂度
    int increment = len / 2; // 4组
    int i, j ,k;
    while(increment >= 1){
        // 希尔排序
        for(i = 0; i < increment; ++i){ // i = 0, increment = 2
            for(j = i + increment; j < len; j += increment){
                int tmp = arr[j]; // 5
                // k = j = 6;
                for(k = j; k > 0 && arr[k - increment] > tmp; k -= increment){
                    // 往后挪动
                    arr[k] = arr[k - increment];
                }
                // k是有问题的
                LOGE("temp = %d,%d,%d,%d",tmp , k, j ,increment);
                arr[k] = tmp;
                // k = 5;
                // print_array(arr,len);
                // LOGE("---------------");
            }
        }
        increment /= 2;
    }

}


extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure28bubbleselectsort_MainActivity_stringFromJNI
        (JNIEnv *env, jobject jobj) {

    // 测试，取时间，两个算法
    int len = 10;
    int *arr = ArrayUtil::create_random_array(len,20,100000);
    // 创建的时接近排好序的数据
//    int *arr = ArrayUtil::create_nearly_ordered_array(len,20);
    int *arr1 = ArrayUtil::copy_random_array(arr,len);
    int *arr2 = ArrayUtil::copy_random_array(arr,len);
    int *arr3 = ArrayUtil::copy_random_array(arr,len);
    // ArrayUtil::sort_array("optimizeBubbleSort",optimizeBubbleSort,arr2,len); // 如果很多有序的话会提前终止循环
    // ArrayUtil::sort_array("bubbleSort",bubbleSort,arr,len); // 3.299840
    ArrayUtil::sort_array("selectSort",selectSort,arr1,len); // 0.876889 O(n2)
    ArrayUtil::sort_array("insertSort",insertSort,arr3,len); //

    print(arr3,len);

    // 思考：冒泡的优化，插入排序，希尔排序，怎样去看 log 日志  O(n平方)
    delete[](arr);
    delete[](arr1);
    delete[](arr2);
    // 对性能进行测试  看错误日志
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

