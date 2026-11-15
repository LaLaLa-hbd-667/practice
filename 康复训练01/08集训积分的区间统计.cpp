/*
集训积分的区间统计

题目背景
为了让新生了解自己的训练情况，算法协会记录了每位同学连续 n 天的积分变化。训练表现好时积分可能增加，状态不好时积分也可能减少。
组织者会频繁查询某一段连续日期的积分变化总和。

题目描述
给定长度为 n 的整数序列 a_1, a_2, \dots, a_n，其中 a_i 表示第 i 天的积分变化。
接下来有 q 次查询，每次给出两个整数 l, r (1 \le l \le r \le n)，请计算第 l 天到第 r 天（包括两端）的积分变化总和：
\sum_{i=l}^{r} a_i. 
所有查询互相独立，不会改变任何一天的数据。积分变化可以为负数，也可以为 0。

输入格式
第一行两个整数 n, q。
第二行 n 个整数 a_1, a_2, \dots, a_n。
接下来 q 行，每行两个整数 l, r，表示一次区间查询。

输出格式
对于每次查询，输出一行整数，表示该区间内的积分变化总和。

样例
输入数据 1
6 4
3 -2 5 0 -7 10
1 3
2 5
4 4
1 6
输出数据 1
6
-4
0
9

*/
//前缀和
#include <iostream>
#include <vector>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;

    // pre[i] 表示前 i 个数之和，开 long long
    vector<long long> pre(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        long long x;
        cin >> x;
        pre[i] = pre[i - 1] + x;
    }

    while (q--) {
        int l, r;
        cin >> l >> r;
        cout << pre[r] - pre[l - 1] << "\n";
    }
    return 0;
}