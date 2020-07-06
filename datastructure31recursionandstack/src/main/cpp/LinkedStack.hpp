//
// Created by 123 on 2020/7/6.
//

#ifndef NDKPRACTICE_LINKEDSTACK_HPP
#define NDKPRACTICE_LINKEDSTACK_HPP

#include <assert.h>

template<class E>
class Node {
public:
    E value = NULL;
    Node<E> *next = NULL;
public:
    Node(E value, Node<E> *next) : value(value), next(next) {
    }
};

template<class E>
class LinkedStack {
private:
    Node<E> *first = NULL; // 头结点
    int index = -1;
    Node<E> *top = NULL; // 栈顶元素
public:
    LinkedStack();

    ~LinkedStack();

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
    Node<E> *node(int index);
};

template<class E>
Node<E> *LinkedStack<E>::node(int index) {

    if (index == 0)
        return first;
    else if (index < 0)
        return NULL;

    Node<E> *h = first;

    for (int i = 0; i < index; ++i) {
        h = h->next;
    }
    return h;
}


template<class E>
void LinkedStack<E>::push(E e) {
    Node<E> *new_node = new Node<E>(e, NULL);
    if (!first)
        first = new_node;
    else
        top->next = new_node;
    top = new_node;
    index++;
}

template<class E>
E LinkedStack<E>::peek() {
    return top;
}

template<class E>
E LinkedStack<E>::pop() {
    assert(index >= 0);
    E e = top->value;
    delete top;

    top = node(--index);
    return e;
}

template<class E>
bool LinkedStack<E>::isEmpty() {
    return index < 0;
}

template<class E>
LinkedStack<E>::LinkedStack() {}

template<class E>
LinkedStack<E>::~LinkedStack() {
    for (int i = 0; i < index; ++i) {
        delete(node(i));
    }

    first = NULL;
    top = NULL;
    index = -1;
}

#endif //NDKPRACTICE_LINKEDSTACK_HPP
