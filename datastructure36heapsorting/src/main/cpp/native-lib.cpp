#include <jni.h>
#include <string>
#include <android/log.h>
#include <queue>
#include <cmath>

#define TAG "TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;

template <class T>
class TreeNode{
public:
    T data = NULL; // 数据
    TreeNode<T> *left = NULL; // 左孩子
    TreeNode<T> *right = NULL; // 右孩子

    TreeNode(T data):data(data){

    }
};


extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure36heapsorting_MainActivity_stringFromJNI
(JNIEnv* env,jobject jobj){

    TreeNode<char> *A = new TreeNode<char>('A');
    TreeNode<char> *B = new TreeNode<char>('B');
    TreeNode<char> *C = new TreeNode<char>('C');
    TreeNode<char> *D = new TreeNode<char>('D');
    TreeNode<char> *E = new TreeNode<char>('E');
    TreeNode<char> *F = new TreeNode<char>('F');

    A->left = B;
    A->right = C;

    B->left = D;
    B->right = E;

    C->right = F;

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

