//简易通讯录
/*
实现功能：
    1.添加联系人
    2.显示所有联系人
    3.查找联系人
    4.删除联系人
    5.按姓名首字母/拼音排序
    6.退出程序
*/
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

//第二版：实现永久存储功能-文件(以当前代码名命名)在当前文件夹路径保存
#include <windows.h>
#include<iostream>
#include<vector>
#include<cstdlib>   //调用system("cls")函数
#include<fstream>   //实现文件读写功能
#include<string>    //获取文件路径用于保存contact.txt文件
using namespace std;

struct Contact {
    string name;
    string phone;

};

vector<Contact> menu;
int usernum=0;

//文件在当前文件夹保存，文件名为当前文件名.txt
string getSavePath() {
    string currentFile = __FILE__; 
    
    // 1. 找到文件夹的路径
    size_t pos = currentFile.find_last_of("\\/");
    string dirPath = currentFile.substr(0, pos + 1); 
    
    // 2. 提取文件名（比如 04UserMenu.cpp）
    string fileName = currentFile.substr(pos + 1);
    
    // 3. 把 .cpp 去掉，换成 .txt
    size_t dotPos = fileName.find_last_of(".");
    if (dotPos != string::npos) {
        fileName = fileName.substr(0, dotPos) + ".txt";
    }
    
    // 4. 拼在一起：路径 + 代码名.txt
    return dirPath + fileName;
}
/*
// 获取当前代码文件所在的完整路径，然后拼上文件名
//文件在当前文件夹保存，文件名为contact
string getSavePath() {
    string currentFile = __FILE__; // 编译器会自动把它变成 "F:\...\04UserMenu.cpp"
    
    // 找到最后一个斜杠或反斜杠
    size_t pos = currentFile.find_last_of("\\/");
    if (pos != string::npos) {
        // 截取前面的路径，拼上 contacts.txt
        return currentFile.substr(0, pos + 1) + "contacts.txt";
    }
    return "contacts.txt"; // 如果出意外，退回默认
}
*/

// 保存数据到文件
void SaveData() {
    // 以覆盖写的方式打开文件
    
    ofstream fout(getSavePath()); 
    //ofstream fout("C++/practice/contacts.txt"); 
    //ofstream fout("contacts.txt");

    if (!fout) {
        cout << "保存文件失败！" << endl;
        return;
    }
    // 遍历 vector，把每一条写进去（姓名和电话用空格隔开）
    for (int i = 0; i < menu.size(); ++i) {
        fout << menu[i].name << " " << menu[i].phone << "\n";
    }
    fout.close();
    cout << "数据已自动保存。" << endl;
}

// 从文件加载数据
void LoadData() {
    // 直接调用这个函数
    ifstream fin(getSavePath());
    //ifstream fin("contacts.txt");

    if (!fin) {
        // 第一次运行没有文件是正常的，直接返回
        return; 
    }
    string n, p;
    // 循环读取，直到文件末尾
    while (fin >> n >> p) {
        Contact c;
        c.name = n;
        c.phone = p;
        menu.push_back(c);
    }
    // 同步你的 usernum 变量（虽然用 menu.size() 更准）
    usernum = menu.size(); 
    fin.close();
}

void Add(){
    
    usernum+=1;
    string s,p;
    
    printf("请输入用户名称：");
    cin>>s;

    printf("请输入用户电话号码：");
    cin>>p;

    menu.push_back({s,p});

    SaveData();

    cout<<"已成功添加："<<endl;
    cout<<menu[usernum-1].name<<endl;
    cout<<menu[usernum-1].phone<<endl;
}

void Delete(){

    string s;

    if(menu.empty()){
        cout<<"通讯录为空。"<<endl;
    }
    else {
        cout<<"请输入要删除的对象："<<endl;
        cin>>s;
        for(int i=0;i<menu.size();++i){
            if(menu[i].name==s){
                cout<<"以成功删除："<<menu[i].name<<endl;
                menu.erase(menu.begin() + i);
                usernum--;
                SaveData();                
            }
        }
    }

}

void Detect(){

    string s;

    cout<<"请输入要查找的对象："<<endl;
    cin>>s;
   
     for(int i=0;i<menu.size();++i){
        if(menu[i].name==s){
            cout<<"查找到"<<menu[i].name<<endl;
            cout<<"ta的电话号码是"<<menu[i].phone<<endl;
        }
    }
    
    
}

void Display(){

    for(int i=0;i<menu.size();++i){
        cout<<"用户"<<i+1<<":"<<endl;
        cout<<"姓名："<<menu[i].name<<endl;
        cout<<"电话号码："<<menu[i].phone<<endl<<endl;
    }
      
}

void UserSort(){

    int n = menu.size();
    // 外层循环：n个元素需要 n-1 趟
    for (int i = 0; i < n - 1; ++i) {
        // 内层循环：每趟比较次数逐渐减少
        for (int j = 0; j < n - 1 - i; ++j) {
            if (menu[j].name > menu[j + 1].name) {
                swap(menu[j], menu[j + 1]); // 直接使用标准库的 swap
            }
        }
    }    

    cout<<"排序后的通讯录为："<<endl;
    Display();
    SaveData();
}

void Change(){

    string s,p;

    cout<<"请输入你要修改的用户"<<endl;
    cin>>s;
    cout<<"请输入修改后的电话号码"<<endl;
    cin>>p;

    for(int i=0;i<menu.size();++i){
        if(s==menu[i].name){
            cout<<"该用户原电话号码为："<<menu[i].phone<<endl;
            cout<<"该用户修改后的电话号码为："<<p<<endl;
            menu[i].phone=p;
        }       
    }
    SaveData();
}

