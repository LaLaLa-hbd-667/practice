//动态数组的熟悉
/*
#include <vector>

vector<int> v;          // 创建一个空动态数组
v.push_back(10);        // 在末尾加元素
v.pop_back();           // 删掉末尾元素

v.size();               // 获取元素个数
v.empty();              // 判断是否为空 (推荐用，比 size()==0 快)
v.clear();              // 清空所有元素

v[0];                   // 像数组一样访问（不检查越界）
v.at(0);                // 像数组一样访问（越界会抛出异常，更安全）

// 如果想在某处插入或删除（效率较低，O(n)）
v.insert(v.begin() + 1, 100); // 在下标1处插入100
v.erase(v.begin() + 1);       // 删除下标1处的元素
*/

//MAX版-约瑟夫环的数学方法运用
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int n, m;
    cin >> n >> m;
    vector<int> v;
    for (int i = 0; i < n; ++i) {
        v.push_back(i + 1); // 存入编号 1 到 n
    }

    int i = 0; // 报数的起点
    
    // 只要数组里还有人，就继续
    while (!v.empty()) {
        // 1. 直接算出这次要被淘汰的人的下标（这就是你说的“巧妙”）
        //知道m了，于是第i+m-1个人就是要删掉的人，超一圈的人数了怎么办?——取余！%一圈的人数就行
        i = (i + m - 1) % v.size();
        
        // 2. 输出他
        printf("%d OUT\n", v[i]);
        
        // 3. 把他删掉
        v.erase(v.begin() + i);
        
        // 4. 关键点：删掉之后，不需要 i-- 吗？
        // 因为刚刚删掉了位置 i 的人，后面的人顶上来了。
        // 此时 i 正好指向了下一个人，而下一次报数，就是从这个人开始算“1”。
        // 所以下一轮开头，i 已经是正确的起始位置了，不需要任何移动！
    }

    return 0;
}

/*
//第二版-简单运用动态数组函数-仍使用j作为计数的方法
#include<iostream>
#include<vector>
using namespace std;
int main(){
    int n,m;
    vector<int> v;
    cin>>n>>m;
    for(int i=0;i<n;++i){
        v.push_back(i+1);
    }
    int i=0,j=0;
    while(!v.empty()){
        ++j;
        if(j==m){
            printf("%dOUT ",v[i]);
            v.erase(v.begin() + i);     //删掉v[i]
            j=0;
            i--;                    //删掉后指针回退
        }
        ++i;
        if(i==v.size()){
            i=0;
        }
    }

    return 0;
}
*/

/*
//第一版-功能实现
#include<iostream>
#include<vector>
using namespace std;

int main(){
    int n,m,num;
    vector<int> v;
    cin>>n>>m;
    for(int i=0;i<n;++i){
        v.push_back(1);
        v[i]=i+1;
    }
    int i=0,j=0;
    num=n;
    while(num!=0){
        if(v[i]!=0){
            ++j;
        }
        if(j==m){
            printf("%dOUT\n",v[i]);
            v[i]=0;
            num--;
            j=0;
        }      
        ++i;
        if(i==n){
            i=0;
        }       
    }
    for(int i=0;i<n;++i){
        
        cout<<v[i]<<"  ";
    }

    return 0;
}
*/