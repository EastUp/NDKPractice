#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "huffman.h"
#include "pQueue.h"
#include <android/log.h>

#define TAG "TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

void traverseTree(htNode *treeNode, hlTable **table, int k, char code[256]) {
    //If we reach the end we introduce the code in the table
    if (treeNode->left == NULL && treeNode->right == NULL) {
        code[k] = '\0';
        hlNode *aux = (hlNode *) malloc(sizeof(hlNode));
        aux->code = (char *) malloc(sizeof(char) * (strlen(code) + 1));
        strcpy(aux->code, code);
        aux->symbol = treeNode->symbol;
        aux->next = NULL;
        if ((*table)->first == NULL) {
            // 映射表为 null
            (*table)->first = aux;
            (*table)->last = aux;
        } else {
            // 映射表不为空
            (*table)->last->next = aux;
            (*table)->last = aux;
        }
    }

    //We concatenate a 0 for each step to the left
    if (treeNode->left != NULL) {
        code[k] = '0';
        traverseTree(treeNode->left, table, k + 1, code);

    }
    //We concatenate a 1 for each step to the right
    if (treeNode->right != NULL) {
        code[k] = '1';
        traverseTree(treeNode->right, table, k + 1, code);

    }
}

hlTable *buildTable(htTree *huffmanTree) {
    //We initialize the table
    hlTable *table = (hlTable *) malloc(sizeof(hlTable));
    table->first = NULL;
    // 这里为什么要有 last
    table->last = NULL;

    //Auxiliary variables
    char code[256];
    //k will memories the level on which the traversal is
    int k = 0;

    //We traverse the tree and calculate the codes
    traverseTree(huffmanTree->root, &table, k, code);
    return table;
}

htTree *buildTree(char *inputString) {
    //The array in which we calculate the frequency of the symbols
    //Knowing that there are only 256 posibilities of combining 8 bits
    //(256 ASCII characters)
    int *probability = (int *) malloc(sizeof(int) * 256);

    //We initialize the array
    // 数组初始化为 0
    for (int i = 0; i < 256; i++)
        probability[i] = 0;

    //We consider the symbol as an array index and we calculate how many times each symbol appears
    for (int i = 0; inputString[i] != '\0'; i++) // a  p[97] ++   2
        probability[(unsigned char) inputString[i]]++;

    // The queue which will hold the tree nodes
    // 初始化队列
    pQueue *huffmanQueue;
    initPQueue(&huffmanQueue);

    // 构建一个队列 ， 不断的从队列中取两个出现次数最小的，生成二叉树后重新加入队列
    //We create nodes for each symbol in the string
    for (int i = 0; i < 256; i++)
        if (probability[i] != 0) {
            htNode *aux = (htNode *) malloc(sizeof(htNode));
            aux->left = NULL;
            aux->right = NULL;
            aux->symbol = (char) i;

            addPQueue(&huffmanQueue, aux, probability[i]);
        }

    //We free the array because we don't need it anymore
    free(probability);

    //We apply the steps described in the article to build the tree

    // 构建一棵霍夫曼树
    while (huffmanQueue->size != 1) {
        // 第一个的出现频率
        int priority = huffmanQueue->first->priority;
        // 拿第二个与第一个相加
        priority += huffmanQueue->first->next->priority;

        // 创建两个树的子节点节点
        htNode *left = getPQueue(&huffmanQueue);
        htNode *right = getPQueue(&huffmanQueue);

        // 创建一个新的节点
        htNode *newNode = (htNode *) malloc(sizeof(htNode));
        newNode->left = left;
        newNode->right = right;

        // 重新插入原来的队列
        addPQueue(&huffmanQueue, newNode, priority);
    }

    //We create the tree 创建一棵树
    htTree *tree = (htTree *) malloc(sizeof(htTree));
    tree->root = getPQueue(&huffmanQueue);

    return tree;
}

void encode(hlTable *table, char *stringToEncode) {
    hlNode *traversal;

    printf("\nEncoding\nInput string : %s\nEncoded string : \n", stringToEncode);

    //For each element of the string traverse the table
    //and once we find the symbol we output the code for it
    LOGE("len = %d",strlen(stringToEncode));
    for (int i = 0; i < strlen(stringToEncode); i++) {
        traversal = table->first;
        LOGE("字符:%c",stringToEncode[i]);
        while (traversal->symbol != stringToEncode[i])
            traversal = traversal->next;
        __android_log_print(ANDROID_LOG_ERROR, TAG, "%c,%s", traversal->symbol, traversal->code);
    }
    // __android_log_print(ANDROID_LOG_ERROR,TAG,"\n");
}

void decode(htTree *tree, char *stringToDecode) {
    htNode *traversal = tree->root;

    printf("\nDecoding\nInput string : %s\nDecoded string : \n", stringToDecode);

    //For each "bit" of the string to decode
    //we take a step to the left for 0
    //or ont to the right for 1
    for (int i = 0; stringToDecode[i] != '\0'; i++) {
        if (traversal->left == NULL && traversal->right == NULL) {
            LOGE("%c", traversal->symbol);
            traversal = tree->root;
        }

        if (stringToDecode[i] == '0')
            traversal = traversal->left;

        if (stringToDecode[i] == '1')
            traversal = traversal->right;

        if (stringToDecode[i] != '0' && stringToDecode[i] != '1') {
            printf("The input string is not coded correctly!\n");
            return;
        }
    }

    if (traversal->left == NULL && traversal->right == NULL) {
        LOGE("%c", traversal->symbol);
        traversal = tree->root;
    }
}