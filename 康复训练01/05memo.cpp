#include <windows.h>
#include<iostream>
#include<vector>
#include<cstdlib>   //调用system("cls")函数
#include<fstream>   //实现文件读写功能
#include<string>    //获取文件路径用于保存contact.txt文件
#include <chrono>   //时间和日期处理库（C++20标准）
#include <format>   
using namespace std;

string today_string(bool local_time = true) {   //如果写today_string(false)它就使用世界标准时间（UTC）
    using namespace chrono;     //时间相关的具体功能又放在了std内部的子命名空间，更规范的写法应为using namespace std::chrono

    auto now = system_clock::now();     //system_clock::now()获取当前系统时间
                                        //system_clock是C++的系统时钟，代表电脑主板上的真实时间
    if (local_time) {                   //判断用户是否要本地时间                  
        zoned_time zt{current_zone(), now};     //创建一个带有特定时区信息的对象zt，zoned_time
                                                //current_zone()函数用于询问电脑在哪个时区
                                                //该行代码相当于zoned_time zt=zoned_time{current_zone(), now},{}是C++11的写法
        return format("{:%Y-%m-%d}", zt);       //format()就是跟printf()差不多的东西
    }
    return format("{:%Y-%m-%d}", floor<days>(now));
}

struct Contact {
    string thing;
    string deadline;
    string time; 
};

vector<Contact> Notebook;
int thingnum=0;

//文件在当前文件夹保存，文件名为当前文件名.txt
//(C++17<filesystem>库就不用自己切字符串了)
string getSavePath() {                  //返回最后生成的保存路径
    string currentFile = __FILE__;      //__FILE_是预处理器宏，编译时替换成当前这段代码所在的源文件的路径字符串
    
    // 1. 找到文件夹的路径
    size_t pos = currentFile.find_last_of("\\/");       //find_last_of("\\/")用来找最后一次出现/或者\的位置（Linus用/）
    string dirPath = currentFile.substr(0, pos + 1);    //substr(开始位置，截取长度)用来截取字符串,(+1是为了把最后的\也存进去)
    
    // 2. 提取文件名（比如 04UserMenu.cpp）
    string fileName = currentFile.substr(pos + 1);      //一直截取到字符串末尾
    
    // 3. 把 .cpp 去掉，换成 .txt
    size_t dotPos = fileName.find_last_of(".");         //找到文件名最后一个.
    if (dotPos != string::npos) {                       //判断有没有找到.
        fileName = fileName.substr(0, dotPos) + ".txt";
    }
    
    // 4. 拼在一起：路径 + 代码名.txt
    return dirPath + fileName;                          //返回文件路径（D:\Code\Project\）和文件名（05memo.txt）拼接到一起
}
// 保存数据到文件
void SaveData() {
    // 以覆盖写的方式打开文件
    

    //打开文件
    ofstream fout(getSavePath());       //ofstream代表输出文件流，fout是文件流的名称
                                        //用ofstream打开文件一个文件时，让默认模式是“截断”，即程序运行到这行时旧内容会全部清空然后重新写
                                        //如果想在文件末尾写，要写成ofstream fout(path,ios::app),即追加写
    //ofstream fout("C++/practice/contacts.txt"); 
    //ofstream fout("contacts.txt");

    if (!fout) {
        cout << "保存文件失败！" << endl;
        return;
    }
    // 遍历 vector，把每一条写进去（姓名和电话用空格隔开）
    for (int i = 0; i < Notebook.size(); ++i) {
        fout << Notebook[i].thing << " " << Notebook[i].deadline << " " << Notebook[i].time << "\n";
    }
                //cout是把数据输出到屏幕控制台，fout是把数据输出到文件


    fout.close();   //好习惯，释放内存和系统资源
    cout << "数据已自动保存。" << endl;
}

// 从文件加载数据
void LoadData() {
    // 直接调用这个函数
    ifstream fin(getSavePath());       //ifstream代表输入文件流，fins是给输入流取的名字
    //ifstream fin("contacts.txt");

    if (!fin) {
        // 第一次运行没有文件是正常的，直接返回
        return; 
    }
    string n, p,q;
    // 循环读取，直到文件末尾
    while (fin >> n >> p >> q) {      //>>提取运算符，自动跳过所有空格、制表符、换行符，读到的分别装给n,p,q
        Contact c;  
        c.thing = n;
        c.deadline = p;
        c.time = q;
        Notebook.push_back(c);        //把组装好的临时结构体推进大数组vector里，随着循环进行，文件里的数据就被一行行还原到了Notebook中
    }
    // 同步你的 usernum 变量（虽然用 menu.size() 更准）
    thingnum = Notebook.size();       //读取完了把总数更新一下
    fin.close();
}

