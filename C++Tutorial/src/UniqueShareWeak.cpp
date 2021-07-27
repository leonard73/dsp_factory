#include <iostream>
#include <memory>
#define LOG_MESSAGE(tag,mes) std::cout<<" "<<tag<<" : "<<mes<<std::endl;
namespace SpaceDemo
{
    class ClassDemo
    {
        private : int * data;
        private : unsigned int size;
        private : void init(){this->data = new int[size];}
        private:  void deinit(){delete[] data;}
        public:   ClassDemo(unsigned int data_nb):size(data_nb){init();LOG_MESSAGE("ClassDemo call void init(); size = ",this->size);}
        public:   ClassDemo():size(1){init();LOG_MESSAGE("ClassDemo call void init(); size = ",this->size);}
        public:   ~ClassDemo(){deinit();LOG_MESSAGE("ClassDemo call void deinit(); size = ",this->size);};
    };
};
int main()
{
    {
        SpaceDemo::ClassDemo * p_normPtr = new SpaceDemo::ClassDemo(11);
        delete p_normPtr; // need delete manually or cause memory leaking
    }
    {
        std::unique_ptr<SpaceDemo::ClassDemo> uniquePtr(new SpaceDemo::ClassDemo(22)); // delete automatically especially useful when exception occurs
        //SpaceDemo::ClassDemo * p_normPtr = uniquePtr; // no permission
    }
    // {
        std::shared_ptr<SpaceDemo::ClassDemo> sharePtr(new SpaceDemo::ClassDemo(33)); // heap memory delete automatically and share only increase reference counter then delete once at the end of the pointer lifetime 
        {
        SpaceDemo::ClassDemo * p_normPtr = sharePtr.get();
        }
    // }// this case the cpu delete only once so that optimize the efficiency of heap memory management 
    // {
        std::weak_ptr<SpaceDemo::ClassDemo> weakPtr;
        {
            std::shared_ptr<SpaceDemo::ClassDemo> sharePtr(new SpaceDemo::ClassDemo(44)); // heap memory delete automatically and share only increase reference counter then delete once at the end of the pointer lifetime 
            weakPtr = sharePtr;
        }// this case the cpu delete only once so that optimize the efficiency of heap memory management 
    // }
    return 0;
}