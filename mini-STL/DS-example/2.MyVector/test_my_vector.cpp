// ============================================================
//  test_my_vector.cpp  ——  v0.2 测试（五大件 + at）
//  运行方式：VSCode 里 Ctrl+Shift+B 构建，然后运行 test_my_vector.exe
//  注意：不要加 -DNDEBUG，否则 assert 全被删掉，测试等于没跑。
// ============================================================
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "my_vector.h"

// 小工具：检查 v 的前 n 个元素是不是 expected
static void check(const MyVector& v, const int* expected, std::size_t n) {
    assert(v.size() == n);
    for (std::size_t i = 0; i < n; ++i) {
        assert(v[i] == expected[i]);
    }
}

int main() {
    // ================= A. v1 的老功能不能坏（回归测试） =================
    {
        MyVector v;
        assert(v.empty() && v.size() == 0 && v.capacity() == 0);

        v.push_back(10);
        v.push_back(20);
        v.push_back(30);
        int exp1[] = {10, 20, 30};
        check(v, exp1, 3);

        for (int i = 0; i < 100; ++i) v.push_back(i);  // 反复扩容
        assert(v.size() == 103);
        assert(v[0] == 10 && v[2] == 30 && v[3] == 0 && v[102] == 99);

        MyVector w;
        w.push_back(7);
        w.reserve(100);
        assert(w.size() == 1 && w[0] == 7 && w.capacity() >= 100);

        w.clear();
        assert(w.empty() && w.capacity() >= 100);
    }
    assert(MyVector::live_blocks() == 0);   // 出了作用域，内存必须全部还干净

    // ================= B. 拷贝构造：深拷贝 =================
    {
        MyVector::reset_counters();

        MyVector a;
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        assert(MyVector::live_blocks() == 1);

        MyVector b = a;                     // 拷贝构造
        int exp[] = {1, 2, 3};
        check(b, exp, 3);

        assert(MyVector::copy_count() == 1);   // 确实走的是拷贝构造
        assert(MyVector::move_count() == 0);
        assert(MyVector::live_blocks() == 2);  // 两块内存：各管各的

        b[0] = 99;                          // 改副本
        assert(a[0] == 1);                  // 原对象不受影响 → 说明是深拷贝
        assert(&a[0] != &b[0]);             // 指针不同，不是同一块内存
    }
    assert(MyVector::live_blocks() == 0);

    // ================= C. 拷贝赋值：会释放自己的旧内存 =================
    {
        MyVector::reset_counters();

        MyVector a;
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);

        MyVector c;
        c.push_back(42);
        c.push_back(43);                   // c 原来有数据，占一块内存

        c = a;                             // 拷贝赋值
        int exp[] = {1, 2, 3};
        check(c, exp, 3);
        assert(MyVector::copy_count() == 1);
        assert(MyVector::live_blocks() == 2);  // 旧块已释放：a 一块 + c 一块，没有泄漏
        assert(&c[0] != &a[0]);
    }
    assert(MyVector::live_blocks() == 0);

    // ================= D. 自赋值：a = a 不能崩 =================
    {
        MyVector a;
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);

        const int* before = &a[0];
        a = a;                             // 没有自赋值判断的版本会在这里崩
        int exp[] = {1, 2, 3};
        check(a, exp, 3);
        assert(&a[0] == before);           // 地址没变，说明根本没重新分配
    }
    assert(MyVector::live_blocks() == 0);

    // ================= E. 移动构造：偷指针，不拷贝 =================
    {
        MyVector::reset_counters();

        MyVector a;
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        int* p = &a[0];

        MyVector d = std::move(a);         // 移动构造
        assert(&d[0] == p);                // 还是原来那块内存，一次拷贝都没发生
        assert(MyVector::move_count() == 1);
        assert(MyVector::copy_count() == 0);

        int exp[] = {1, 2, 3};
        check(d, exp, 3);

        assert(a.empty());                 // 源对象被置空
        assert(a.capacity() == 0);
        assert(MyVector::live_blocks() == 1);  // 内存换了主人，总数还是 1

        a.push_back(7);                    // 被移动过的对象必须还能继续用
        assert(a.size() == 1 && a[0] == 7);
    }
    assert(MyVector::live_blocks() == 0);

    // ================= F. 移动赋值：先还自己的，再接对方的 =================
    {
        MyVector::reset_counters();

        MyVector a;
        a.push_back(1);
        a.push_back(2);

        MyVector e;
        e.push_back(9);
        e.push_back(9);
        e.push_back(9);                    // e 占一块内存

        e = std::move(a);                  // 移动赋值
        int exp[] = {1, 2};
        check(e, exp, 2);
        assert(a.empty());
        assert(MyVector::move_count() == 1);
        assert(MyVector::live_blocks() == 1);  // e 的旧块被释放，a 的块转给 e
    }
    assert(MyVector::live_blocks() == 0);

    // ================= G. 自移动赋值：a = std::move(a) 不能崩 =================
    {
        MyVector a;
        a.push_back(5);
// 编译器看到 a = std::move(a) 会提醒"自己移动自己"，这里故意这么写，关掉这条警告
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
        a = std::move(a);                  // 没有判断的话：先 release() 再偷自己 → 全空
#pragma GCC diagnostic pop
        assert(a.size() == 1 && a[0] == 5);
    }
    assert(MyVector::live_blocks() == 0);

    // ================= H. at()：越界抛异常，[] 不抛 =================
    {
        MyVector v;
        v.push_back(5);

        assert(v.at(0) == 5);

        bool thrown = false;
        try {
            v.at(1);                       // 越界
        } catch (const std::out_of_range&) {
            thrown = true;
        }
        assert(thrown);
    }

    // ================= I. 移动之后再扩容，源对象不受影响 =================
    {
        MyVector a;
        for (int i = 0; i < 10; ++i) a.push_back(i);

        MyVector b = std::move(a);
        for (int i = 0; i < 10; ++i) b.push_back(100 + i);

        assert(b.size() == 20 && b[19] == 109);
        assert(a.empty());
    }
    assert(MyVector::live_blocks() == 0);

    std::cout << "all tests passed\n";
    return 0;
}