int main(){

    SetConsoleOutputCP(65001); // 设置控制台输出代码页为UTF-8
    SetConsoleCP(65001);       // 设置控制台输入代码页为UTF-8

    LoadData();                // <--- 程序启动时，先读取上次保存的数据

    int p=0;
    
    system("cls");
    printf("请输入操作数：\n");
        cout<<"1.添加联系人"<<endl<<"2.删除联系人"<<endl<<"3.查找联系人"<<endl;
        cout<<"4.显示所有联系人"<<endl<<"5.按姓名首字母/拼音排序"<<endl<<"6.修改用户电话号码"<<endl<<"7.退出程序"<<endl;

        while(cin>>p){

        switch(p){
            case 1 :
                Add();              
                break;
            case 2 :
                Delete();
                break;
            case 3 :
                Detect();
                break;
            case 4 :
                Display();
                break;
            case 5 :
                UserSort();
                break;
            case 6:
                Change();
                break;
            case 7 :
                cout<<endl<<"667"<<endl;
                return 0;
        }

        if (cin.fail()) {
            cin.clear(); // 清除错误状态
            cin.ignore(10000, '\n'); // 丢弃错误输入
        }

        Sleep(2000);
        system("cls");
        printf("请输入操作数：\n");
        cout<<"1.添加联系人"<<endl<<"2.删除联系人"<<endl<<"3.查找联系人"<<endl;
        cout<<"4.显示所有联系人"<<endl<<"5.按姓名首字母/拼音排序"<<endl<<"6.修改用户电话号码"<<endl<<"7.退出程序"<<endl;
        

    }

    
    return 0;
}

/*
//第一版：通讯录基本功能实现
#include <windows.h>
#include<iostream>
#include<vector>
#include<cstdlib>   //调用system("cls")函数
using namespace std;

struct Contact {
    string name;
    string phone;

};

vector<Contact> menu;
int usernum=0;

void Add(){
    
    usernum+=1;
    string s,p;
    
    printf("请输入用户名称：");
    cin>>s;

    printf("请输入用户电话号码：");
    cin>>p;

    menu.push_back({s,p});

    cout<<"已成功添加："<<endl;
    cout<<menu[usernum-1].name<<endl;
    cout<<menu[usernum-1].phone<<endl;
}

void Delete(){

    string s;

    if(menu.empty()){
        cout<<"通讯录为空。"<<endl;
    }
    else {
        cout<<"请输入要删除的对象："<<endl;
        cin>>s;
        for(int i=0;i<menu.size();++i){
            if(menu[i].name==s){
                cout<<"以成功删除："<<menu[i].name<<endl;
                menu.erase(menu.begin() + i);                
            }
        }
    }

}

void Detect(){

    string s;

    cout<<"请输入要查找的对象："<<endl;
    cin>>s;
   
     for(int i=0;i<menu.size();++i){
        if(menu[i].name==s){
            cout<<"查找到"<<menu[i].name<<endl;
            cout<<"ta的电话号码是"<<menu[i].phone<<endl;
        }
    }
    
    
}

void Display(){

    for(int i=0;i<menu.size();++i){
        cout<<"用户"<<i+1<<":"<<endl;
        cout<<"姓名："<<menu[i].name<<endl;
        cout<<"电话号码："<<menu[i].phone<<endl<<endl;
    }
      
}

void UserSort(){

    int n = menu.size();
    // 外层循环：n个元素需要 n-1 趟
    for (int i = 0; i < n - 1; ++i) {
        // 内层循环：每趟比较次数逐渐减少
        for (int j = 0; j < n - 1 - i; ++j) {
            if (menu[j].name > menu[j + 1].name) {
                swap(menu[j], menu[j + 1]); // 直接使用标准库的 swap
            }
        }
    }    

    cout<<"排序后的通讯录为："<<endl;
    Display();

}

void Change(){

    string s,p;

    cout<<"请输入你要修改的用户"<<endl;
    cin>>s;
    cout<<"请输入修改后的电话号码"<<endl;
    cin>>p;

    for(int i=0;i<menu.size();++i){
        if(s==menu[i].name){
            cout<<"该用户原电话号码为："<<menu[i].phone<<endl;
            cout<<"该用户修改后的电话号码为："<<p<<endl;
            menu[i].phone=p;
        }       
    }

}

int main(){

    SetConsoleOutputCP(65001); // 设置控制台输出代码页为UTF-8
    SetConsoleCP(65001);       // 设置控制台输入代码页为UTF-8


    int p=0;
    
    system("cls");
    printf("请输入操作数：\n");
        cout<<"1.添加联系人"<<endl<<"2.删除联系人"<<endl<<"3.查找联系人"<<endl;
        cout<<"4.显示所有联系人"<<endl<<"5.按姓名首字母/拼音排序"<<endl<<"6.修改用户电话号码"<<endl<<"7.退出程序"<<endl;

        while(cin>>p){

        switch(p){
            case 1 :
                Add();              
                break;
            case 2 :
                Delete();
                break;
            case 3 :
                Detect();
                break;
            case 4 :
                Display();
                break;
            case 5 :
                UserSort();
                break;
            case 6:
                Change();
                break;
            case 7 :
                cout<<endl<<"667"<<endl;
                return 0;
        }

        Sleep(2000);
        system("cls");
        printf("请输入操作数：\n");
        cout<<"1.添加联系人"<<endl<<"2.删除联系人"<<endl<<"3.查找联系人"<<endl;
        cout<<"4.显示所有联系人"<<endl<<"5.按姓名首字母/拼音排序"<<endl<<"6.修改用户电话号码"<<endl<<"7.退出程序"<<endl;
        

    }

    
    return 0;
}
*/
