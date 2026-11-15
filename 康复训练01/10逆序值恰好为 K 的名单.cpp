/*
逆序值恰好为 K 的名单

题目背景
新生校赛的选手编号为 
1,2,…,n。主办方想把这些编号排列成一份特殊的名单，用于展示逆序对的概念。

题目描述
请构造一个 1 到 n 的排列 p，使它的逆序对数量恰好为 k。
如果有多种符合要求的排列，输出其中字典序最小的一个。
当且仅当 i<j 且 pi>pj 时，(i,j) 构成一个逆序对。排列是指长度为 n，
且数字 1到 n 恰好各出现一次的序列。
字典序比较：从左往右寻找第一个不同位置，在该位置数字较小的排列更小。
输入保证 k 在合法范围内，因此一定有解。

输入格式
一行两个整数 n,k。

输出格式
输出一行 n 个空格分隔的整数，表示符合要求且字典序最小的排列。

样例 1
输入数据 1
4 3
输出数据 1
1 4 3 2

样例 2
输入数据 2
1 0
输出数据 2
1

样例解释
样例 1中，[1,4,3,2]恰有(2,3)、(2,4) 和 (3,4) 三个逆序对。

逆序对说明
对于一个排列 p，如果存在两个位置 i,j满足：i<j且pi>pj那么称 (i,j)(i,j) 为一个逆序对。
简单来说，就是前面的数字比后面的数字大。
*/

/*
思路：

假设当前处理到第 i 位，后面还剩：m=n−i个位置。

如果第 i 位放当前最小的数 i：
    它不会和后面的数产生逆序对；
    后面 m 个数最多产生：m(m−1)/2个逆序对。

所以分两种情况：

如果 k <= m(m-1)/2
第 i 位放 i，剩下的逆序对完全可以在后面构造出来。
因为 i 是当前能放的最小数，所以直接放它最优。

如果 k > m(m-1)/2
第 i 位放 i 已经不够产生 k 个逆序对了。
后面最多贡献 m(m-1)/2，所以第 i 位本身至少要贡献：
r=k−m(m−1)/2个逆序对。
把剩下的数从小到大看，选择第 r+1 小的数，也就是：
i+r。
它后面恰有 r 个比它小的数，因此贡献 r 个逆序对。
此时剩下的逆序对数正好等于后缀的最大值，所以后面的数必须严格降序排列。
这样每一步都取了当前能取的最小数，因此得到的就是字典序最小的排列。

*/

#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long n, k;
    cin >> n >> k;

    vector<long long> ans;
    ans.reserve(n);

    for (long long i = 1; i <= n; ++i) {
        long long m = n - i;
        long long maxSuffix = m * (m - 1) / 2;

        if (k <= maxSuffix) {
            // 放当前最小的数，后面仍然可以凑出 k 个逆序对
            ans.push_back(i);
        } else {
            // 必须在这一位产生 r 个逆序对
            long long r = k - maxSuffix;
            long long pivot = i + r;

            ans.push_back(pivot);

            // 剩下的数全部降序，产生最大逆序对
            for (long long v = n; v >= i; --v) {
                if (v != pivot) {
                    ans.push_back(v);
                }
            }
            break;
        }
    }

    for (long long i = 0; i < (long long)ans.size(); ++i) {
        if (i > 0) cout << ' ';
        cout << ans[i];
    }
    cout << '\n';

    return 0;
}
/*
时间复杂度 O(n)
空间复杂度 O(n)
*/