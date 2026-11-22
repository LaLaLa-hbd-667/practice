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




