#include"MyVector.h"

int MyVector::slive=0;

MyVector::MyVector():m_data(nullptr),m_size(),m_capacity(){}

MyVector::~MyVector(){
    if(m_data!=nullptr){
        delete [] m_data;
        --slive;
        m_data=nullptr;
    }
    m_size=0;
    m_capacity=0;
}

std::size_t MyVector::size() const { return m_size; }
std::size_t MyVector::capacity() const { return m_capacity; }

int& MyVector::operator[](std::size_t index){ return m_data[index]; }
bool MyVector::empty() const { return m_size==0; }

void MyVector::relocate(std::size_t newcap){
    int *new_data = new int[newcap];
    ++slive;

    for(int i=0;i<m_size;++i){
        new_data[i]=m_data[i];
    }

    if(m_data!=nullptr){
        delete[] m_data;
        --slive;
    }

    m_data=new_data;
    m_capacity=newcap;


}


void MyVector::push(int value){
    if(m_size==m_capacity){
        std::size_t newcap = (m_capacity==0) ? 1 : m_capacity*2;
        relocate(newcap);
    }
    m_data[m_size]=value;
    m_size++;
}

void MyVector::reserve(std::size_t newcap){

    if(newcap>m_capacity){
        relocate(newcap);
    }

}


void MyVector::clear(){

    m_size=0;

}




