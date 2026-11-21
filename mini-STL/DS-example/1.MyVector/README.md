# MyVector v1 参考实现 · 导读

> 位置：`code-refs/myvector-v1/`（放在规划中枢里只是方便你随时对照，正式工程请拷到 `F:\DSworkplace\code\mini-stl\`）
> 对应任务：`30-执行表-2026.11.14-12.14.md` 的 W2（v1 骨架 → tag `v0.1`）

---

## 1. 四个文件各管什么

| 文件 | 作用 | 你要从这里学什么 |
|---|---|---|
| `my_vector.h` | 声明：类长什么样、对外提供什么 | 声明与实现分离；`#pragma once`；`const` 成员函数 |
| `my_vector.cpp` | 实现：真正干活的地方 | 构造/析构如何配对；`new[]` 只出现在一个函数里 |
| `test_my_vector.cpp` | 测试：用 `assert` 断言行为 | 测试怎么写；8 个必测点 |
| `README.md` | 你现在读的这一份 | 讲清"为什么这么写"，不是"写了什么" |

## 2. 编译与运行

```powershell
cd F:\DSworkplace\CodeStudy01\code-refs\myvector-v1
g++ -std=c++17 -Wall -Wextra -g my_vector.cpp test_my_vector.cpp -o test.exe
.\test.exe
```

输出 `all tests passed` 才算过。**不要加 `-DNDEBUG`**——那会把所有 `assert` 删掉，测试变成空跑。

## 3. 三个必须记住的模型

### 模型一：一个对象 = 三个数字 + 一块内存

```
MyVector v;
v.push_back(1); v.push_back(2); v.push_back(3);

   data_ ──► [ 1 ][ 2 ][ 3 ][ ? ][ ? ]   ← 堆上一共 capacity_ = 5 个格子（举例）
             ├──── size_ = 3 ────┤
             └────────── capacity_ = 5 ──────────┘
```

- `size_` 是"放进去几个"，`capacity_` 是"申请了几个"。
- 只有 `size_ == capacity_` 时才扩容；扩容后 `capacity_` 变，`size_` 不变。

### 模型二：`new` / `delete` 只有三个合法位置

```
构造  → 不申请（空数组）
reallocate() → new[] 申请新块 + delete[] 释放旧块
析构  → delete[] 释放
```

写完自查：`grep -n "new\|delete" my_vector.cpp` 只应命中这几处。出现在 `push_back` 里，说明你在"边用边申请"，那是错的。

### 模型三：扩容的四步，顺序不能乱

1. `new int[new_cap]` —— 先有地方放
2. 把旧数据**逐个**搬到新块（只搬 `size_` 个）
3. `delete[]` 旧块
4. 指针和 `capacity_` 指向新块

**顺序反了会崩**：先 `delete[]` 再搬，就是读已释放内存。

## 4. 逐条回答"为什么"

| 代码 | 为什么这么写 | 换成别的会怎样 |
|---|---|---|
| 空构造不申请内存 | 大部分 vector 是空的，提前申请浪费 | 每个对象至少一次 `new`，还多一次释放 |
| 扩容 2 倍 | 让 N 次 `push_back` 均摊 O(1) | 每次 +1 → 总复杂度 O(N²) |
| 只搬 `size_` 个 | `capacity_ - size_` 个格子是脏数据，搬了没意义，还更慢 | 搬全部 → 慢且读到未初始化值 |
| `reserve` 只增不减 | 和标准库一致，避免误缩容引发重新分配 | 传小值就缩容 → 迭代器/指针突然失效 |
| `clear()` 不释放 | 保留容量，下次 push 不用重新申请 | 每次都重新申请 → 频繁 malloc |
| `operator[]` 不检查越界 | 与 `std::vector` 一致，主打性能；要安全用 `at()` | 加检查 → 每次访问多一次比较（下一版做 `at()`，不是改 `[]`） |
| `delete[] nullptr` | 标准保证是空操作 | 但账本 `s_live_blocks` 需要判空，否则计数变负 |

## 5. 闭卷重写前的 8 个自问

写完关上文件，逐条问自己，答不上来就是没懂：

1. `size_` 和 `capacity_` 分别什么时候变？
2. 一次扩容里，一共发生了几次 `new`、几次 `delete`？
3. 为什么第一次 `push_back` 后 `capacity()` 是 1，第二次是 2，第三次是 4？
4. `reserve(100)` 之后 `size()` 为什么不变？
5. `clear()` 之后 `capacity()` 为什么不变？
6. 如果忘了搬 `data_[0]`，哪个测试会挂？
7. 如果 `delete[]` 写成 `delete`，会发生什么？（提示：`int` 没有析构函数，可能不崩，但换成一个有析构函数的类就泄漏）
8. `const MyVector&` 类型的对象能不能调 `operator[]`？为什么需要两个重载？

## 6. 下一版（v0.2）要加的东西 —— 只给规格，自己写

1. **拷贝构造** `MyVector(const MyVector& other)`：深拷贝，两个对象的 `data_` 必须不同。
2. **拷贝赋值** `MyVector& operator=(const MyVector& other)`：先判自赋值 `this == &other`，再申请→拷贝→释放旧块。
3. **移动构造/移动赋值**：`MyVector(MyVector&& other)` 偷指针，然后把 `other.data_ = nullptr; other.size_ = other.capacity_ = 0;`
4. **`at(n)`**：越界时 `throw std::out_of_range`。
5. **`begin()/end()`**：先用 `int*` 当迭代器，能配合范围 for 用即可。
6. **计数器**：`static int copy_count / move_count`，用测试证明 `push_back` 一个临时对象走的是移动不是拷贝。

**验证标准**：加完后这 8 个用例仍全绿 + 新增 4 个用例（自赋值、拷贝后互不影响、移动后源对象为空、拷贝次数符合预期）。

## 7. 使用纪律（重要）

1. **先看一遍，然后关掉文件**，不要开着对照抄。
2. 闭卷重写，卡住超过 **20 分钟**再看一眼对应函数，然后立刻关掉继续。
3. 重写版必须能过 `-Wall -Wextra` 零警告 + 测试全绿，才算这一版完成。
4. 重写完对照一次，重点看：你有没有把 `new` 放到 `push_back` 里？有没有忘搬数据？
5. 打完 tag `v0.1` 再进入 v0.2，不要"边写边补测试"。
