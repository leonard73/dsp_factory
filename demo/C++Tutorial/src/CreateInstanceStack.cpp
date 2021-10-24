/* new allocate memory on heap */
/* object/value allocate memory on statck */
#include <iostream>
#define LOG_MESSAGE(tag,mes) std::cout<<"["<<tag<<"]: "<<mes<<std::endl;
class class1{
    private: std::string name;
    public: class1(std::string setName):name(setName)
    {
        LOG_MESSAGE("class1","create");
    }
    public: ~class1()
    {
        LOG_MESSAGE("class1","destory");
    }
};
int main()
{
    LOG_MESSAGE("1.","create instance on stack and compiler will free automatically");
    {
        class1 obj_class1=class1("demo1");
    }
    LOG_MESSAGE("2.","create instance on heap but need delete manually");
    class1 * p_class1;
    {
        class1 * p_obj_class1 = new class1("demo2");
        p_class1=p_obj_class1;
    }
    delete p_class1;
    return 0;
}