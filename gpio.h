#ifndef
#define GPIO_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


volatile unsigned int * gpio;



void setup_gpio();
void setPinMode(unsigned int pin, int value);
void setPin(unsigned int pin, int value);


#endif