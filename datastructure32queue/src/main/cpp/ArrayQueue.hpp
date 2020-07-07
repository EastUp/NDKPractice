//
// Created by 123 on 2020/7/7.
//

#ifndef NDKPRACTICE_ARRAYQUEUE_HPP
#define NDKPRACTICE_ARRAYQUEUE_HPP

#include <malloc.h>

#define TAG "TAG"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

template <class E>
class ArrayQueue{
private:
    // 必须是 2 的幂次
    int size = 0;
    // 队列的头索引（是反向的添加，从 size - 1 ,size -2,...添加）
    int head = 0;
    // 队尾索引
    int tail = 0;
    // 数组地址
    E *array = NULL;
public:
    ArrayQueue();

    ArrayQueue(int size);

    ~ArrayQueue();

public:

    // 添加到队尾巴
    void enqueue(E e);

    // 移除队首的元素（指的是 tail的位置）
    E dequeue();

    // 获取队首的位置，但不移除
    E peek();

    bool isEmpty();

private:
    void growArray(); // 扩容数组

    void copyElement(E *src,int sPo, E *dest,int dPo,int len); // 拷贝数组元素到新的数组
};

template <class E>
ArrayQueue<E>::ArrayQueue():ArrayQueue(4){

}

template <class E>
ArrayQueue<E>::ArrayQueue(int cap){
    // cap 可以乱传，以下代码保证是 2 的幂次
    int n = cap - 1;
    if(cap >= 2){
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
    }
    n += 1;
    LOGE("size = %d",n);
    size = n;
    array = (E*)malloc(sizeof(E) * size);
    for (int i = 0; i < size; ++i) {
        array[i] = NULL;
    }
}

template <class E>
ArrayQueue<E>::~ArrayQueue() {
    free(array);
}

template <class E>
void ArrayQueue<E>::enqueue(E e) {

    if(head == tail && array[head]!=NULL){ // 弹出的时候把对应的元素置为 NULL,这样好判断能否扩容
        LOGE("扩容了 head = %d,tail = %d",head,tail);
        // 扩容，双端队列，头部增加删除，也可以在尾部增加删除
        growArray();
    }

    // -1 二进制为：-1的源码的补码
    head = (head - 1) & (size - 1); //(-1 & size -1  = size - 1) 这样head的取值才是：size-1,size-2,size-3
    array[head] = e;
}

template <class E>
E ArrayQueue<E>::peek() {
    return array[(tail - 1) & (size - 1)];
}

template <class E>
E ArrayQueue<E>::dequeue() {
    tail = (tail - 1) & (size - 1);
    E value = array[tail];
    array[tail] = NULL; // 弹出去的元素置为 NULL;
    return value;
}

template <class E>
bool ArrayQueue<E>::isEmpty() {
    return head == tail && array[head]==NULL;
}


template <class E>
void ArrayQueue<E>::growArray() {
    int new_size = size << 1; // 扩容为原来的两倍

    E *new_array = (E*)malloc(sizeof(E) * new_size);

    // 对数据进行 copy, 将 tail 后面的拷贝到前面，将tail 前面的拷贝到后面
    int rightLen = size - tail;
    copyElement(array,tail,new_array,0,rightLen);
    copyElement(array,0,new_array,rightLen,tail);

    free(array);
    head = 0;
    tail = size;
    size = new_size;
    array = new_array;
}

template <class E>
void ArrayQueue<E>::copyElement(E *src, int sPo, E *dest, int dPo, int len) {
    for (int i = 0; i <len; ++i) {
        dest[dPo+i] = src[sPo+i];
    }
}


#endif //NDKPRACTICE_ARRAYQUEUE_HPP
