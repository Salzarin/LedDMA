

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <math.h>

#include "gpio.h"
#include "pwm.h"
#include "dma.h"
#include "color.h"

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


void solidColor(unsigned int color){
	for(int i = 0; i<150; i++){
		setColor(color,i);
	}
}

void makePulse(unsigned int head, int tail_length){
	tail_length = tail_length <= 0 ? 1: tail_length;
	unsigned int pos = head;
	char color = 0x0;
	
	for(int i = 0; i<tail_length;i++){
		color = i*0xFF/tail_length;
		//printf("%x |", (0xFF<<16)|(yellow<<8));
		setColor((0xFF<<16)|(color<<8),pos);
		pos--;
		pos = pos>150?150:pos;
	}

	for(int i = 0; i<tail_length;i++){
		//printf("%x |", (0xFF<<16)|(yellow<<8));
		color = i*0xFF/tail_length;
		setColor(((0xFF-color)<<16)|(0xFF<<8),pos);
		pos--;
		pos = pos>150?150:pos;
	}

	for(int i = 0; i<tail_length;i++){
		color = i*0xFF/tail_length;
		//printf("%x |", (0xFF<<16)|(yellow<<8));
		setColor(((0xFF)<<8)|(color),pos);
		pos--;
		pos = pos>150?150:pos;
	}
	
	for(int i = 0; i<tail_length;i++){
		color = i*0xFF/tail_length;
		//printf("%x |", (0xFF<<16)|(yellow<<8));
		setColor(((0xFF-color)<<8)|(0xFF),pos);
		pos--;
		pos = pos>150?150:pos;
	}

	for(int i = 0; i<tail_length;i++){
		color = i*0xFF/tail_length;
		//printf("%x |", (0xFF<<16)|(yellow<<8));
		setColor(((color)<<16)|(0xFF),pos);
		pos--;
		pos = pos>150?150:pos;
	}

	for(int i = 0; i<tail_length;i++){
		color = i*0xFF/tail_length;
		//printf("%x |", (0xFF<<16)|(yellow<<8));
		setColor(((0xFF)<<16)|(0xFF-color),pos);
		pos--;
		pos = pos>150?150:pos;
}
	pos++;
	setColor(0,pos);
}




int main(){

setup_gpio();

setPinMode(18, 2);
setPinMode(20,0);
setPinMode(21,0);

signal(SIGINT, INThandler);

printf("Setting up the clock\n");
setup_pwm_clock();
set_pwm_clock(1,5,0);
printf("Finished Setting up the clock\n");
printf("Setting up PWM\n");
setup_pwm();
setPwm();
printf("Finished Setting up PWM\n");

setup_dma();
set_dma();



printf("starting loop\n");


solidColor(0x0);
int j = 20;

HSL red;
HSL green;

RGBtoHSL(0xFF0000,&red);
RGBtoHSL(0x00FF00,&green);

while(1){

//printf("%d %d\r", readPin(20), readPin(21));
printf("%d\n", j);

/*
for(int i = 0; i<150; i++){
setColor(0xFF,i);
}
*/
//setColor(0xFFFF00,j);


solidColor(interpolateColor(red,green,150,j));

//makePulse(j, 20);

j++;
j = j%150;


usleep(1000000);
}
//pthread_join(thread_id,NULL);
return 0;
}
