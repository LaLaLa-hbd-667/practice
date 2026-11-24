#include "MyVector.h"
#include <cassert>
#include<iostream>


int main(){
    MyVector v;
    int temp;
    std::cin>>temp;
    v.push(temp);
    std::cout<<v[0]<<std::endl;
    if(v.size()==1) std::cout<<"GOOD"<<std::endl;
   


    return 0;


}