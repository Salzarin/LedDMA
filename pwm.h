
#ifndef PWM_H
#define PWM_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <unistd.h>
#include <time.h>

volatile unsigned int * pwm;
volatile unsigned int *pwm_clock;

void set_pwm_clock(unsigned int type, unsigned int IDIV, unsigned int FDIV);
void setup_pwm();
void setup_pwm_clock();
void setPwm();


#endif
