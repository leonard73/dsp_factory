#ifndef __GPIO_H__
#define __GPIO_H__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include<poll.h>
#define GPIO_OUT    (0)
#define GPIO_IN     (1)
#define GPIO_ON     (1)
#define GPIO_OFF    (0)
#define MSG(args...) printf(args)
int gpio_export(int pin);
int gpio_unexport(int pin);
int gpio_direction(int pin, int dir);
int gpio_write(int pin, int value);
int gpio_read(int pin);
int gpio_edge(int pin, int edge);
int setGpioX_DirectionD_ValueV(int X,int D,int V);
#endif //__GPIO_H__