/*
恰好达标的学习计划

题目背景
校赛集训时，每一天的训练进度可能超额完成，也可能欠账。因此用一个可以为负数的整数表示当天的净进度。

题目描述
给定长度为 n 的整数序列 a_1, a_2, \dots, a_n 和一个目标整数 k。
定义一段连续的日期区间 [l, r] (1 \le l \le r \le n) 的总进度为
\sum_{i=l}^{r} a_i. 
请计算总进度恰好等于 k 的非空连续区间个数。
不同的 (l, r) 视为不同区间。序列中允许负数和 0。

输入格式
第一行两个整数 n, k。
第二行 n 个整数 a_1, a_2, \dots, a_n。

输出格式
输出一个整数，表示符合条件的非空连续区间数量。


样例
输入数据 1
6 3
1 2 -1 3 0 1

输出数据 1
4

样例解释
符合条件的区间是 
[1,2]、[3,6]、[4,4] 和 [4,5]。

*/

/*
核心思路：前缀和 + 哈希表

把这一列数从左到右遍历每一个端点
每个端点都可以作为右端点 r，计算出前缀和 S[r]，
然后去找有多少个左端点 l 使得 S[r] - S[l-1] = k，也就是 S[l-1] = S[r] - k。

注意
cnt[0] = 1：表示空前缀 S[0]，这样从位置 1 开始的区间也能被统计到。
每次要先查询，再把当前前缀和插入哈希表，否则 k=0 时会把空区间也算进去。
答案可能很大，用 long long。
前缀和也可能很大，同样用 long long。

*/


#include <iostream>
#include <unordered_map>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    long long k;
    cin >> n >> k;

    std::unordered_map<long long, long long> cnt;
    cnt.reserve((size_t)n * 2 + 1);
    cnt.max_load_factor(0.7f);

    // 空前缀 S[0] = 0
    cnt[0] = 1;

    long long prefix = 0;
    long long ans = 0;

    for (int i = 0; i < n; ++i) {
        long long x;
        cin >> x;
        prefix += x;

        // 需要之前出现过 prefix - k 个前缀和
        auto it = cnt.find(prefix - k);
        if (it != cnt.end()) {
            ans += it->second;
        }

        // 当前前缀和加入哈希表
        ++cnt[prefix];
    }

    cout << ans << '\n';
    return 0;
}
/*
时间复杂度：平均 O(n)
空间复杂度：O(n)
*/