void Add(){
    
    thingnum+=1;
    string s,p,q;
    
    printf("请输入代办事项：");
    cin.ignore();
    getline(cin,s);

    printf("请输入截至日期：");
    cin.ignore();
    getline(cin,p);

    q=today_string();

    Notebook.push_back({s,p,q});

    SaveData();

    cout<<"已成功添加："<<endl;
    cout<<Notebook[thingnum-1].thing<<endl;
    cout<<Notebook[thingnum-1].deadline<<endl;
    cout<<Notebook[thingnum-1].time<<endl;
}

void Delete(){

    string s;

    if(Notebook.empty()){
        cout<<"记事本为空。"<<endl;
    }
    else {
        cout<<"请输入要删除的事项："<<endl;
        cin>>s;
        for(int i=0;i<Notebook.size();++i){
            if(Notebook[i].thing==s){
                cout<<"已成功删除："<<Notebook[i].thing<<endl;
                Notebook.erase(Notebook.begin() + i);
                thingnum--;
                SaveData();                
                i--;        //防止删除连续两个事项时，第二个跳过了
            }
        }
    }

}

void Display(){

    for(int i=0;i<Notebook.size();++i){
        cout<<"事项"<<i+1<<":"<<Notebook[i].thing<<endl;
        cout<<"截至日期："<<Notebook[i].deadline<<endl;
        cout<<"添加时间："<<Notebook[i].time<<endl<<endl;
    }
      
}

void UserSortdeadline(){

    int n = Notebook.size();
    // 外层循环：n个元素需要 n-1 趟
    for (int i = 0; i < n - 1; ++i) {
        // 内层循环：每趟比较次数逐渐减少
        for (int j = 0; j < n - 1 - i; ++j) {
            if (Notebook[j].deadline > Notebook[j + 1].deadline) {
                swap(Notebook[j], Notebook[j + 1]); // 直接使用标准库的 swap
            }
        }
    }    

    cout<<"排序后的通讯录为："<<endl;
    Display();
    SaveData();
}

void UserSorttime(){

    int n = Notebook.size();
    // 外层循环：n个元素需要 n-1 趟
    for (int i = 0; i < n - 1; ++i) {
        // 内层循环：每趟比较次数逐渐减少
        for (int j = 0; j < n - 1 - i; ++j) {
            if (Notebook[j].time > Notebook[j + 1].time) {
                swap(Notebook[j], Notebook[j + 1]); // 直接使用标准库的 swap
            }
        }
    }    

    cout<<"排序后的通讯录为："<<endl;
    Display();
    SaveData();
}

int main(){

    SetConsoleOutputCP(65001); // 设置控制台输出代码页为UTF-8
    SetConsoleCP(65001);       // 设置控制台输入代码页为UTF-8

    LoadData();                // <--- 程序启动时，先读取上次保存的数据

    int p=0;
    
    system("cls");
    printf("请输入操作数：\n");
        cout<<"1.添加事项"<<endl<<"2.删除事项"<<endl<<"3.显示事项"<<endl;
        cout<<"4.事项按截至日期排序"<<endl<<"5.添加按添加时间排序"<<endl<<"6.结束"<<endl;

        while(cin>>p){

        switch(p){
            case 1 :
                Add();              
                break;
            case 2 :
                Delete();
                break;
            case 3 :
                Display();
                break;
            case 4 :
                UserSortdeadline();
                break;
            case 5 :
                UserSorttime();
                break;
            case 6 :
                cout<<endl<<"667"<<endl;
                return 0;
        }

        if (cin.fail()) {
            cin.clear(); // 清除错误状态
            cin.ignore(10000, '\n'); // 丢弃错误输入
        }

        Sleep(4000);
        system("cls");
        printf("请输入操作数：\n");
        cout<<"1.添加事项"<<endl<<"2.删除事项"<<endl<<"3.显示事项"<<endl;
        cout<<"4.事项按截至日期排序"<<endl<<"5.添加按添加时间排序"<<endl<<"6.结束"<<endl;
        

    }

    
    return 0;
}