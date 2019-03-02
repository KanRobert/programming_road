#include<cstdio>

class Hello{
    Hello(const Hello &)=delete;
    Hello &operator(const Hello &)=delete;
    ~Hello()=delete;

    static void Greeting(){
        std::printf<<"Hello World!\n";
    }
}
