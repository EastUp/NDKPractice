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

void print_array(int *arr, int len){
    for (int i = 0; i < len; ++i) {
        // 这个方法比较复杂
        LOGE("i = %d--->value = %d",i,arr[i]);
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
/*
void insertSort(int arr[],int len){
    for (int i = 1; i < len; ++i) {
        for (int j = i; j >0 && arr[j] < arr[j-1] ; --j) {
            swap(arr[j],arr[j-1]);
        }
    }
}

void insertSort(int arr[],int len){
    for (int i = 1; i < len; ++i) {
        for (int j = i; j >0; --j) {
            if(arr[j] < arr[j-1] )
                swap(arr[j],arr[j-1]);
            else
                break;
        }
    }
}
*/

// 插入排序优化
void insertSort(int arr[],int len){
    int temp,j,i;
    for (i = 1; i < len; ++i) {
        // 当前的位置
        temp = arr[i];
        for (j = i; j >0 && arr[j-1] >temp ; --j) {
            arr[j] = arr[j-1];
        }
        // 插入合适的位置 (这个时候的j 是 -- 了的哦 注意看循环的最后一个条件)
        arr[j] = temp;
    }
}

// 插入排序优化1(这样会跳出循环更快)
void insertSort1(int arr[],int len){
    int temp,j,i;
    for (i = 1; i < len; ++i) {
        // 当前的位置
        temp = arr[i];
        for (j = i; j >0; --j) {
            if(arr[j-1] >temp)
                arr[j] = arr[j-1];
            else
                break;
        }
        // 插入合适的位置 (这个时候的j 是 -- 了的哦 注意看循环的最后一个条件)
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
                for(k = j; k > i  && arr[k - increment] > tmp; k -= increment){
                    // 往后挪动
                    arr[k] = arr[k - increment];
                }
                arr[k] = tmp;
            }
        }
        increment /= 2;
    }
}


void shellInsertSort1(int arr[],int len){ // 8
    // 思考 ： 求算法的复杂度
    int i, j ;
    for(int increment = len / 2;increment >0;increment /= 2){ // 4组
        // 希尔排序
        for(i = increment; i < len; ++i){ // i = 0, increment = 2
            int tmp = arr[i]; // 5
            for(j = i ; j - increment >= 0 && tmp < arr[j - increment]; j -= increment){
                arr[j] = arr[j - increment];
            }
            arr[j] = tmp;
        }
    }
}

// 对数组区间 [l,mid] 和 [mid+1,r] 进行归并
void merge_(int arr[],int l,int mid,int r){
    // 1. 对数组进行一次拷贝
    int temp[r - l + 1];
    for(int i = l ;i <= r; ++i){
        temp[i -l] = arr[i];
    }
    // 2. 确定好分析之后的变量
    int i = l;
    int j = mid + 1;
    int k = l;
    for (; k <= r; ++k) {
        if(i > mid){
            arr[k] = temp[j - l];
            j++;
        }else if( j > r){
            arr[k] = temp[i -l];
            i++;
        }else if(temp[i - l] < temp[j - l]){ // 临时数据里面的 i 位置和 j 位置去比较
            arr[k] = temp[i - l];
            i++;
        } else{
            arr[k] = temp[j - l];
            j++;
        }
    }
}

// 对数组的 [l,r] 区间进行归并排序
void mergeSort_(int arr[], int l, int r){
    // 递归到底的情况
    if(l >= r)
        return;
    int mid = ( l + r) >> 1;
    mergeSort_(arr,l,mid);//左边归并排序，使得左子序列有序
    mergeSort_(arr,mid + 1,r);//右边归并排序，使得右子序列有序
    // 优化要根据具体的场景去做（因为前面是排好序的！！）
    if(arr[mid] > arr[mid + 1]){
        merge_(arr, l, mid, r);//将两个有序子数组合并操作
    }
}

// 归并排序
void mergeSort(int arr[],int len){
    mergeSort_(arr,0,len - 1);
}


