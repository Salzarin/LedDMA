

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

#include <mosquitto.h>

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
	
	HSL red;
	HSL green;
	HSL blue;
	HSL test;
	
	RGBtoHSL(0xFF0000, &red);
	RGBtoHSL(0x00FF00, &green);
	RGBtoHSL(0x0000FF, &blue);
	

	
	for(int i = 0; i<tail_length;i++){

		setColor(interpolateColor(red,green,tail_length,i),pos);
		pos--;
		pos = pos>149?149:pos;
	}
	for(int i = 0; i<tail_length;i++){

		setColor(interpolateColor(green,blue,tail_length,i),pos);
		pos--;
		pos = pos>149?149:pos;
	}
	for(int i = 0; i<tail_length;i++){

		setColor(interpolateColor(blue,red,tail_length,i),pos);
		pos--;
		pos = pos>149?149:pos;
	}
	
	if(tail_length*3 < 150){
		setColor(0,pos);
	}

}


void makeRandomPulse(unsigned int head, int tail_length){
	tail_length = tail_length <= 0 ? 1: tail_length;
	unsigned int pos = head;
	
	HSL red;
	HSL green;
	HSL blue;
	HSL test;
	
	RGBtoHSL(0xFF0000, &red);
	RGBtoHSL(0x00FF00, &green);
	RGBtoHSL(0x0000FF, &blue);
	

	
	for(int i = 0; i<tail_length;i++){

		setColor(interpolateColor(red,green,tail_length,i),pos);
		pos--;
		pos = pos>149?149:pos;
	}
	for(int i = 0; i<tail_length;i++){

		setColor(interpolateColor(green,blue,tail_length,i),pos);
		pos--;
		pos = pos>149?149:pos;
	}
	for(int i = 0; i<tail_length;i++){

		setColor(interpolateColor(blue,red,tail_length,i),pos);
		pos--;
		pos = pos>149?149:pos;
	}
	
	if(tail_length*3 < 150){
		setColor(0,pos);
	}
	else{
		setColor(0xFF0000,pos);
	}
}


int on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg)
{
	printf("%s %s (%d)\n", msg->topic, (const char *)msg->payload, msg->payloadlen);
	return 0;
}

int main(int argc, char *argv[]){

int solidColorFlag = 1;
int pulseGenerator = 0;

if(argc>1){
solidColorFlag = 0;
pulseGenerator = 1;
}
srand(time(0));
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

printf("Starting MQTT");

printf("starting loop\n");


solidColor(0x0);
int j = 20;

HSL start;
HSL finish;

RGBtoHSL(rand() % 0xFFFFFF,&start);
RGBtoHSL(rand() % 0xFFFFFF,&finish);

while(1){



if(solidColorFlag){
	solidColor(interpolateColor(start,finish,1000,j));
	j++;
	if(j == 1000){
	HSLset(&start,&finish);
	RGBtoHSL(rand() % 0xFFFFFF,&finish);
	}
	j = j%1000;
	usleep(1000);
}
if(pulseGenerator){

makePulse(j, 50);
j++;
j = j%150;
usleep(100000);
}






}
//pthread_join(thread_id,NULL);
return 0;
}
