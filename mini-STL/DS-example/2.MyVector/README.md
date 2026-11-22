# MyVector v0.2 —— 五大件版（第 2 步）

> 第 1 步是 `1.MyVector`（v0.1：能存能取能扩容）。
> 这一步只加一件事：**对象之间怎么"复制"和"搬家"才不出事**。

---

## 1. 从 v1 到 v0.2，多了什么

| 新增 | 一句话作用 |
|---|---|
| `MyVector(const MyVector&)` 拷贝构造 | `MyVector b = a;` 时，给 b 造一块**新的**内存，把数据搬过去 |
| `MyVector& operator=(const MyVector&)` 拷贝赋值 | `c = a;` 时，先处理掉 c 的旧内存，再深拷贝 |
| `MyVector(MyVector&&)` 移动构造 | `MyVector d = std::move(a);` 时，**直接偷走** a 的指针，不拷贝 |
| `MyVector& operator=(MyVector&&)` 移动赋值 | `e = std::move(a);` 时，先还掉 e 的旧内存，再偷 a 的 |
| `at(n)` | 越界抛 `std::out_of_range`（`[]` 仍然不检查，保持快） |
| 计数器 `copy_count()/move_count()` | 用数字证明"到底是拷贝还是移动"，不靠猜 |

`~MyVector()` 还是原来那个，不算新增——**五大件 = 析构 + 拷贝构造 + 拷贝赋值 + 移动构造 + 移动赋值**。

## 2. 为什么 v1 那样写会崩（先看崩溃，再看代码）

v1 里没写拷贝构造，编译器就**自动生成一个**，它只会做"逐成员复制"：

```cpp
MyVector a; a.push_back(1);
MyVector b = a;   // b.data_ 被复制成 a.data_ —— 两个对象指向同一块内存！
                  // 程序结束时两个析构函数各 delete[] 一次 → double free
```

这个 bug 的可怕之处：**可能不崩**。`int` 没有析构函数，`delete[]` 两次在部分平台只是"看起来没事"，等你换成 `string` 就炸。

所以规则是：**只要类里手工管理了一块内存（有 `new[]`/`delete[]`），就必须自己写五大件。**

## 3. 四个函数的三句口诀

```
拷贝构造 = 申请新的 + 逐个搬 + 源对象不动
移动构造 = 接指针 + 把源对象置空
拷贝赋值 = 先建新的 + 再拆旧的 + 换指针
移动赋值 = 先拆旧的 + 再接指针 + 把源对象置空
```

### 3.1 拷贝赋值为什么是"先建后拆"，不是"先拆后建"

```cpp
// 错误写法：先 delete 自己，再去读 other
delete[] data_;
data_ = new int[other.size_];
for (...) data_[i] = other.data_[i];
```
如果 `other` **就是自己**（`a = a;`），你先把内存删了，下一行读的就是已释放内存。
"先建后拆"顺手解决了自赋值，也让 new 抛异常时 `*this` 保持原样。

代码里还是加了一句自赋值判断（`if (this == &other) return *this;`）——它能让 `a = a` **完全不做无用功**（测试 D 断言了地址不变）。

### 3.2 移动之后，源对象必须"空"

```cpp
other.data_ = nullptr;
other.size_ = other.capacity_ = 0;
```
少了这三行，a 和 d 都指向同一块内存，析构时 double free。
移动后的对象处于"有效但未指定"状态；我们这个实现把它变成空 vector，所以还能继续 `push_back`（测试 E 验证了这一点）。

### 3.3 `noexcept` 不是装饰

移动构造/移动赋值标 `noexcept`，标准库才敢在扩容时用它搬元素；不标就退回拷贝，你就白写了。

## 4. 怎么跑

VSCode 打开 `2.MyVector` 文件夹：

- `Ctrl + Shift + B`：构建（调用 `.vscode/tasks.json`，产出 `test_my_vector.exe`）
- 终端运行 `.\test_my_vector.exe`，看到 `all tests passed` 即通过
- `F5`：断点调试（`launch.json` 已配好 gdb）

命令行也行：

```powershell
g++ -Wall -Wextra -g my_vector.cpp test_my_vector.cpp -o test_my_vector.exe
.\test_my_vector.exe
```

## 5. 测试清单（A–I 都在 `test_my_vector.cpp` 里）

| 编号 | 测什么 | 想验证的点 |
|---|---|---|
| A | v1 老功能 | 回归：加了五大件不能弄坏原来的行为 |
| B | 拷贝构造 | 深拷贝：改 b 不影响 a；两块内存 |
| C | 拷贝赋值 | 旧内存被释放（`live_blocks` 不涨） |
| D | 自赋值 | `a = a` 不崩、不重新分配 |
| E | 移动构造 | 指针相同 = 没拷贝；源对象变空；还能继续用 |
| F | 移动赋值 | 自己的旧内存被释放 |
| G | 自移动赋值 | `a = std::move(a)` 不把自己掏空 |
| H | `at()` | 越界抛 `out_of_range` |
| I | 移动后再操作 | 两个对象互不干扰 |

`live_blocks()` 在每一段结束都断言归零——**这就是你的泄漏检测器**。

## 6. 你可能踩的坑

- 移动构造里忘写 `other.data_ = nullptr` → double free
- 拷贝赋值里忘了返回值（`return *this;`）→ `a = b = c` 链式赋值出错
- 拷贝构造参数写成 `MyVector(MyVector other)`（值传递）→ 无限递归，栈溢出
- 拷贝构造里 `new int[capacity_]` 但只搬了 `size_` 个 → 能跑，但白占内存（标准库也这么做，可以接受）
- 给 `at()` 加了检查却忘了 `[]` 保持不检查 → 性能白白变差
- 测试里加 `-DNDEBUG` → assert 全消失，测试形同虚设

## 7. 第 3 步（v0.3）要做什么

1. 把 `int` 换成 `template <typename T>`：**全部实现搬进头文件**（模板不能拆 .cpp，这一步你会亲身撞上）。
2. 加迭代器 `begin()/end()`：先用 `T*` 当迭代器，让范围 for 能跑。
3. 用 `std::vector` 写一份对照测试：同样的操作，结果一致。

到那时再回来改这份 README，不要提前动手。