// 对数组 arr 区间[l,r] 进行分割操作
int partition_(int arr[], int l, int r) {// 10 , 20
    // 优化，跟区间[l,r]随机位置进行比较
    swap(arr[l], arr[rand() % (r - l + 1) + l]);
    int v = arr[l];
    // 以 p 为分割，[l+1,p]<v 和  [p+1,r] > v
    int p = l;
    for (int i = l; i <= r; ++i) {
        if (arr[i] < v) {
            // 只需要处理小于的情况
            swap(arr[p + 1], arr[i]);
            p++;
        }
    }
    swap(arr[l], arr[p]);
    return p;
}

// 对数组 arr 区间[l,r] 进行快速排序
void quickSort_(int arr[], int l, int r) {
    // 递归到底的情况
    if (l >= r) {
        return;
    }
    int p = partition_(arr, l, r);
    quickSort_(arr, l, p - 1); // 对基准元素左边的元素进行递归排序
    quickSort_(arr, p + 1, r); // 对基准元素右边的进行递归排序
}

// 快速排序
void quickSort(int arr[], int len) {
    srand(time(NULL)); // 初始化随机数发生器
    quickSort_(arr, 0, len - 1);
}

void quickSort3ways_(int arr[], int l, int r) {
    // 递归到底的情况
    if (l >= r) {
        return;
    }

    // 定义变量
    swap(arr[l], arr[rand() % (r - l + 1) + l]);
    int v = arr[l];

    int lt = l;//[l+1, lt] < v
    int gt = r + 1;// [gt,r] >v
    int i = l + 1;// [lt + 1,i) = v

    while (gt > i) {
        if (arr[i] > v) {
            swap(arr[i], arr[gt - 1]);
            gt--;
        } else if (arr[i] < v) {
            swap(arr[i], arr[lt + 1]);
            i++;
            lt++;
        } else {
            i++;
        }
    }

    swap(arr[l], arr[lt]);
    quickSort3ways_(arr, l, lt - 1);
    quickSort3ways_(arr, gt, r);
}

void quickSort3ways(int arr[], int len) {
    srand(time(NULL)); // 初始化随机数发生器
    quickSort3ways_(arr, 0, len - 1);
}


extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure28bubbleselectsort_MainActivity_stringFromJNI
        (JNIEnv *env, jobject jobj) {

    // 测试，取时间，两个算法
    int len = 40000;
    int *arr = ArrayUtil::create_nearly_ordered_array(len,10);
//    int *arr = ArrayUtil::create_random_array(len,2,10000);
    LOGE("------------------1");
    // 创建的时接近排好序的数据
//    int *arr = ArrayUtil::create_nearly_ordered_array(len,20);
    int *arr1 = ArrayUtil::copy_random_array(arr,len);
    int *arr2 = ArrayUtil::copy_random_array(arr,len);
    int *arr3 = ArrayUtil::copy_random_array(arr,len);
    int *arr4 = ArrayUtil::copy_random_array(arr,len);
//    int *arr5 = new int[]{1,-1,-2,-3,7,8};
    // ArrayUtil::sort_array("optimizeBubbleSort",optimizeBubbleSort,arr2,len); // 如果很多有序的话会提前终止循环
    // ArrayUtil::sort_array("bubbleSort",bubbleSort,arr,len); // 3.299840
    // ArrayUtil::sort_array("selectSort",selectSort,arr1,len); // 0.876889 O(n2)
    // ArrayUtil::sort_array("insertSort",insertSort,arr3,len); // 提前终止循环
    // ArrayUtil::sort_array("insertSort1",insertSort1,arr4,len); //
    // 如果对于接近排好序的数据，时间复杂度最优 O(n)，考虑最坏的情况 O(n2)
//    ArrayUtil::sort_array("shellInsertSort",shellInsertSort,arr,len);
//    ArrayUtil::sort_array("shellInsertSort1",shellInsertSort1,arr,len);
    ArrayUtil::sort_array("quickSort",quickSort,arr,len);
    ArrayUtil::sort_array("mergeSort",mergeSort,arr1,len);
    ArrayUtil::sort_array("quickSort3ways",quickSort3ways,arr2,len);
    delete[](arr);
    delete[](arr1);
    delete[](arr2);
    delete[](arr3);
    delete[](arr4);
    // 对性能进行测试  看错误日志
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

