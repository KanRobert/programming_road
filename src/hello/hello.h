#include<iostream>

class Hello{
    Hello(const Hello &)=delete;
    Hello &operator(const Hello &)=delete;
    ~Hello()=delete;

    static void Greeting(){
        std::cout<<"Hello World!\n";
    }
}
