#include <iostream>
#define LOG_MESSAGE(tag,mes) std::cout<<"["<<tag<<"]: "<<mes<<std::endl;
void function1(void)
{
    LOG_MESSAGE("function1","run");
}
void Run_Funtion_Times(void (*func)(void),unsigned int times)
{
    for(int i=0;i<times;i++)
    {
        func();
    }
}
int main()
{
    auto fun1 = function1;
    LOG_MESSAGE("fun1 ptr is",*fun1);
    auto fun2 = function1;
    LOG_MESSAGE("fun2 ptr is",*fun2);
    Run_Funtion_Times(fun1,15);
    return 0;
}