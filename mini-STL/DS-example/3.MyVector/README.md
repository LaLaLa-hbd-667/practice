# MyVector v0.3 —— 模板 + 迭代器版（第 3 步）

> 第 1 步（`1.MyVector`）：能存能取能扩容。
> 第 2 步（`2.MyVector`）：五大件，拷得对、移得对。
> **这一步：一个类变成"一类"类——`MyVector<int>`、`MyVector<std::string>`、`MyVector<任何东西>`。**

---

## 1. 最显眼的变化：**这个文件夹里没有 my_vector.cpp**

打开目录你会发现只有 `my_vector.h` 和 `test_my_vector.cpp`。这不是偷懒，是语言规则：

- 模板的成员函数**只有 T 确定时才会生成真正的代码**（这叫实例化）；
- `.cpp` 是单独编译的，编译 `my_vector.cpp` 时编译器不知道 `T` 是什么，于是**什么都不生成**；
- 等到链接时，`test` 里调用了 `MyVector<int>::push_back`，却找不到实现 → 链接错误：

```
undefined reference to `MyVector<int>::push_back(int const&)'
```

**所以模板类的实现必须和声明写在同一个头文件里。** 想亲自验证这个坑：

1. 把 `my_vector.h` 里 `push_back` 的实现剪到 `my_vector.cpp`，并在那里 `#include "my_vector.h"`；
2. 把 `test_my_vector.cpp` 也 `#include "my_vector.h"`，两个文件都编译；
3. 你会看到上面那条 undefined reference。看完记得改回来。

> 这个实验只花 10 分钟，但能一次性治掉"为什么模板不能拆 .cpp"这个万年疑惑。

## 2. 从 v0.2 到 v0.3 的四处改动

| 改动 | 内容 |
|---|---|
| 加模板 | `class MyVector` → `template <typename T> class MyVector` |
| 类型别名 | `value_type` / `iterator` / `const_iterator` |
| 迭代器 | `begin()/end()`，各两个版本（可写 + 只读），范围 for 就能用了 |
| `push_back` 两个重载 | `const T&`（左值，拷贝）和 `T&&`（右值，移动） |

五大件的逻辑**一行都没改**，只是把 `int` 换成了 `T`。这说明：拷贝/移动的规则跟类型无关，它管的是"资源"。

## 3. 迭代器为什么用 `T*` 就够

范围 for `for (int x : v)` 会被编译器展开成：

```cpp
{
    auto __begin = v.begin();
    auto __end   = v.end();
    for (; __begin != __end; ++__begin) {
        int x = *__begin;
        // 循环体
    }
}
```

它只要求迭代器能做四件事：**`*` 取值、`++` 前进、`!=` 比较、`=` 赋值**。指针全部满足，所以 `T*` 就是一个合法迭代器。

副作用（好事）：标准库算法直接能用——测试 C 里 `std::sort(v.begin(), v.end())` 就是靠这个。

> 空 vector 的 `begin() == end() == nullptr`，范围 for 一次都不进，不会出问题。

## 4. 静态成员是"每个 T 各一份"

```cpp
MyVector<int>::live_blocks();          // int 版的一个变量
MyVector<std::string>::live_blocks();  // string 版的另一个变量
```

测试 H 就是在验证这一点：给 int 版做一次拷贝、给 string 版做一次拷贝，两边计数器各自是 1，互不干扰。

定义也必须放头文件里：
```cpp
template <typename T> int MyVector<T>::s_live_blocks = 0;   // 不能放 .cpp
```

## 5. 测试 A–H 在测什么

| 编号 | 内容 | 关键断言 |
|---|---|---|
| A | int 版回归 | 老功能不能坏 |
| B | 拷贝/移动 + 计数 | `copy_count()==1`、移动后源为空 |
| C | 迭代器三连 | 手写循环 / 范围 for / `std::sort` 都能用 |
| D | 元素是 `std::string` | 深拷贝后改副本不影响原对象 |
| E | `Tracked` 记账类型 | 作用域结束后 `alive == 0`：析构函数确实逐个调用了 |
| F | 与 `std::vector` 对照 | 同样 500 次 push，元素逐一相等 |
| G | `at()` 越界 | 抛 `std::out_of_range` |
| H | 计数器分类型 | int 和 string 各数各的 |

## 6. 这版故意做的简化（知道就行，不用现在补）

1. **`new T[n]` 要求 T 能默认构造**：它会先默认构造 n 个元素再赋值。真正的 `std::vector` 用 allocator + placement new，只构造 `size` 个，不需要默认构造。
2. **扩容时元素用拷贝搬，不是移动**：真实实现会用 `std::move_if_noexcept`，让"移动不抛异常"的类型走移动。
3. **没有 `pop_back / insert / erase / resize`**：等你手写链表时自然会需要，到时候再加。
4. **迭代器没有类型检查**：`MyVector<int>::iterator` 和 `MyVector<double>::iterator` 都是指针，混用编译器不管（真实实现用类包装）。

## 7. 怎么跑

这个文件夹只有 `test_my_vector.cpp` 需要编译：

```powershell
g++ -Wall -Wextra -g -finput-charset=UTF-8 -fexec-charset=UTF-8 test_my_vector.cpp -o test_my_vector.exe
.\test_my_vector.exe
```

VSCode 里：`Ctrl + Shift + B` 构建（`tasks.json` 已改成只编译测试文件），`F5` 调试。

## 8. 下一步（第 4 步）：`MyString`

在 `4.MyString` 里写自己的字符串类，练的是：

- `char*` + 结尾 `'\0'` 的手工管理（比 vector 更容易踩坑：忘记补 `'\0'`）
- `operator+` / `operator+=` / `operator==` / `c_str()`
- 拷贝走深拷贝、移动可偷指针——**五大件的逻辑直接从 MyVector 搬过来**，你会发现几乎不用重想。

写 `MyString` 之前先回来把 `MyVector` 的这五件闭卷重写一遍，确认"拷贝=新建+搬、移动=偷+置空"已经变成本能。
