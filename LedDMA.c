

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

struct mosquitto *mosq;
int solidColorFlag = 1;
int pulseGenerator = 0;
int j = 0;

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
	mosquitto_lib_cleanup();
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


void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	bool match = 0;
	bool state = 1;
	printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);

	mosquitto_topic_matches_sub("state", message->topic, &match);
	if (match) {
		if(message->payload){
			if(atoi(message->payload)){
				state = 1;
					solidColorFlag = 1;
			}
			else{
				state = 0;
				solidColorFlag = 0;
				pulseGenerator = 0;
				solidColor(0x0);
			}
		}
	}
	mosquitto_topic_matches_sub("Pulse Generator", message->topic, &match);
	if (match) {
		if(message->payload){
			if(atoi(message->payload)){
			solidColorFlag = 0;
			pulseGenerator = 1;
			}
			else{
			pulseGenerator = 0;
			solidColorFlag = 1;
			}
			j= 0;
		}
	}


	mosquitto_topic_matches_sub("color", message->topic, &match);
	if (match) {
		if(message->payload){
			if(!strncmp("Red", message->payload,sizeof("Red"))){
			solidColorFlag = 0;
			pulseGenerator = 0;
			solidColor(0xFF0000);
			}
			else{
				
			}
			j= 0;
		}
	}
	
	
	if(!(pulseGenerator|solidColorFlag)){
		if(!state){
			solidColor(0x0);
		}
		
	}
}
void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	printf("connect callback, rc=%d\n", result);
}

int main(int argc, char *argv[]){



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

char clientid[24];
int rc = 0;

printf("Starting MQTT\n");
mosquitto_lib_init();
memset(clientid, 0, 24);
snprintf(clientid, 23, "mysql_log_%d", getpid());
mosq = mosquitto_new(clientid, true, 0);

if(mosq){
	
	mosquitto_connect_callback_set(mosq, connect_callback);
	mosquitto_message_callback_set(mosq, message_callback);
	mosquitto_username_pw_set(mosq,"eheplzcu","5iq2RpaDVH08");
	rc = mosquitto_connect(mosq, "m15.cloudmqtt.com", 12293, 60);
	printf("Connecting to MQTT: %d",rc);
	mosquitto_subscribe(mosq, NULL, "state", 0);
	mosquitto_subscribe(mosq, NULL, "Pulse Generator", 0);
	mosquitto_subscribe(mosq, NULL, "color", 0);
	rc = mosquitto_loop_start(mosq);
	
}
else{
	exit(1);
}

printf("starting loop\n");


solidColor(0x0);


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
return 0;
}
