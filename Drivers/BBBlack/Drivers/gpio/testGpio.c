#include "stdio.h"
#include "gpio.h"
#define GPIO_PINX   (60)
#define GPIO_PINY   (50)
#define GPIO_PINZ   (51)
int gpio_state(int red_state,int green_state,int blue_state)
{
    printf("state: R: %d ; G: %d ; B : %d\n",red_state, green_state, blue_state);
    int ret;
    ret=setGpioX_DirectionD_ValueV(GPIO_PINX,GPIO_OUT,red_state);
    ret=setGpioX_DirectionD_ValueV(GPIO_PINY,GPIO_OUT,green_state);
    ret=setGpioX_DirectionD_ValueV(GPIO_PINZ,GPIO_OUT,blue_state);
    return ret;
}
int main(int argc,char * argv[])
{

    int ret = 0;
    printf("start GPIO test\n");
    int r_start=0,g_start=0,b_start=0;
    int pinx = (int)GPIO_PINX;
    int piny = (int)GPIO_PINY;
    int pinz = (int)GPIO_PINZ;
    // ret=gpio_unexport(pinx);
    // ret=gpio_unexport(piny);
    // ret=gpio_unexport(pinz);
    // ret=gpio_export(pinx);
    // ret=gpio_export(piny);
    // ret=gpio_export(pinz);
    while(1)
    {
        ret=gpio_state(GPIO_ON,GPIO_OFF,GPIO_OFF);
        sleep(1);
        ret=gpio_state(GPIO_ON,GPIO_ON,GPIO_OFF);
        sleep(1);
        ret=gpio_state(GPIO_OFF,GPIO_ON,GPIO_OFF);
        sleep(1);
        ret=gpio_state(GPIO_OFF,GPIO_ON,GPIO_ON);
        sleep(1);
        ret=gpio_state(GPIO_OFF,GPIO_OFF,GPIO_ON);
        sleep(1);
        ret=gpio_state(GPIO_ON,GPIO_OFF,GPIO_ON);
        sleep(1);
        ret=gpio_state(GPIO_ON,GPIO_ON,GPIO_ON);
        sleep(1);
        ret=gpio_state(GPIO_OFF,GPIO_OFF,GPIO_OFF);
        sleep(1);
    }
    // ret=gpio_unexport(pinx);
    // ret=gpio_unexport(piny);
    // ret=gpio_unexport(pinz);
    return 0;
}