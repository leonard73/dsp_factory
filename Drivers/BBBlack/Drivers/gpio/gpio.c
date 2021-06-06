#include "gpio.h"
int gpio_export(int pin)  
{  
    char buffer[64];  
    int len;  
    int fd;  
    system("chmod 777 -R sys/class/gpio");
    fd = open("/sys/class/gpio/export", O_WRONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open export for writing!\n");  
        return(-1);  
    }  
  
    len = snprintf(buffer, sizeof(buffer), "%d", pin);  
    printf("%s,%d,%d\n",buffer,sizeof(buffer),len);
    if (write(fd, buffer, len) < 0) 
    {  
        MSG("Failed to export gpio!");  
        return -1;  
    }  
     
    close(fd);  
    return 0;  
}  
int gpio_unexport(int pin)  
{  
    char buffer[64];  
    int len;  
    int fd;  
  
    fd = open("/sys/class/gpio/unexport", O_WRONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open unexport for writing!\n");  
        return -1;  
    }  
  
    len = snprintf(buffer, sizeof(buffer), "%d", pin);  
    if (write(fd, buffer, len) < 0) 
    {  
        MSG("Failed to unexport gpio!");  
        return -1;  
    }  
     
    close(fd);  
    return 0;  
} 
//dir: 0-->IN, 1-->OUT
int gpio_direction(int pin, int dir)  
{  
    static const char dir_str[] = "in\0out";  
    char path[64];  
    int fd;  
  
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);  
    fd = open(path, O_WRONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open gpio direction for writing!\n");  
        return -1;  
    }  
  
    if (write(fd, &dir_str[dir == 0 ? 0 : 3], dir == 0 ? 2 : 3) < 0) 
    {  
        MSG("Failed to set direction!\n");  
        return -1;  
    }  
  
    close(fd);  
    return 0;  
}  
//value: 0-->LOW, 1-->HIGH
int gpio_write(int pin, int value)  
{  
    static const char values_str[] = "01";  
    char path[64];  
    int fd;  
  
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);  
    fd = open(path, O_WRONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open gpio value for writing!\n");  
        return -1;  
    }  
  
    if (write(fd, &values_str[value == 0 ? 0 : 1], 1) < 0) 
    {  
        MSG("Failed to write value!\n");  
        return -1;  
    }  
  
    close(fd);  
    return 0;  
}
int gpio_read(int pin)  
{  
    char path[64];  
    char value_str[3];  
    int fd;  
  
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);  
    fd = open(path, O_RDONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open gpio value for reading!\n");  
        return -1;  
    }  
  
    if (read(fd, value_str, 3) < 0)
    {  
        MSG("Failed to read value!\n");  
        return -1;  
    }  
  
    close(fd);  
    return (atoi(value_str));
}  
// none表示引脚为输入，不是中断引脚
// rising表示引脚为中断输入，上升沿触发
// falling表示引脚为中断输入，下降沿触发
// both表示引脚为中断输入，边沿触发
// 0-->none, 1-->rising, 2-->falling, 3-->both
int gpio_edge(int pin, int edge)
{
const char dir_str[] = "none\0rising\0falling\0both"; 
char ptr;
char path[64];  
    int fd; 
switch(edge)
{
    case 0:
        ptr = 0;
        break;
    case 1:
        ptr = 5;
        break;
    case 2:
        ptr = 12;
        break;
    case 3:
        ptr = 20;
        break;
    default:
        ptr = 0;
} 
  
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/edge", pin);  
    fd = open(path, O_WRONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open gpio edge for writing!\n");  
        return -1;  
    }  
  
    if (write(fd, &dir_str[ptr], strlen(&dir_str[ptr])) < 0) 
    {  
        MSG("Failed to set edge!\n");  
        return -1;  
    }  
  
    close(fd);  
    return 0;  
}
int setGpioX_DirectionD_ValueV(int X,int D,int V)
{
 #define GPIO_SET_D_PATH "/sys/class/gpio"
 char path_gpio[200];
 char cmd[200];
 FILE * fileP=NULL;
 //set export gpio pin number
 // sprintf(cmd,"echo %d > %s/export",X,GPIO_SET_D_PATH);
 // system(cmd);
 //set gpio direction
 if(D==0)
 {
   sprintf(path_gpio,"%s/gpio%d/direction",GPIO_SET_D_PATH,X);
   fileP=fopen(path_gpio,"w");
   fprintf(fileP,"out");
 }else
 {
   sprintf(path_gpio,"%s/gpio%d/direction",GPIO_SET_D_PATH,X);
   fileP=fopen(path_gpio,"w");
   fprintf(fileP,"in");
 }
 fclose(fileP);
 //set gpio value
 sprintf(path_gpio,"/sys/class/gpio/gpio%d/value",X);
 fileP=fopen(path_gpio,"w");
 fprintf(fileP,"%d",V);
 fclose(fileP);
 //unexport gpio
 // sprintf(cmd,"echo %d > %s/unexport",X,GPIO_SET_D_PATH);
 // system(cmd);
 // sprintf(path_gpio,"%s/unexport",GPIO_SET_D_PATH);
 // fileP=fopen(path_gpio,"w");
 // fprintf(fileP,"%d",X);
 // fclose(fileP);
 return 0;
}