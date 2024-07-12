#include <iostream>

class Empty {
    // 空类没有显式声明的成员或成员函数
};

int main() {
    Empty e1;               // 调用默认构造函数
    Empty e2 = e1;          // 调用拷贝构造函数
    Empty e3;               
    e3 = e1;                // 调用拷贝赋值运算符
    Empty e4 = std::move(e1); // 调用移动构造函数（如果满足条件）
    e3 = std::move(e1);     // 调用移动赋值运算符（如果满足条件）
    // 调用析构函数（当对象的生命周期结束时）
    return 0;
}
