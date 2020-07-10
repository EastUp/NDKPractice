#include <jni.h>
#include <string>
#include <android/log.h>
#include "PriorityQueue.hpp"

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


/**
 * 序列化
 */
void serializeTree(TreeNode<char> *pNode, string &str) {
    if(!pNode){
        str.append("#");
        return;
    }

    // 先添加根节点
    str.append(string(1,pNode->data));

    // 再左节点
    serializeTree(pNode->left,str);

    // 再右节点
    serializeTree(pNode->right,str);

}

/**
 * 反序列化 (ABD##E##C#F##)
 * 注意：必须要传 2 级指针，如果是一级指针这只是传过来的数组，++的时候是不会对数组有影响的
 */
TreeNode<char> *deserializeTree(char **str) {
    if(**str == '#'){
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

    // 反序列化
    char *treeStr = "ABD##E##C#F##";
    TreeNode<char> *node =  deserializeTree(&treeStr); // 这是一级指针，需要传二级指针 里面的 ++ 才有用

    // 序列化
    string str;
    serializeTree(node,str);
    LOGE("%s",str.c_str());

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

