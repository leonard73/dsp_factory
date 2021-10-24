#include <iostream>
namespace logSpace
{
    #include <iostream>
    #define LOG_MESSAGE(tag,mes) std::cout<<"["<<tag<<"]: "<<mes<<std::endl; 
    class logClass
    {
    public:  enum EnumLogLevel:char
            {
                warning=0,
                info=1,   
                error=2         
            };
    private: char LogLevelSetting=warning;
             char tagW[10]="warning";
             char tagI[10]="info   ";
             char tagE[10]="error  ";
    public:  void setLogLevel(EnumLogLevel level)
            {
                this->LogLevelSetting = level;
            }
    public:  void LogW(const char message[])
            {
                if(LogLevelSetting<=warning) LOG_MESSAGE(tagW,message);
            }
    public:  void LogI(const char message[])
            {
                if(LogLevelSetting<=info) LOG_MESSAGE(tagI,message);
            }
    public:  void LogE(const char message[])
            {
                if(LogLevelSetting<=error) LOG_MESSAGE(tagE,message);
            }
    };
};
int main()
{
    logSpace::logClass logObj;
    logObj.setLogLevel(logSpace::logClass::warning);
    logObj.LogW("Level warning: warning messages");
    logObj.LogI("Level warning: info    messages");
    logObj.LogE("Level warning: error   messages");
    logObj.setLogLevel(logSpace::logClass::info);
    logObj.LogW("Level info   : warning messages");
    logObj.LogI("Level info   : info    messages");
    logObj.LogE("Level info   : error   messages");
    logObj.setLogLevel(logSpace::logClass::error);
    logObj.LogW("Level error  : warning messages");
    logObj.LogI("Level error  : info    messages");
    logObj.LogE("Level error  : error   messages");
    return 0;
}