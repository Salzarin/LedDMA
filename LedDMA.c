

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "gpio.h"
#include "pwm.h"



volatile unsigned int *dma;
void setup_dma(){
	unsigned int memfd = open("/dev/mem", O_RDWR | O_SYNC);
	void* dma_map = mmap(
						NULL,
						0xF00,
						PROT_READ|PROT_WRITE,
						MAP_SHARED|MAP_LOCKED,
						memfd,
						0x20007000);
	if(dma_map == MAP_FAILED){
		printf("Failed dma map.");
		exit(-1);
	}
	dma = (volatile unsigned int *) dma_map;
	close(memfd);
}





int set_dma(){
return 0;
}


void reset(){
	
while(!(*(pwm+1) &0x2));
for(int i = 0; i<50000; i++){
	while((*(pwm+1) &0x1));
	*(pwm+6) = 0x0;
}

}

unsigned int makeWord(unsigned char led){
	unsigned int word;
	for(int i = 0; i<8;i++){
		if((led>>i) & 0x1){
			word |= 0xC<<(i*4);
		}
		else{
			word |= 0xC<<(i*4);
		}
	}
	return word;
}


void setColor( unsigned int led){


unsigned int word;


while((*(pwm+1) &0x1));
*(pwm+6) = makeWord(led & 0xFF);

while((*(pwm+1) &0x1));
*(pwm+6) = makeWord((led & 0xFF00)>>8);

while((*(pwm+1) &0x1));
*(pwm+6) = makeWord((led & 0xFF0000)>>16);

}

void *showLights(){
while(1){
	for(int i = 0; i<10;i++){
		unsigned int color = 0xFF<<(8*(i%3));
		setColor(color);
	}
	reset();
}

}



int main(){

setup_gpio();

setPinMode(18, 2);
setPinMode(20,0);
setPinMode(21,0);
//setup_dma();
printf("Setting up the clock\n");
setup_pwm_clock();
set_pwm_clock();
printf("Finished Setting up the clock\n");
printf("Setting up PWM\n");
setup_pwm();
setPwm();
printf("Finished Setting up PWM\n");

int state = 0;

pthread_t thread_id;
pthread_create(&thread_id,NULL,showLights,NULL);
//while(1){
//for(int i =0; i<100;i++){
	//setColor(0x0F0);

//}
	//setColor(0x0);

//}


while(1){
//state =!state;
//setPin(18,state);
//printf("%x %x\n", readPin(20), readPin(21));
//usleep(10);
}
pthread_join(thread_id,NULL);
return 0;
}
