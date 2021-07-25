#include <iostream>
#include <string.h>
#define LOG_MESSAGE(tag,mes) std::cout<<"["<<tag<<"]: "<<mes<<std::endl;
namespace deviceSpace
{
    class deviceBasic{
        public:     enum DeviceType:char{ TypeA='A',TypeX='X' };
        private:    char device_name[100];
        private:    DeviceType device_type = TypeA;
        protected:  int  data_init[4]={1,2,3,4};
        public:     void setDeviceName(const char *name_device){memcpy(this->device_name,name_device,sizeof(name_device));}
        public:     void setDeviceType(DeviceType setType){this->device_type = setType;}
        public:     void logDeviceName() const {LOG_MESSAGE("DeviceName",this->device_name);};
        public:     void logDeviceType() const {LOG_MESSAGE("DeviceType",char(this->device_type));};
        public:     virtual int getDataFromDevice() const {LOG_MESSAGE("VERTUAL FUNCTION","Not Overeided yet");return 404;};
    };
    class deviceSeriesA:public deviceBasic{
        private: int sum_data() const{return (data_init[0]+data_init[1]+data_init[2]+data_init[3]);};
        public:  deviceSeriesA(){setDeviceType(deviceBasic::TypeA);};
        public:  deviceSeriesA(const char *name_device)
                {   
                    setDeviceType(deviceBasic::TypeA);
                    setDeviceName(name_device);
                };
        public:  int getDataFromDevice() const override {return sum_data();};
        public: ~deviceSeriesA(){LOG_MESSAGE("class deviceSeriesA","Destory class deviceSeriesA");};
    };
    class deviceSeriesX:public deviceBasic{
        private: int sum2_data() const {return (data_init[0]*data_init[0]+data_init[1]*data_init[1]+data_init[2]*data_init[2]+data_init[3]*data_init[3]);};
        public:  deviceSeriesX(){setDeviceType(deviceBasic::TypeX);};
        public:  deviceSeriesX(const char *name_device)
                {   
                    setDeviceType(deviceBasic::TypeX);
                    setDeviceName(name_device);
                };
        public:  int getDataFromDevice() const override {return sum2_data();};
        public: ~deviceSeriesX(){LOG_MESSAGE("class deviceSeriesX","Destory class deviceSeriesX");};
    };
};
int main()
{
    deviceSpace::deviceSeriesA * p_objDeviceA = new deviceSpace::deviceSeriesA("AID1");
    p_objDeviceA->logDeviceType();
    p_objDeviceA->logDeviceName();
    LOG_MESSAGE("GetDataFromDeviceA",p_objDeviceA->getDataFromDevice());
    deviceSpace::deviceSeriesX * p_objDeviceX = new deviceSpace::deviceSeriesX("XID3");
    p_objDeviceX->logDeviceType();
    p_objDeviceX->logDeviceName();
    LOG_MESSAGE("GetDataFromDeviceX",p_objDeviceX->getDataFromDevice());
    delete p_objDeviceA;
    delete p_objDeviceX;
    return 0;
}
