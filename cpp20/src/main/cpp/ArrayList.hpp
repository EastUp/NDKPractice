#ifndef JNI20_ARRAYLIST_H
#define JNI20_ARRAYLIST_H

#include <malloc.h>
//------------------------ 声明 ------------------------//

template<class E>
class ArrayList {
private:
    // 长度，数组，当前交表
    E *array = NULL;      // 当前数组指针
    int len = 0;   // 数组大小
    int index = 0; // 当前角标
public:
    ArrayList();

    ArrayList(int len);

    ~ArrayList();

    ArrayList(const ArrayList& list);

    void add(E e);

    E remove(int index);

    E get(int index);

    int size();

private:
    void ensureCapacityInternal(int capacity);

    void grow(int capacity);
};

//------------------------ 实现 ------------------------//
template <class E>
ArrayList<E>::ArrayList() {}

// 每个方法都得添加
template <class E>
ArrayList<E>::ArrayList(int len) {
    if(len <= 0)
        return;
    this->len = len;
    this->array = (E*)malloc(sizeof(E)*len);
}

template <class E>
ArrayList<E>::~ArrayList() {
    if(this->array){
        free(this->array);
        this->array = NULL;
    }
}

template <class E>
ArrayList<E>::ArrayList(const ArrayList &list) {
    this->len = list.len;
    this->index = list.index;
    if(this->array){
       free(this->array);
    }
    this->array = malloc(sizeof(E)*list.len);
    memcpy(this->array,list.array, sizeof(E)*index);
}


template <class E>
int ArrayList<E>::size() {
    return this->index;
}

template <class E>
void ArrayList<E>::add(E e) {
    ensureCapacityInternal(index+1); // Increments modCount!!
    this->array[index++] = e;
}

template <class E>
void ArrayList<E>::ensureCapacityInternal(int capacity) {
    if(this->array == NULL)
        capacity = 10;
    if(capacity > this->len){
        // 创建新数组
        grow(capacity);
    }
}

template <class E>
void ArrayList<E>::grow(int capacity) {
    int new_len = len + (len >> 1); // 扩容 len的一半

    if(capacity > new_len){
        new_len = capacity;
    }

    // 创建新的数组
    E* new_array = (E*)malloc(sizeof(E)*new_len);

    if(this->array){
        memcpy(new_array,this->array,sizeof(E)* index); // sizeof(E)*index 字节
        // 释放旧的array
        free(this->array);
    }

    this->array = new_array;
    this->len = new_len;
}

template <class E>
E ArrayList<E>::get(int index) {
    return array[index];
}

template <class E>
E ArrayList<E>::remove(int index) {
    E e = array[index];
    int numMoved = this->index - index - 1;
    for(int i = index; i < index + numMoved ;i++){
        array[i] = array[i+1];
    }
    array[--this->index] = NULL;

    return e;
}


#endif // JNI20_ARRAYLIST_H