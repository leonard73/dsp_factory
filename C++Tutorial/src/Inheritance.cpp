#include <iostream>
#include <string.h>
#define LOG_MESSAGE(tag,mes) std::cout<<"["<<tag<<"]: "<<mes<<std::endl;
namespace deviceSpace
{
    class deviceBasic{
        public:  enum DeviceType:char{ TypeA='A',TypeX='X' };
        private: char device_name[100];
        private: DeviceType device_type = TypeA;
        public:  void setDeviceName(const char *name_device){memcpy(this->device_name,name_device,sizeof(name_device));}
        public:  void setDeviceType(DeviceType setType){this->device_type = setType;}
        public:  void logDeviceName(){LOG_MESSAGE("DeviceName",this->device_name);};
        public:  void logDeviceType(){LOG_MESSAGE("DeviceType",char(this->device_type));};
        public:  virtual int getDataFromDevice(){LOG_MESSAGE("VERTUAL FUNCTION","Not Overeided yet");return 404;};
    };
    class deviceSeriesA:public deviceBasic{
        private: int data[4]={1,2,3,4};
        private: int sum_data(){return (this->data[0]+this->data[1]+this->data[2]+this->data[3]);};
        public:  deviceSeriesA(){setDeviceType(deviceBasic::TypeA);};
        public:  deviceSeriesA(const char *name_device)
                {   
                    setDeviceType(deviceBasic::TypeA);
                    setDeviceName(name_device);
                };
        public:  int getDataFromDevice() override {return sum_data();};
        public: ~deviceSeriesA(){LOG_MESSAGE("class deviceSeriesA","Destory");};
    };
    class deviceSeriesX:public deviceBasic{
        private: int data[4]={1,2,3,4};
        private: int sum2_data(){return (this->data[0]*this->data[0]+this->data[1]*this->data[1]+this->data[2]*this->data[2]+this->data[3]*this->data[3]);};
        public:  deviceSeriesX(){setDeviceType(deviceBasic::TypeX);};
        public:  deviceSeriesX(const char *name_device)
                {   
                    setDeviceType(deviceBasic::TypeX);
                    setDeviceName(name_device);
                };
        public:  int getDataFromDevice() override {return sum2_data();};
        public: ~deviceSeriesX(){LOG_MESSAGE("class deviceSeriesX","Destory");};
    };
};
void test()
{
    deviceSpace::deviceSeriesA *objDeviceA = new deviceSpace::deviceSeriesA("AID1");
    objDeviceA->logDeviceType();
    objDeviceA->logDeviceName();
    LOG_MESSAGE("GetDataFromDeviceA",objDeviceA->getDataFromDevice());
    deviceSpace::deviceSeriesX *objDeviceX = new deviceSpace::deviceSeriesX("XID3");
    objDeviceX->logDeviceType();
    objDeviceX->logDeviceName();
    LOG_MESSAGE("GetDataFromDeviceX",objDeviceX->getDataFromDevice());
}
int main()
{
    test();
    return 0;
}
