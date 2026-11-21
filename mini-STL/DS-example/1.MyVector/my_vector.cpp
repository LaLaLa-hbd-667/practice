// ============================================================
//  my_vector.cpp  ——  MyVector v1 的实现
//  记住一条规则：这个文件里，new[] 只允许出现在 reallocate()，
//                delete[] 只允许出现在 reallocate() 和 ~MyVector()。
// ============================================================
#include "my_vector.h"

// 静态成员必须在 .cpp 里定义一次（否则链接器找不到它）。
int MyVector::s_live_blocks = 0;

// ---------------- 构造函数：只初始化，不申请内存 ----------------
// 空数组不占堆内存，所以三个成员直接给零值。
MyVector::MyVector() : data_(nullptr), size_(0), capacity_(0) {}

// ---------------- 析构函数：把内存还回去 ----------------
// 每个 new[] 都必须有一个对应的 delete[]，这就是 RAII 的"释放"半边。
// 注意：delete[] nullptr 是安全的空操作，但这里仍显式判空，
//      因为我们要维护 s_live_blocks 这个账本。
MyVector::~MyVector() {
    if (data_ != nullptr) {
        delete[] data_;
        --s_live_blocks;
        data_ = nullptr;  // 防御性写法：万一以后有人在析构后还调成员函数
    }
    size_ = 0;
    capacity_ = 0;
}

// ---------------- 查询 ----------------
std::size_t MyVector::size() const { return size_; }
std::size_t MyVector::capacity() const { return capacity_; }
bool MyVector::empty() const { return size_ == 0; }
int MyVector::live_blocks() { return s_live_blocks; }

// ---------------- 下标访问 ----------------
// 不做越界检查，和 std::vector::operator[] 一致：
// 越界是调用者的错（未定义行为）。要检查请用 at()，那是下一版的事。
int& MyVector::operator[](std::size_t index) { return data_[index]; }

const int& MyVector::operator[](std::size_t index) const { return data_[index]; }

// ---------------- 扩容：整个类的内存操作都收口在这里 ----------------
// 四步：申请新的 -> 搬旧数据 -> 释放旧的 -> 改指针与容量
void MyVector::reallocate(std::size_t new_cap) {
    int* new_data = new int[new_cap];  // ① 申请
    ++s_live_blocks;

    for (std::size_t i = 0; i < size_; ++i) {
        new_data[i] = data_[i];  // ② 只搬 size_ 个元素，不是 capacity_ 个
    }

    if (data_ != nullptr) {  // ③ 释放旧块（第一次扩容时旧块是空的）
        delete[] data_;
        --s_live_blocks;
    }

    data_ = new_data;  // ④ 换指针
    capacity_ = new_cap;

    // 注意：size_ 不变。扩容只改"能装多少"，不改"已经装了多少"。
}

// ---------------- 追加 ----------------
void MyVector::push_back(int value) {
    if (size_ == capacity_) {  // 只有满了才扩容
        // 容量从 0 开始：先给 1；以后每次翻倍。
        // 翻倍而不是每次 +1，才能让 N 次 push_back 的均摊复杂度是 O(1)。
        std::size_t new_cap = (capacity_ == 0) ? 1 : capacity_ * 2;
        reallocate(new_cap);
    }
    data_[size_] = value;  // 放在"第一个空位"上
    ++size_;
}

// ---------------- 预申请 ----------------
void MyVector::reserve(std::size_t new_cap) {
    // 只增不减：和 std::vector 一样，reserve 一个更小的值什么也不做。
    if (new_cap > capacity_) {
        reallocate(new_cap);
    }
}

// ---------------- 清空 ----------------
void MyVector::clear() {
    // 只把 size_ 归零，内存留着下次复用。
    // 这也是为什么 clear() 之后 capacity() 不变。
    size_ = 0;
}
