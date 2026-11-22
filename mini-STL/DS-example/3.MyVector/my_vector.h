// ============================================================
//  my_vector.h  ——  MyVector v0.3（模板 + 迭代器版）
//
//  和前两版最大的不同：这个文件夹里【没有 my_vector.cpp】。
//  模板的成员函数只有被"实例化"（T 确定下来）时才会生成代码，
//  而 .cpp 是单独编译的，编译器在编译 my_vector.cpp 时不知道 T 是什么，
//  于是什么也不生成 → 链接时 test 里调用的 push_back 找不到实现
//  → undefined reference to `MyVector<int>::push_back(int)`
//
//  所以：模板类的实现必须和声明放在同一个头文件里。
//  这就是"全部塞进一个 .h"不是因为懒，是语言规则逼的。
// ============================================================
#pragma once
#include <cstddef>   // std::size_t
#include <stdexcept> // std::out_of_range
#include <utility>   // std::move

template <typename T>
class MyVector {
public:
    // ---------- 给外部看的类型别名 ----------
    using value_type = T;
    using iterator = T*;              // 迭代器就是指针，先不封装
    using const_iterator = const T*;

    // ---------- 五大件 ----------
    MyVector();
    MyVector(const MyVector& other);
    MyVector(MyVector&& other) noexcept;
    MyVector& operator=(const MyVector& other);
    MyVector& operator=(MyVector&& other) noexcept;
    ~MyVector();

    // ---------- 查询 ----------
    std::size_t size() const;
    std::size_t capacity() const;
    bool empty() const;

    // ---------- 访问 ----------
    T& operator[](std::size_t index);
    const T& operator[](std::size_t index) const;
    T& at(std::size_t index);
    const T& at(std::size_t index) const;

    // ---------- 修改 ----------
    void push_back(const T& value);  // 传进来的是左值（要拷贝）
    void push_back(T&& value);       // 传进来的是右值（可以移动）
    void reserve(std::size_t new_cap);
    void clear();

    // ---------- 迭代器（范围 for 靠的就是这两个函数） ----------
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    // ---------- 测试用统计 ----------
    static int live_blocks();
    static int copy_count();
    static int move_count();
    static void reset_counters();

private:
    T* data_;
    std::size_t size_;
    std::size_t capacity_;

    void reallocate(std::size_t new_cap);
    void release();

    // 注意：模板的静态成员是"每个 T 各有一份"，
    // MyVector<int>::s_live_blocks 和 MyVector<std::string>::s_live_blocks 是两个变量。
    static int s_live_blocks;
    static int s_copy_count;
    static int s_move_count;
};

// ============================================================
//  静态成员必须在头文件里定义（每个实例化各生成一份）
//  写成 my_vector.cpp 里那种 int MyVector::s_live_blocks = 0; 是不行的
// ============================================================
template <typename T>
int MyVector<T>::s_live_blocks = 0;

template <typename T>
int MyVector<T>::s_copy_count = 0;

template <typename T>
int MyVector<T>::s_move_count = 0;

// ============================================================
//  以下是实现，全部内联在头文件里
// ============================================================

template <typename T>
MyVector<T>::MyVector() : data_(nullptr), size_(0), capacity_(0) {}

template <typename T>
MyVector<T>::MyVector(const MyVector& other)
    : data_(nullptr), size_(other.size_), capacity_(other.size_) {
    ++s_copy_count;
    if (size_ > 0) {
        // 注意这里用的是 new T[n]：它会"默认构造" n 个元素，
        // 之后再用赋值把数据填进去。所以这版要求 T 能默认构造。
        // 真正的 std::vector 用 placement new 只构造 size 个，没这个要求。
        data_ = new T[size_];
        ++s_live_blocks;
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }
}

