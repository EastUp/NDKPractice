
// 如果开发中涉及到模板类，声明和实现要写到同一个类中， hpp = h + cpp/c (编译)

#include "ArrayList.h"

#include "malloc.h"
template <class E>
ArrayList<E>::ArrayList() {}


// 每次实现方法都得添加
template <class E>
ArrayList<E>::ArrayList(int len) {
    if(len <=0 )
        return ;
    this->len = len;
    this->array = (E*)malloc(sizeof(E)*len);
}