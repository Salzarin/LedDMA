
#ifndef
#define PWM_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


volatile unsigned int * pwm;
volatile unsigned int *pwm_clock;

void set_pwm_clock(unsigned int type = 1, unsigned int IDIV = 6, unsigned int FDIV = 0);
void setup_pwm();
void setup_pwm_clock();
void setPwm();


#endif