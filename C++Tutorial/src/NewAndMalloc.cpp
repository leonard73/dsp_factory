/* new allocate memory on heap and call constructor in the same time*/
/* malloc only allocate memory without calling constructors/deconstructors */
#include <iostream>
#define LOG_MESSAGE(tag,mes) std::cout<<"["<<tag<<"]: "<<mes<<std::endl;
class class1{
    private: std::string name;
    public: class1(std::string setName):name(setName)
    {
        LOG_MESSAGE("class1","create");
    }
    public: class1():name("Null")
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
    LOG_MESSAGE("1.","allocate instances with malloc/free");
    class1 * p_class1_1;
    {
        class1 *obj_class1 = (class1 *)malloc(10*sizeof(class1));
        p_class1_1 = obj_class1;
    }
    free(p_class1_1);
    LOG_MESSAGE("2.","allocate instances with new/delete");
    class1 * p_class1;
    {
        class1 * p_obj_class1 = new class1[10]();
        p_class1=p_obj_class1;
    }
    delete[] p_class1;
    return 0;
}