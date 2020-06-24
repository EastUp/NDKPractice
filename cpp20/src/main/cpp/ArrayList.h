#ifndef JNI20_ARRAYLIST_H
#define JNI20_ARRAYLIST_H


template<class E>
class ArrayList {
private:
    // 长度，数组，当前交表
    E *array;      // 当前数组指针
    int len = 0;   // 数组大小
    int index = 0; // 当前角标
public:
    ArrayList();

    ArrayList(int len);

    void add(E e);

    E remove(int index);

    E get(int index);

    ~ArrayList();
};

#endif // JNI20_ARRAYLIST_H