template <typename T>
MyVector<T>::MyVector(MyVector&& other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    ++s_move_count;
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

template <typename T>
MyVector<T>& MyVector<T>::operator=(const MyVector& other) {
    if (this == &other) {
        return *this;
    }
    ++s_copy_count;

    T* new_data = nullptr;
    if (other.size_ > 0) {
        new_data = new T[other.size_];
        ++s_live_blocks;
        for (std::size_t i = 0; i < other.size_; ++i) {
            new_data[i] = other.data_[i];
        }
    }

    release();
    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.size_;
    return *this;
}

template <typename T>
MyVector<T>& MyVector<T>::operator=(MyVector&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    ++s_move_count;

    release();
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
    return *this;
}

template <typename T>
MyVector<T>::~MyVector() {
    release();
}

template <typename T>
void MyVector<T>::release() {
    if (data_ != nullptr) {
        delete[] data_;   // 对每个元素调用析构函数，再释放整块内存
        --s_live_blocks;
    }
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template <typename T>
std::size_t MyVector<T>::size() const { return size_; }

template <typename T>
std::size_t MyVector<T>::capacity() const { return capacity_; }

template <typename T>
bool MyVector<T>::empty() const { return size_ == 0; }

template <typename T>
int MyVector<T>::live_blocks() { return s_live_blocks; }

template <typename T>
int MyVector<T>::copy_count() { return s_copy_count; }

template <typename T>
int MyVector<T>::move_count() { return s_move_count; }

template <typename T>
void MyVector<T>::reset_counters() {
    s_copy_count = 0;
    s_move_count = 0;
}

template <typename T>
T& MyVector<T>::operator[](std::size_t index) { return data_[index]; }

template <typename T>
const T& MyVector<T>::operator[](std::size_t index) const { return data_[index]; }

template <typename T>
T& MyVector<T>::at(std::size_t index) {
    if (index >= size_) throw std::out_of_range("MyVector::at: index out of range");
    return data_[index];
}

template <typename T>
const T& MyVector<T>::at(std::size_t index) const {
    if (index >= size_) throw std::out_of_range("MyVector::at: index out of range");
    return data_[index];
}

template <typename T>
void MyVector<T>::reallocate(std::size_t new_cap) {
    // 和 v1 一样：new T[new_cap] 会默认构造 new_cap 个元素
    T* new_data = new T[new_cap];
    ++s_live_blocks;

    for (std::size_t i = 0; i < size_; ++i) {
        new_data[i] = data_[i];   // 拷贝赋值（真正的 vector 这里会优先移动）
    }

    if (data_ != nullptr) {
        delete[] data_;
        --s_live_blocks;
    }

    data_ = new_data;
    capacity_ = new_cap;
}

template <typename T>
void MyVector<T>::push_back(const T& value) {
    if (size_ == capacity_) {
        std::size_t new_cap = (capacity_ == 0) ? 1 : capacity_ * 2;
        reallocate(new_cap);
    }
    data_[size_] = value;
    ++size_;
}

template <typename T>
void MyVector<T>::push_back(T&& value) {
    if (size_ == capacity_) {
        std::size_t new_cap = (capacity_ == 0) ? 1 : capacity_ * 2;
        reallocate(new_cap);
    }
    data_[size_] = std::move(value);   // 右值：直接搬，不拷贝
    ++size_;
}

template <typename T>
void MyVector<T>::reserve(std::size_t new_cap) {
    if (new_cap > capacity_) {
        reallocate(new_cap);
    }
}

template <typename T>
void MyVector<T>::clear() { size_ = 0; }

// ---------------- 迭代器 ----------------
// 空 vector 时 data_ 是 nullptr，begin() == end() == nullptr，
// 范围 for 会立刻结束，不会出问题。
template <typename T>
typename MyVector<T>::iterator MyVector<T>::begin() { return data_; }

template <typename T>
typename MyVector<T>::iterator MyVector<T>::end() { return data_ + size_; }

template <typename T>
typename MyVector<T>::const_iterator MyVector<T>::begin() const { return data_; }

template <typename T>
typename MyVector<T>::const_iterator MyVector<T>::end() const {
    return data_ + size_;
}
