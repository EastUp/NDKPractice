//
// Created by 123 on 2020/7/10.
//

#ifndef NDKPRACTICE_PRIORITYQUEUE_HPP
#define NDKPRACTICE_PRIORITYQUEUE_HPP

#include <string.h>

using namespace std;

template<class E>
class PriorityQueue {
    int count = 0; // 数组的大小，不够要扩容
    int index = 0; // 当前数组的角标位置
    E *array = NULL; // 数据数组

private:

    // 向上调整为大根堆
    void shiftUp(int index) {
        if(index > 1 && array[index] > array[index/2]){
            swap(array[index],array[index/2]);
            shiftUp(index/2);
        }
    }

    // 向下调整为大根堆
    void shiftDown(int k) {
        while (k * 2 <= index){ // 到底的情况
            int max = k*2; // 先假设左孩子是最大的
            if(max + 1 <= index && array[max] < array[max + 1]){
                max = max + 1; // 右孩子比左孩子大
            }

            // 因为没弹出之前就是大根堆，所以如果比子孩子最大的都还要大就不用排序了
            if(array[k] > array[max])
                break;

            // 交换，最大的往上冒
            swap(array[k],array[max]);

            k = max;
        }
    }

public:
    PriorityQueue(int count) : count(count) {
        array = new E[count];
    }

    bool isEmpty() {
        return index == 0;
    }

    E pop() {
        E max = array[1];
        // 把最后一个赋值到第一个
        array[1] = array[index];
        array[index] = NULL;
        index --;
        shiftDown(1);
        return max;
    }

    void push(E e){
        array[index + 1] = e;
        index ++;
        // 不断的调整堆
        shiftUp(index);
    }

};


#endif //NDKPRACTICE_PRIORITYQUEUE_HPP
