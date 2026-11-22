#pragma once                 
#include <cstddef>     

class MyVector {
public :
    MyVector();
    ~MyVector();

    std::size_t size() const;
    std::size_t capacity() const;

    //void push(int value);
    //void reserve();



private:
    int *m_data;
    std::size_t m_size;
    std::size_t m_capacity;

    void relocate(std::size_t new_cap);

    static int slive;

};