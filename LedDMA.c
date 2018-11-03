

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>


#include "gpio.h"
#include "pwm.h"
#include "dma.h"

void reset(){
	
while(!(*(pwm+1) &0x2));
for(int i = 0; i<100000; i++){
	while((*(pwm+1) &0x1));
	*(pwm+6) = 0x0;
}

}




void INThandler(int test){
	
	for(int i = 0; i<150; i++){
		setColor(0x00,i);
	}
	
	usleep(10000);
	shutdown_dma();
	exit(1);
}

void makePulse(unsigned int head){
	int tail_length = 10;
	for(int i = 0; i<tail_length;i++){
		char color = i*0x19;
		char red = color;
		char green = 0xFF - red;
		int pos = head-i;
		pos = pos>150 ? 150 : pos;
		printf("%d %d |", red, green);
		setColor((red<<16)|(green<<8),pos);
	}

}

int main(){

setup_gpio();

setPinMode(18, 2);
setPinMode(20,0);
setPinMode(21,0);

signal(SIGINT, INThandler);

printf("Setting up the clock\n");
setup_pwm_clock();
set_pwm_clock(1,6,0);
printf("Finished Setting up the clock\n");
printf("Setting up PWM\n");
setup_pwm();
setPwm();
printf("Finished Setting up PWM\n");

setup_dma();
set_dma();



printf("starting loop\n");


for(int i = 0; i<150; i++){
setColor(0x00,i);
}

int j = 20;

while(1){

//printf("%d %d\r", readPin(20), readPin(21));
printf("%d\n", j);

/*
for(int i = 0; i<150; i++){
setColor(0xFF,i);
}
*/
//setColor(0xFFFF00,j);
makePulse(10);

j++;
j = j%150;

usleep(100000);
}
//pthread_join(thread_id,NULL);
return 0;
}
