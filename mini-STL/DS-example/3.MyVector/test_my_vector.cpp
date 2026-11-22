// ============================================================
//  test_my_vector.cpp  ——  v0.3 测试（模板 + 迭代器）
//  注意：这个文件夹里没有 my_vector.cpp，构建时只编译本文件。
//  因为模板全部在 my_vector.h 里，include 进来就相当于把实现也带进来了。
// ============================================================
#include <algorithm>   // std::sort
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "my_vector.h"

// 一个"会记账"的元素类型：构造 +1，析构 -1。
// 用它来证明 delete[] 确实对每个元素调用了析构函数。
struct Tracked {
    static int alive;   // 当前还活着的 Tracked 对象个数

    int v;
    Tracked(int value = 0) : v(value) { ++alive; }
    Tracked(const Tracked& other) : v(other.v) { ++alive; }
    Tracked(Tracked&& other) noexcept : v(other.v) { ++alive; }
    Tracked& operator=(const Tracked& other) {
        v = other.v;
        return *this;
    }
    Tracked& operator=(Tracked&& other) noexcept {
        v = other.v;
        return *this;
    }
    ~Tracked() { --alive; }
};

int Tracked::alive = 0;

// 小工具：检查前 n 个元素
template <typename T>
static void check(const MyVector<T>& v, const T* expected, std::size_t n) {
    assert(v.size() == n);
    for (std::size_t i = 0; i < n; ++i) {
        assert(v[i] == expected[i]);
    }
}

int main() {
    // ================= A. int 版老功能回归 =================
    {
        MyVector<int> v;
        assert(v.empty() && v.capacity() == 0);

        v.push_back(10);
        v.push_back(20);
        v.push_back(30);
        int exp[] = {10, 20, 30};
        check(v, exp, 3);

        for (int i = 0; i < 100; ++i) v.push_back(i);
        assert(v.size() == 103 && v[0] == 10 && v[102] == 99);

        v.reserve(500);
        assert(v.capacity() >= 500 && v.size() == 103);

        v.clear();
        assert(v.empty() && v.capacity() >= 500);
    }
    assert(MyVector<int>::live_blocks() == 0);

    // ================= B. 拷贝 / 移动：计数器要分类型看 =================
    {
        MyVector<int>::reset_counters();

        MyVector<int> a;
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);

        MyVector<int> b = a;                  // 拷贝构造
        assert(MyVector<int>::copy_count() == 1);
        assert(MyVector<int>::move_count() == 0);
        b[0] = 99;
        assert(a[0] == 1);                    // 深拷贝：互不影响

        MyVector<int> c;
        c = b;                                // 拷贝赋值
        assert(MyVector<int>::copy_count() == 2);

        MyVector<int> d = std::move(a);        // 移动构造
        assert(MyVector<int>::move_count() == 1);
        assert(a.empty());                     // 源对象被置空
        assert(d.size() == 3 && d[0] == 1);
    }
    assert(MyVector<int>::live_blocks() == 0);

    // ================= C. 迭代器：手写循环 / 范围 for / 交给 STL 算法 =================
    {
        MyVector<int> v;
        for (int i = 0; i < 10; ++i) v.push_back(i * i);   // 0 1 4 9 ...

        // ① 手写迭代器循环（和用 vector 的写法一模一样）
        int sum = 0;
        for (MyVector<int>::iterator it = v.begin(); it != v.end(); ++it) {
            sum += *it;
        }
        assert(sum == 285);   // 0+1+4+...+81

        // ② 范围 for：编译器展开成上一种写法
        int sum2 = 0;
        for (int x : v) sum2 += x;
        assert(sum2 == 285);

        // ③ 迭代器就是指针，所以标准库算法直接能用
        std::sort(v.begin(), v.end(), [](int x, int y) { return x > y; });
        assert(v[0] == 81 && v[9] == 0);

        // ④ 空的 vector：begin() == end()，范围 for 一次都不进
        MyVector<int> empty_v;
        int count = 0;
        for (int x : empty_v) {
            (void)x;
            ++count;
        }
        assert(count == 0);
        assert(empty_v.begin() == empty_v.end());
    }
    assert(MyVector<int>::live_blocks() == 0);

    // ================= D. 元素是 std::string：模板真正的价值 =================
    {
        MyVector<std::string> v;
        v.push_back("hello");                 // 右值重载，走移动
        v.push_back(std::string("world"));

        assert(v.size() == 2);
        assert(v[0] == "hello" && v[1] == "world");

        MyVector<std::string> w = v;           // 深拷贝：字符串也是独立的
        w[0] = "changed";
        assert(v[0] == "hello");
        assert(w[0] == "changed");

        MyVector<std::string> m = std::move(w);
        assert(w.empty());
        assert(m[0] == "changed");

        // 字符串拼接这类操作，string 自己管内存，MyVector 只管"格子"
        m[1] += "!";
        assert(m[1] == "world!");
    }
    assert(MyVector<std::string>::live_blocks() == 0);

    // ================= E. 用 Tracked 证明析构函数真的被调用了 =================
    {
        {
            MyVector<Tracked> v;
            v.push_back(Tracked(1));
            v.push_back(Tracked(2));
            assert(Tracked::alive == 2);      // 两个元素

            MyVector<Tracked> w = v;          // 拷贝：又两个
            assert(Tracked::alive == 4);
            assert(w[0].v == 1 && w[1].v == 2);
        }
        assert(Tracked::alive == 0);          // 出了作用域，一个都不许剩
    }
    assert(MyVector<Tracked>::live_blocks() == 0);

    // ================= F. 和 std::vector 对照跑一遍同样的操作 =================
    {
        MyVector<int> mine;
        std::vector<int> theirs;

        for (int i = 0; i < 500; ++i) {
            int value = (i * 37 + 11) % 101;
            mine.push_back(value);
            theirs.push_back(value);
        }
        assert(mine.size() == theirs.size());

        bool same = true;
        for (std::size_t i = 0; i < theirs.size(); ++i) {
            if (mine[i] != theirs[i]) {
                same = false;
                break;
            }
        }
        assert(same);

        // 同样的 reserve / clear 行为
        mine.reserve(2000);
        theirs.reserve(2000);
        assert(mine.capacity() >= 2000 && theirs.capacity() >= 2000);

        mine.clear();
        theirs.clear();
        assert(mine.empty() && theirs.empty());
    }
    assert(MyVector<int>::live_blocks() == 0);

    // ================= G. at() 越界抛异常 =================
    {
        MyVector<int> v;
        v.push_back(5);
        assert(v.at(0) == 5);

        bool thrown = false;
        try {
            v.at(1);
        } catch (const std::out_of_range&) {
            thrown = true;
        }
        assert(thrown);
    }

    // ================= H. 静态计数器是"每个类型各一份" =================
    {
        MyVector<int>::reset_counters();
        MyVector<std::string>::reset_counters();

        MyVector<int> a;
        a.push_back(1);
        MyVector<int> b = a;                   // int 的拷贝 +1

        MyVector<std::string> c;
        c.push_back("x");
        MyVector<std::string> d = c;           // string 的拷贝 +1

        assert(MyVector<int>::copy_count() == 1);
        assert(MyVector<std::string>::copy_count() == 1);
    }
    assert(MyVector<int>::live_blocks() == 0);
    assert(MyVector<std::string>::live_blocks() == 0);

    std::cout << "all tests passed\n";
    return 0;
}
