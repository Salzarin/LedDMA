#ifndef GPIO_H
#define GPIO_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>


volatile unsigned int * gpio;



void setup_gpio();
void setPinMode(unsigned int pin, int value);
void setPin(unsigned int pin, int value);
unsigned int readPin(unsigned int pin);

#endif
