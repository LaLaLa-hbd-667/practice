// ============================================================
//  my_vector.h  ——  MyVector v0.2（int 专用版 · 五大件版）
//  相比 v1 新增：拷贝构造、拷贝赋值、移动构造、移动赋值、at()。
//  仍然不做：模板（第 3 步）、迭代器（第 3 步）。
// ============================================================
#pragma once
#include <cstddef>   // std::size_t
#include <stdexcept> // std::out_of_range

class MyVector {
public:
    // ---------- 生命周期：五大件 ----------
    MyVector();                                     // 默认构造：空数组
    MyVector(const MyVector& other);                // ① 拷贝构造：深拷贝
    MyVector(MyVector&& other) noexcept;            // ② 移动构造：偷指针
    MyVector& operator=(const MyVector& other);     // ③ 拷贝赋值：先建后拆
    MyVector& operator=(MyVector&& other) noexcept; // ④ 移动赋值：先拆后偷
    ~MyVector();                                    // ⑤ 析构：还内存

    // ---------- 查询 ----------
    std::size_t size() const;
    std::size_t capacity() const;
    bool empty() const;

    // ---------- 访问 ----------
    int& operator[](std::size_t index);              // 不检查越界（快）
    const int& operator[](std::size_t index) const;
    int& at(std::size_t index);                      // 越界抛 std::out_of_range
    const int& at(std::size_t index) const;

    // ---------- 修改 ----------
    void push_back(int value);
    void reserve(std::size_t new_cap);
    void clear();

    // ---------- 测试用统计 ----------
    static int live_blocks();     // 当前活着的堆块数，最终必须归零
    static int copy_count();      // 拷贝构造 + 拷贝赋值 的总次数
    static int move_count();      // 移动构造 + 移动赋值 的总次数
    static void reset_counters();

private:
    int* data_;            // 堆上那块内存；没申请时是 nullptr
    std::size_t size_;     // 用掉几个格子
    std::size_t capacity_; // 一共几个格子

    // 扩容的唯一出口：new / delete 只出现在这里（和 v1 一样）
    void reallocate(std::size_t new_cap);

    // 释放当前这块内存（析构、拷贝赋值、移动赋值都要用，抽出来避免写三遍）
    void release();

    static int s_live_blocks;
    static int s_copy_count;
    static int s_move_count;
};
