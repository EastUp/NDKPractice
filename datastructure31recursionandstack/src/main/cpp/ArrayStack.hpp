//
// Created by 123 on 2020/7/6.
//

#ifndef NDKPRACTICE_ARRAYSTACK_HPP
#define NDKPRACTICE_ARRAYSTACK_HPP

#include <malloc.h>
#include <assert.h>

// 用数组来实现（java stack源码）
template<class E>
class ArrayStack {
private:
    // 栈顶元素的角标位置
    int top = -1;
    E *array = NULL;
    int size = 10; // 栈的初始大小
public:
    // 构造和析构
    ArrayStack();
    ~ArrayStack();

public:
    /**
     *  将元素压入栈中
     */
    void push(E e);

    /**
     *  获取栈顶的元素，不弹出
     */
    E peek();

    /**
     * 弹出栈顶元素
     */
    E pop();

    /**
     *  栈是否为空
     */
    bool isEmpty();

private:
    void growArray();
};

template<class E>
ArrayStack<E>::ArrayStack() {
    array = (E*)malloc(sizeof(E)*size);
}

template<class E>
ArrayStack<E>::~ArrayStack() {
    delete[] array;
}

template<class E>
void ArrayStack<E>::growArray(){
    size = size << 1;
    // 改变内存空间大小
    array = (E*) realloc(array,size * sizeof(E));
}

template<class E>
void ArrayStack<E>::push(E e) {
    if(top + 1 >= size)
        growArray(); // 扩容
    array[top++] = e;
}

template<class E>
E ArrayStack<E>::peek() {
    return array[top];
}

template<class E>
E ArrayStack<E>::pop() {
    assert(top >= 0);
    E e = peek();
    array[top--] = NULL;
    return e;
}

template<class E>
bool ArrayStack<E>::isEmpty() {
    return top < 0;
}



#endif //NDKPRACTICE_ARRAYSTACK_HPP
