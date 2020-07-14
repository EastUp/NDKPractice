//
// Created by 123 on 2020/7/14.
//

#ifndef NDKPRACTICE_BST_HO
#define NDKPRACTICE_BST_HO

#include <iostream>

// c++ map multiMap 红黑树
// java Map 和 c++ Map 比较
template<class K, class V>
struct TreeNode {
public:
    TreeNode<K,V> *left = NULL;
    TreeNode<K,V> *right = NULL;
    K key = NULL;
    V value = NULL;

    TreeNode(TreeNode<K,V> *pNode){
        this->left = pNode->left;
        this->right = pNode->right;
        this->key = pNode->key;
        this->value = pNode->value;
    }

    TreeNode(K key, V value):key(key),value(value){
    }

};


// 二叉搜索树
template<class K, class V>
class BST {
    int count = 0; // 总个数
    TreeNode<K,V> *root = NULL; // 根节点

public:
    BST(){}

    ~BST(){
        deleteAllNode(root);
        root = NULL;
    }

    // 新增
    void put(K key,V value){
        root = addNode(root,key,value);
    }

    // 获取
    V get(K key){
        TreeNode<K,V> *node = root;
        while (node){
            if(node->key == key)
                return node->value;
            else if(node->key > key)
                node = node->left;
            else if(node->key < key)
                node = node->right;
        }
        return NULL;
    }

    int size(){
        return count;
    }

    // 是否包含
    bool contains(K key){
        TreeNode<K,V> *node = root;
        while (node){
            if(node->key == key)
                return true;
            else if(node->key > key)
                node = node->left;
            else if(node->key < key)
                node = node->right;
        }
        return false;
    }



    // 移除
    void remove(K key){
        root = removeNode(root,key);
    }

    // 中序遍历 就是相当于从小到大的升序遍历了
    void infixOrderTraverse(void(*log)(int,int)){
        infixOrderTraverse(root,log);
    }



private:
    // 删除所有的数据
    void deleteAllNode(TreeNode<K,V> *pNode){
        if(pNode->left)
            deleteAllNode(pNode->left);
        if(pNode->right)
            deleteAllNode(pNode->right);
        delete(pNode);
    }

    TreeNode<K,V> *addNode(TreeNode<K,V> *pNode,K key,V value){
        if(!pNode){
            count++;
            return new TreeNode<K,V>(key,value);
        }

        if(pNode->value > key){
            pNode->left = addNode(pNode->left,key,value);
        } else if (pNode->value < key){
            pNode->right = addNode(pNode->right,key,value);
        }else{
            pNode->value = value;
        }

        return pNode;
    }

    TreeNode<K,V> *removeNode(TreeNode<K,V> *pNode,K key){
        if(pNode->key > key)
            pNode->left = removeNode(pNode->left,key);
        else if(pNode->key < key)
            pNode->right = removeNode(pNode->right);
        else{ // 相等找到了
            count --;
            if(pNode->left == NULL && pNode->right == NULL){
                delete(pNode);
                return NULL;
            }else if(pNode->left == NULL){
                TreeNode<K, V> *right = pNode->right;
                delete (pNode);
                return right;
            } else if (pNode->right == NULL) {
                TreeNode<K, V> *left = pNode->left;
                delete (pNode);
                return left;
            } else {
                // 左右两子树都不为空（把左子树的最大值作为根，或者右子树的最小值作为根）
                TreeNode<K, V> *successor = new TreeNode<K, V>(maximum(pNode->left));
                successor->left = deleteMax(pNode->left);
                count++;
                successor->right = pNode->right;
                delete (pNode);
                return successor;
            }
        }
        return pNode;
    }


    TreeNode<K, V> *deleteMax(TreeNode<K, V> *pNode) {
        if (pNode->right == NULL) {
            TreeNode<K, V> *left = pNode->left;
            delete (pNode);
            count--;
            return left;
        }
        pNode->right = deleteMax(pNode->right);
        return pNode;
    }

    // 查找当前树的左边的最大值 （右边的最小值怎么找？）
    TreeNode<K, V> *maximum(TreeNode<K, V> *pNode) {
        // 不断的往右边找，直到找到右子树为空节点
        if (pNode->right == NULL) {
            return pNode;
        }
        return maximum(pNode->left);
    }

    void infixOrderTraverse(TreeNode<K,V> *pNode,void(*log)(int,int)){
        // 先左孩子
        infixOrderTraverse(pNode->left,log);

        // 再根节点
        log(pNode->key,pNode->value);

        // 再右孩子
        infixOrderTraverse(pNode->right,log);
    }
};


#endif //NDKPRACTICE_BST_HO
