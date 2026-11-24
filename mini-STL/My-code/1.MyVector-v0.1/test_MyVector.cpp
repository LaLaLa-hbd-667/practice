#include <cassert>
#include <iostream>

#include "MyVector.h"

int main() {
    // ---- 0. 内存账本：刚启动时没有活着的堆块 ----
    assert(MyVector::live() == 0);
    {
        MyVector tmp;
        tmp.push(1);
        assert(MyVector::live() == 1);  // 恰好一块
    }
    assert(MyVector::live() == 0);  // 出了作用域就该还回去

    // ---- 1. 空数组的初始状态 ----
    MyVector v;
    assert(v.empty());
    assert(v.size() == 0);
    assert(v.capacity() == 0);

    // ---- 2. 追加 + 下标读 ----
    v.push(10);
    v.push(20);
    v.push(30);
    assert(v.size() == 3);
    assert(v[0] == 10 && v[1] == 20 && v[2] == 30);
    assert(v.capacity() >= 3);  // 具体是 3 还是 4 由实现决定，>= 3 就算对

    // ---- 3. 扩容后旧数据必须完整（最容易写错的地方） ----
    for (int i = 0; i < 100; ++i) {
        v.push(i);
    }
    assert(MyVector::live() == 1);        // 扩容 8 次，堆上仍只该有 1 块
    assert(v.size() == 103);
    assert(v[0] == 10 && v[1] == 20 && v[2] == 30);
    assert(v[3] == 0 && v[102] == 99);

    // ---- 4. reserve：改容量，不改元素 ----
    MyVector w;
    w.push(7);
    w.reserve(100);
    assert(w.size() == 1);
    assert(w[0] == 7);
    assert(w.capacity() >= 100);

    // ---- 5. reserve 一个更小的值：不缩容 ----
    const std::size_t cap = w.capacity();
    w.reserve(2);
    assert(w.capacity() == cap);

    // ---- 6. clear：清元素，留内存 ----
    w.clear();
    assert(w.empty());
    assert(w.capacity() >= 100);

    
    w.clear(); w.push(42);                // clear 之后还能不能接着用
    assert(w.size() == 1 && w[0] == 42);
    
    std::cout << "all tests passed\n";
    return 0;
}
