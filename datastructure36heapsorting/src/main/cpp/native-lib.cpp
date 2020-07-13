#include <jni.h>
#include <string>
#include <android/log.h>
#include "PriorityQueue.hpp"

#define TAG "TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;

template<class T>
class TreeNode {
public:
    T data = NULL; // 数据
    TreeNode<T> *left = NULL; // 左孩子
    TreeNode<T> *right = NULL; // 右孩子

    TreeNode(T data) : data(data) {

    }
};


/**
 * 序列化
 */
void serializeTree(TreeNode<char> *pNode, string &str) {
    if (!pNode) {
        str.append("#");
        return;
    }

    // 先添加根节点
    str.append(string(1, pNode->data));

    // 再左节点
    serializeTree(pNode->left, str);

    // 再右节点
    serializeTree(pNode->right, str);

}

/**
 * 反序列化 (ABD##E##C#F##)
 * 注意：必须要传 2 级指针，如果是一级指针这只是传过来的数组，++的时候是不会对数组有影响的
 */
TreeNode<char> *deserializeTree(char **str) {
    if (**str == '#') {
        *str += 1;
        return NULL;
    }

    TreeNode<char> *node = new TreeNode<char>(**str);
    *str += 1;

    // 解析左边的
    node->left = deserializeTree(str);
    // 解析右边的
    node->right = deserializeTree(str);

    return node;
}

/**
 * 调整为大根堆
 */
void adjuastHeap(int arr[],int k, int n){
    while (k * 2 + 1 < n){ // 到底的情况
        // 最大指向左孩子
        int max = 2*k +1;
        // 有右孩子且右孩子大于左孩子
        if(max + 1 < n && arr[max + 1] > arr[max]){
            max = max + 1;
        }

        if(arr[k] > arr[max])
            break;

        // 交换最大的网上冒
        swap(arr[k],arr[max]);
        k = max;
    }
}

/**
 *  堆排序
 */
void heapSort(int arr[],int len){
    // 1. 从最后一个不是叶子节点的节点，开始调整为大根堆
    for(int i = len/2-1; i>=0; i--){
        adjuastHeap(arr,i,len);
    }

    // 2. 第一个与最后一个进行交换，最后一个值自然就是最大值了，接着把剩下的再进行大根堆排序
    for (int i = len - 1; i > 0; --i) {
        swap(arr[0],arr[i]);// 将最大的与末尾交换，这时末尾的就是最大值了
        adjuastHeap(arr,0,i); // 对 第 0 个位置进行调整。
    }
}



extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure36heapsorting_MainActivity_stringFromJNI
        (JNIEnv *env, jobject jobj) {

//    TreeNode<char> *A = new TreeNode<char>('A');
//    TreeNode<char> *B = new TreeNode<char>('B');
//    TreeNode<char> *C = new TreeNode<char>('C');
//    TreeNode<char> *D = new TreeNode<char>('D');
//    TreeNode<char> *E = new TreeNode<char>('E');
//    TreeNode<char> *F = new TreeNode<char>('F');
//
//    A->left = B;
//    A->right = C;
//
//    B->left = D;
//    B->right = E;
//
//    C->right = F;
//
//    // 反序列化
//    char *treeStr = "ABD##E##C#F##";
//    TreeNode<char> *node =  deserializeTree(&treeStr); // 这是一级指针，需要传二级指针 里面的 ++ 才有用
//
//    // 序列化
//    string str;
//    serializeTree(node,str);
//    LOGE("%s",str.c_str());

//    PriorityQueue<int> queue(10);
//
//    for (int i = 0; i < 10; ++i) {
//        queue.push(rand() % 100);
//    }
//
//    while (!queue.isEmpty()){
//        int max = queue.pop();
//        LOGE("%d",max);
//    }

    int len = 10;
    int arr[len];
    for (int i = 0; i < len; ++i) {
        arr[i] = rand() % 100;
    }

    time_t start = clock();
    heapSort(arr,len);
    time_t end = clock();

    LOGE("消耗时间为：%lf",((double)(end-start))/CLOCKS_PER_SEC);

    for (int i = 0; i < len; ++i) {
        LOGE("%d",arr[i]);
    }

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

