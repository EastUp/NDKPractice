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

/**
 *  获取树的深度
 */
template <class T>
int getDepthTree(TreeNode<T> *pNode){
    if(!pNode)
        return 0;

    int left = getDepthTree(pNode->left);
    int right = getDepthTree(pNode->right);

    return max(left,right) + 1; // 需要加上自己
}

/**
 *  判断一棵树是否是平衡二叉树
 */
template <class T>
bool isBalanceTree(TreeNode<T> *pNode){
    // 可以是一棵空树，左右子树的高度差不会超过 1 ，并且左右两棵子树都是一棵平衡二叉树
    if(!pNode)
        return true;

    // 左右子树的高度差不会超过 1
    int left = getDepthTree(pNode->left);
    int right = getDepthTree(pNode->right);

    // 并且左右两棵子树都是一棵平衡二叉树
    return abs(left-right) <= 1 && isBalanceTree(pNode->left) && isBalanceTree(pNode->right);
}


/**
 *  前序遍历 (必须写成模板才会通用) (方法的回调) 返回值void(*visit)(T)
 * @tparam T
 * @param pNode
 * @param visit
 */
template <class T>
void preOrderTraverse(TreeNode<T> *pNode,void(*visit)(T)){
    if(!pNode)
        return;

    // 先根节点
    visit(pNode->data);
    // 再左节点
    preOrderTraverse(pNode->left,visit);
    // 再右节点
    preOrderTraverse(pNode->right,visit);

}


/**
 *  中序遍历
 * @tparam T
 * @param pNode
 * @param visit
 */
template <class T>
void infixOrderTraverse(TreeNode<T> *pNode,void(*visit)(T)){
    if(!pNode)
        return;

    // 先左节点
    infixOrderTraverse(pNode->left,visit);
    // 再根节点
    visit(pNode->data);
    // 再右节点
    infixOrderTraverse(pNode->right,visit);

}

/**
 *  后序遍历
 * @tparam T
 * @param pNode
 * @param visit
 */
template <class T>
void afterOrderTraverse(TreeNode<T> *pNode,void(*visit)(T)){
    if(!pNode)
        return;

    // 先左节点
    afterOrderTraverse(pNode->left,visit);
    // 再右节点
    afterOrderTraverse(pNode->right,visit);
    // 再根节点
    visit(pNode->data);
}

/**
 *  层序遍历
 * @tparam T
 * @param pNode
 * @param visit
 */
template <class T>
void levelOrderTraverse(TreeNode<T> *pNode,void(*visit)(T)){
    if(!pNode)
        return;

    queue<TreeNode<T>*> queue;
    queue.push(pNode);
    while(!queue.empty()){
        TreeNode<T> *front = queue.front();
        queue.pop();
        visit(front->data);

        if(front->left)
            queue.push(front->left);

        if(front->right)
            queue.push(front->right);
    }

}



void visit(char data){
    LOGE("%c",data);
}



extern "C"
JNIEXPORT jstring JNICALL Java_com_east_datastructure35binarytree_MainActivity_stringFromJNI
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

//    LOGE("前序遍历");
//    preOrderTraverse(A,visit);
//
//    LOGE("中序遍历");
//    infixOrderTraverse(A,visit);
//
//    LOGE("后序遍历");
//    afterOrderTraverse(A,visit);
//
//    LOGE("层序遍历");
//    levelOrderTraverse(A,visit);

    int depth = getDepthTree(A);
    LOGE("树的深度为：%d",depth);

    bool isBalance = isBalanceTree(A);
    LOGE("该树是否为平衡二叉树：%d",isBalance);

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

