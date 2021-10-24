#include <iostream>
#define LOG_MESSAGE(tag,mes) std::cout<<"["<<tag<<"]: "<<mes<<std::endl;
void fun1_changeContentOfConstPointer()
{
    const int * constPtr = new int;// should not write: *constPtr = 141;
    int value_set = 141;
    constPtr = &value_set;
    LOG_MESSAGE("constPtr content value is",*constPtr);
}
void fun2_changeContentOfPointerToConst()
{
    int * const PtrToConstValue = new int;// should not write: PtrToConstValue = &value_set;
    *PtrToConstValue = 141;
    LOG_MESSAGE("PtrToConstValue is",*PtrToConstValue);
}
class func3Class{
    private: int data;
    private: mutable int var;
    public:  int getData()const{LOG_MESSAGE("func3Class","int getData()const")  ;var = 5;return this->data;}
    public:  int getData()     {LOG_MESSAGE("func3Class","int getData()")       ;var = 5;return this->data;}
    public:  void setData(int setVal){this->data=setVal;};
};
void func3_logData_const(const func3Class & f3Class)
{
    LOG_MESSAGE("func3_logData_const",f3Class.getData());
}
void func3_logData(func3Class & f3Class)
{
    LOG_MESSAGE("func3_logData",f3Class.getData());
}
void fun3_CallConstFuntionFromClass()
{
    func3Class f3Class;
    f3Class.setData(654);
    func3_logData_const(f3Class);
    func3_logData(f3Class);
}
int main()
{
    fun1_changeContentOfConstPointer();
    fun2_changeContentOfPointerToConst();
    fun3_CallConstFuntionFromClass();
    return 0;
}