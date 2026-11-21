// ============================================================
//  my_vector.h  ——  MyVector v1（int 专用版）
//  目标：用最少的代码，讲清 "一块堆内存 + 三个数字" 是怎么管起来的。
//  这版不做：拷贝/移动（W3 加）、迭代器（W4 加）、模板（W4 加）。
// ============================================================
#pragma once                 // 头文件守卫：保证只被 include 一次
#include <cstddef>           // std::size_t

class MyVector {
public:
    // ---------- 生命周期：内存什么时候出生、什么时候死亡 ----------
    MyVector();              // 默认构造：空数组，先不申请内存
    ~MyVector();             // 析构：把内存还回去（RAII 的"释放"那一半）

    // ---------- 查询：只看不改 ----------
    std::size_t size() const;       // 已经放了几个元素
    std::size_t capacity() const;   // 一共申请了几个位置
    bool empty() const;             // size() == 0

    // ---------- 访问 ----------
    int& operator[](std::size_t index);              // 可读可写
    const int& operator[](std::size_t index) const;  // 只读对象用这个

    // ---------- 修改 ----------
    void push_back(int value);         // 末尾追加，满了就扩容
    void reserve(std::size_t new_cap); // 一次性申请够，避免反复扩容
    void clear();                      // 清空元素，但不释放内存

    // ---------- 测试用：当前堆上还活着几块内存（正常应最终归零） ----------
    static int live_blocks();

private:
    int* data_;            // 指向堆上那块内存；没申请时是 nullptr
    std::size_t size_;     // 用掉几个格子
    std::size_t capacity_; // 一共几个格子

    // 扩容的唯一出口：整个类里 new / delete 只出现在这个函数里
    void reallocate(std::size_t new_cap);

    static int s_live_blocks;  // 静态成员：所有 MyVector 对象共享这一份
};
