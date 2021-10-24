/* Use Explicit avoid immidiate conversion, we should use constructor mode*/
#include <iostream>
#define LOG_MESSAGE(tag,mes) std::cout<<"["<<tag<<"]: "<<mes<<std::endl;
class class1{
    private: std::string name;
    private: int value;
    public: class1(const std::string setName):name(setName),value(0)
    {
        LOG_MESSAGE("class1","create");
    }
    public: class1(const int setValue):name("Null"),value(setValue)
    {
        LOG_MESSAGE("class1","create");
    }
    public: class1():name("Null"),value(0)
    {
        LOG_MESSAGE("class1","create");
    }
    public: ~class1()
    {
        LOG_MESSAGE("class1","destory");
    }
};
class class1_better{
    private: std::string name;
    private: int value;
    public: explicit class1_better(const std::string setName):name(setName),value(0)
    {
        LOG_MESSAGE("class1_better","create");
    }
    public: explicit class1_better(const int setValue):name("Null"),value(setValue)
    {
        LOG_MESSAGE("class1_better","create");
    }
    public: explicit class1_better():name("Null"),value(0)
    {
        LOG_MESSAGE("class1_better","create");
    }
    public: ~class1_better()
    {
        LOG_MESSAGE("class1_better","destory");
    }
};
int main()
{
    LOG_MESSAGE("1.","allow immediate conversion ");
    class1  p_class1_1=std::string("create1");
    class1  p_class1_2=(int) 76;
    LOG_MESSAGE("2.","avoid immediate conversion ");
    // class1_better  p_class1_1=std::string("create1"); //no permission
    // class1_better  p_class1_2=(int) 76;//no permission
    class1_better  p_class2_1=class1_better(std::string("create1"));
    class1_better  p_class2_2=class1_better((int) 76);
    return 0;
}