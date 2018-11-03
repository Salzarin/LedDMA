

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




void setColor( unsigned int led){




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

void INThandler(int){
	shutdown_dma();
	
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



//int state = 0;

//pthread_t thread_id;
//pthread_create(&thread_id,NULL,showLights,NULL);
/*
while(1){
	//setColor(0x0F0);
	showLights();

	//setColor(0x0);

}
*/
printf("starting loop\n");
while(1){

printf("%d %d\r", readPin(20), readPin(21));
usleep(10);
}
//pthread_join(thread_id,NULL);
return 0;
}
