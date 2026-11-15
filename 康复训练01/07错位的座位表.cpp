/*
错位的座位表

题目背景
新生校赛即将开始，机房里有n个座位，座位编号为1,2,3...n。恰好有n名同学，编号也为1,2,3...n
原本编号为i的同学应该坐在第i个座位。现在老师想安排一份特殊的座位表，用来考验大家的构造能力。

题目描述
给定两个整数 n,k请构造一个 1 到 n 的排列p，其中 pi表示坐在第 i 个座位的同学编号。
要求恰好有 k 名同学没有坐在自己编号对应的座位上，
也就是满足 pi≠i的位置恰好有 k 个。
如果有多个符合要求的排列，必须输出其中字典序最小的一个。如果不存在符合条件的排列，输出 -1。

字典序的定义：对于两个不同的排列，从左到右找到第一个不同的位置，那个位置上数字更小的排列，字典序更小。

样例
输入数据 1
7 5
输出数据 1
1 2 4 3 6 7 5
输入数据 2
4 1
输出数据 2
-1
输入数据 3
5 0
输出数据 3
1 2 3 4 5
*/
#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long n, k;
    cin >> n >> k;

    // k = 1 时不可能只有一个位置发生错位
    if (k < 0 || k > n || k == 1) {
        cout << -1 << '\n';
        return 0;
    }

    vector<long long> p(n + 1);
    for (long long i = 1; i <= n; ++i) {
        p[i] = i;
    }

    long long fixedCnt = n - k; // 固定点数量
    long long L = fixedCnt + 1; // 后缀起点

    if (k >= 2) {
        if (k % 2 == 0) {
            // 偶数个：相邻两两交换
            for (long long i = L; i <= n; i += 2) {
                swap(p[i], p[i + 1]);
            }
        } else {
            // 奇数个：前面两两交换
            for (long long i = L; i <= n - 3; i += 2) {
                swap(p[i], p[i + 1]);
            }

            // 最后三个循环错位
            long long a = n - 2;
            p[a]     = a + 1;
            p[a + 1] = a + 2;
            p[a + 2] = a;
        }
    }

    for (long long i = 1; i <= n; ++i) {
        if (i > 1) cout << ' ';
        cout << p[i];
    }
    cout << '\n';

    return 0;
}