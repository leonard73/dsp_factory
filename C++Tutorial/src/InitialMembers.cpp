#include <iostream>
#define LOG_MESSAGE(tag,mes) std::cout<<"["<<tag<<"]: "<<mes<<std::endl;
namespace newSpace{
    class class1{
        private: int data;
        public: class1(int x):data(x)
        {
            LOG_MESSAGE("class1-initialize with ",x);
        }
        public: class1():data(0)
        {
            LOG_MESSAGE("class1-initialize with empty"," ");
        }
    };
    class class2{
        private: class1 class1_obj;
        public:  class2()
        {
            class1_obj=class1(55);
            LOG_MESSAGE("class2-initialize"," ");
        }
    };
    class class2_better{
        private: class1 class1_obj;
        public:  class2_better():class1_obj(class1(55))
        {
            LOG_MESSAGE("class2_better-initialize"," ");
        }
    };
};
int main()
{
    LOG_MESSAGE("worse one"," ")
    newSpace::class2 class2Obj;
    LOG_MESSAGE("better one"," ")
    newSpace::class2_better class2Obj2;
    // newSpace::class2 *class2Obj = new newSpace::class2();
    return 0;
}
