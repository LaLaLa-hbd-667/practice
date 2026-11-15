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

#include<iostream>
#include <windows.h>
#include <random>

using namespace std;

struct Cactus{      
    int x;          
    int length;     
};

vector<Cactus> cactus;      

void gotoXY(int x, int y) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);     // 获取标准输出的句柄
    COORD pos = { (SHORT)x, (SHORT)y };             // 创建一个 COORD 结构体，表示光标位置
    SetConsoleCursorPosition(h, pos);               // 设置控制台光标位置
}

void background(){
    for(int i=1;i<=30;++i) printf("#");   cout<<endl;       
    for(int i=1;i<=30;++i) printf(" ");   cout<<endl;       
    for(int i=1;i<=30;++i) printf(" ");   cout<<endl;
    for(int i=1;i<=30;++i) printf(" ");   cout<<endl;
    for(int i=1;i<=30;++i) printf(" ");   cout<<endl;
    for(int i=1;i<=30;++i) printf(" ");   cout<<endl;
    for(int i=1;i<=30;++i) printf("#");   cout<<endl;
}

void draw(int a){
    for(int i=0;i<cactus[a].length;++i){
        gotoXY(cactus[a].x,5-i);
        cout<<"*";
    }
    cactus[a].x--;
}

// 取 [min, max] 闭区间整数
int random_int(int min, int max) {
    static mt19937 gen{random_device{}()};          // 只初始化一次
    uniform_int_distribution<int> dist(min, max);   // 创建一个均匀分布的整数分布对象
    return dist(gen);                               // 生成一个随机整数
}

int main(){

    int wait;

    for(int i=1;i<=1000;++i){
        system("cls");
        background();

        if(random_int(1,8)==1&&wait==0){
            wait =1;
            cactus.push_back({30,random_int(2,3)});             
        }

        for(int i=0;i<cactus.size();++i){
            draw(i);
        }
        if(cactus.size()){
            if(cactus[0].x<2){
                cactus.erase(cactus.begin()); 

            }
        }
        
        gotoXY(2,6);
        Sleep(200);
        if(wait!=0){
            wait++;
        }        
        if(wait>4){
            wait=0;
        }
    }
    
    return 0;
}


/*
#include<iostream>
#include <windows.h>
#include <random>

using namespace std;

struct Cactus{
    int x;
    int length;
    bool live=0;
};

Cactus cactus;

void gotoXY(int x, int y) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(h, pos);
}

void background(){
    for(int i=1;i<=30;++i) printf("#");   cout<<endl;
    for(int i=1;i<=30;++i) printf(" ");   cout<<endl;
    for(int i=1;i<=30;++i) printf(" ");   cout<<endl;
    for(int i=1;i<=30;++i) printf(" ");   cout<<endl;
    for(int i=1;i<=30;++i) printf(" ");   cout<<endl;
    for(int i=1;i<=30;++i) printf(" ");   cout<<endl;
    for(int i=1;i<=30;++i) printf("#");   cout<<endl;
}

void draw(){
    for(int i=0;i<cactus.length;++i){
        gotoXY(cactus.x,5-i);
        cout<<"*";
    }

}

// 取 [min, max] 闭区间整数
int random_int(int min, int max) {
    static mt19937 gen{random_device{}()};   // 只初始化一次
    uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

int main(){
    for(int i=1;i<=1000;++i){
        system("cls");
        background();

        if(random_int(1,5)==1){
            cactus.live=1;
            cactus.x=30;
            cactus.length=3;
        }
        if(cactus.live==1){
            draw();
            cactus.x--;
        }
        if(cactus.x<1){
            cactus.live=0;
        }
        
        gotoXY(2,6);
        Sleep(200);
    }
    
    return 0;
}
*/