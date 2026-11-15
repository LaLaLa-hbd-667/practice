//MAX版-任意数量整数排序
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream> // 用于处理字符串流

using namespace std;

// 1. 使用 vector 引用传参，彻底解决长度问题
// 传引用（&）避免拷贝，提升性能，同时 vector 自己知道长度 (arr.size())
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    // 外层循环：n个元素需要 n-1 趟
    for (int i = 0; i < n - 1; ++i) {
        // 内层循环：每趟比较次数逐渐减少
        for (int j = 0; j < n - 1 - i; ++j) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]); // 直接使用标准库的 swap
            }
        }
    }
}

int main() {
    vector<int> nums; // 建立一个空的动态数组
    string line;

    //cout << "请输入要排序的整数（用空格隔开），按回车结束：" << endl;
    
    // 读取一整行输入（直到你按下回车）
    getline(cin, line);
    
    // 将这一行字符串转化为流
    stringstream ss(line);
    
    int num;
    // 从流中一个一个读取数字，存入 nums 中
    while (ss >> num) {      //>>是流提取运算符，编译器看到ss>>num时会先看num是说明类型然后启动解析整数模式，从ss这个字符池子跳过开头的空格和回车开始抓取字符，发现数字就继续抓，碰到空格或非数字字符就停止抓取。
                             //ss成功吐出一个数字塞给num时，操作成功，while认为条件为真；ss吐完了，ss>>num失败了，就返回false
        nums.push_back(num); // push_back 表示把数字塞进数组的末尾
    }

    // 调用函数，只需要传数组本身，不需要传长度了
    bubbleSort(nums);

    // 打印结果（使用 C++11 的增强 for 循环，非常简洁）
    //cout << "排序后的结果：";
    for (int val : nums) { 
        cout << val << " ";
    }
    cout << endl;

    return 0;
}

/*
//第二版 固定数量整数排序-swap函数的直接调用&标准命名
#include<iostream>
#include<algorithm>
using namespace std;
void bubbleSort(int *a){
    for(int i=0;i<=3;++i){
        for(int j=0;j<=4-i;++j){
            if(a[j]>a[j+1]){
                swap(a[j],a[j+1]);
            }
        }
    }
}
int main(){
    int nums[5];
    for(int i=0;i<=4;++i){
        cin>>nums[i];
    }
    bubbleSort(nums);   
    for(int i=0;i<=4;++i){
        cout<<nums[i]<<" ";
    }
    cout<<endl;
    return 0;
}
*/

/*
//第一版 固定数量整数排序-功能实现

#include<iostream>
using namespace std;
void swap(int &a,int &b){
    a+=b;
    b=a-b;
    a-=b;
}
void pop(int *a){
    for(int i=1;i<=4;++i){
        for(int j=1;j<=5-i;++j){
            if(a[j]>a[j+1]){
                swap(a[j],a[j+1]);
            }
        }
    }
}
int main(){
    int a[11];
    for(int i=1;i<=5;++i){
        cin>>a[i];
    }
    pop(a);   
    for(int i=1;i<=5;++i){
        cout<<a[i]<<" ";
    }
    cout<<endl;
    return 0;
}
*/