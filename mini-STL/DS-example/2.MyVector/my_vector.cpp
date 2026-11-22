// ============================================================
//  my_vector.cpp  ——  MyVector v0.2 的实现
//
//  这个文件只用记三个"安全口诀"，五大件全是从它们推出来的：
//    拷贝 = 申请一块新的，把数据搬过去（源对象不动）
//    移动 = 直接把指针接过来，把源对象置空（不申请、不拷贝）
//    赋值 = 先处理掉自己的旧内存，再照上面两种说法之一做
// ============================================================
#include "my_vector.h"

int MyVector::s_live_blocks = 0;
int MyVector::s_copy_count = 0;
int MyVector::s_move_count = 0;

// ---------------- 默认构造 ----------------
MyVector::MyVector() : data_(nullptr), size_(0), capacity_(0) {}

// ---------------- ① 拷贝构造：深拷贝 ----------------
// 标准库 vector 的拷贝构造只按 size() 申请（容量是实现细节，不保证一样）。
// 这里也一样：新对象恰好装得下元素即可。
MyVector::MyVector(const MyVector& other)
    : data_(nullptr), size_(other.size_), capacity_(other.size_) {
    ++s_copy_count;
    if (size_ > 0) {
        data_ = new int[size_];
        ++s_live_blocks;
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }
}

// ---------------- ② 移动构造：偷指针 ----------------
// noexcept 必须写：标准库（比如 std::vector 扩容）只有在移动构造声明
// 不会抛异常时，才敢用它搬元素；否则宁可退回去做拷贝。
MyVector::MyVector(MyVector&& other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    ++s_move_count;
    other.data_ = nullptr;   // 这三行是关键：源对象交出去之后必须"空"
    other.size_ = 0;         // 否则两边都会 delete 同一块内存
    other.capacity_ = 0;
}

// ---------------- ③ 拷贝赋值：先建后拆 ----------------
// 顺序很重要：先申请新内存并拷完，最后才释放自己的旧块。
// 这样即使 new 抛异常，*this 还是原来的样子（基本的异常安全），
// 而且自己给自己赋值时，也不会出现"先 delete 再读已释放内存"。
MyVector& MyVector::operator=(const MyVector& other) {
    if (this == &other) {     // 自赋值：a = a，直接返回
        return *this;         // 没有这句，下面会先释放自己的内存再去读它
    }
    ++s_copy_count;

    int* new_data = nullptr;
    if (other.size_ > 0) {
        new_data = new int[other.size_];   // ① 先建
        ++s_live_blocks;
        for (std::size_t i = 0; i < other.size_; ++i) {
            new_data[i] = other.data_[i];
        }
    }

    release();                 // ② 后拆：释放自己的旧块
    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.size_;
    return *this;              // 支持 a = b = c 这种链式赋值
}

// ---------------- ④ 移动赋值：先拆后偷 ----------------
MyVector& MyVector::operator=(MyVector&& other) noexcept {
    if (this == &other) {     // 自移动赋值：a = std::move(a)
        return *this;
    }
    ++s_move_count;

    release();                 // 先把自己手里的旧内存还掉

    data_ = other.data_;       // 再偷对方的
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;     // 对方交出来之后必须置空
    other.size_ = 0;
    other.capacity_ = 0;
    return *this;
}

// ---------------- ⑤ 析构 ----------------
MyVector::~MyVector() { release(); }

// ---------------- 释放（内部工具） ----------------
void MyVector::release() {
    if (data_ != nullptr) {
        delete[] data_;
        --s_live_blocks;
    }
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

// ---------------- 查询 ----------------
std::size_t MyVector::size() const { return size_; }
std::size_t MyVector::capacity() const { return capacity_; }
bool MyVector::empty() const { return size_ == 0; }

int MyVector::live_blocks() { return s_live_blocks; }
int MyVector::copy_count() { return s_copy_count; }
int MyVector::move_count() { return s_move_count; }

void MyVector::reset_counters() {
    s_copy_count = 0;
    s_move_count = 0;
}

// ---------------- 下标访问 ----------------
int& MyVector::operator[](std::size_t index) { return data_[index]; }
const int& MyVector::operator[](std::size_t index) const { return data_[index]; }

// ---------------- 带检查的访问 ----------------
int& MyVector::at(std::size_t index) {
    if (index >= size_) throw std::out_of_range("MyVector::at: index out of range");
    return data_[index];
}
const int& MyVector::at(std::size_t index) const {
    if (index >= size_) throw std::out_of_range("MyVector::at: index out of range");
    return data_[index];
}

// ---------------- 扩容 ----------------
void MyVector::reallocate(std::size_t new_cap) {
    int* new_data = new int[new_cap];
    ++s_live_blocks;

    for (std::size_t i = 0; i < size_; ++i) {
        new_data[i] = data_[i];
    }

    if (data_ != nullptr) {
        delete[] data_;
        --s_live_blocks;
    }

    data_ = new_data;
    capacity_ = new_cap;
}

// ---------------- 追加 ----------------
void MyVector::push_back(int value) {
    if (size_ == capacity_) {
        std::size_t new_cap = (capacity_ == 0) ? 1 : capacity_ * 2;
        reallocate(new_cap);
    }
    data_[size_] = value;
    ++size_;
}

// ---------------- 预申请 ----------------
void MyVector::reserve(std::size_t new_cap) {
    if (new_cap > capacity_) {
        reallocate(new_cap);
    }
}

// ---------------- 清空 ----------------
void MyVector::clear() { size_ = 0; }
