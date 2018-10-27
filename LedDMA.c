#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>


volatile unsigned int * gpio;
volatile unsigned int * pwm;
volatile unsigned int *pwm_clock;
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
void setup_gpio(){
	unsigned int memfd = open("/dev/mem", O_RDWR | O_SYNC);
	void* gpio_map = mmap(
						NULL,
						0xB4,
						PROT_READ|PROT_WRITE,
						MAP_SHARED|MAP_LOCKED,
						memfd,
						0x20200000);
	if(gpio_map == MAP_FAILED){
		printf("Failed gpio map.");
		exit(-1);
	}
	gpio = (volatile unsigned int *) gpio_map;
	close(memfd);
}
void setup_pwm(){
	unsigned int memfd = open("/dev/mem", O_RDWR | O_SYNC);
	void* pwm_map = mmap(
						NULL,
						0x28,
						PROT_READ|PROT_WRITE,
						MAP_SHARED|MAP_LOCKED,
						memfd,
						0x2020C000);
	if(pwm_map  == MAP_FAILED){
		printf("Failed pwm map.");
		exit(-1);
	}
	pwm = (volatile unsigned int *) pwm_map;
	close (memfd);
}

void setup_pwm_clock(){
	unsigned int memfd = open("/dev/mem", O_RDWR | O_SYNC);
	void* pwm_clock_map = mmap(
						NULL,
						0xA8,
						PROT_READ|PROT_WRITE,
						MAP_SHARED|MAP_LOCKED,
						memfd,
						0x20101000);
	if(pwm_clock_map  == MAP_FAILED){
		printf("Failed pwm clock map.");
		exit(-1);
	}
	pwm_clock = (volatile unsigned int *) pwm_clock_map;
	close (memfd);

}

int set_dma(){
return 0;


}
void setPinMode(unsigned int pin, int value){
	if(value >0b111){
	return;
	}
	*(gpio + pin/10) &= ~(7<<((pin%10)*3));
	*(gpio + (pin/10)) |= (value<<((pin%10)*3));

}




void setPin(unsigned int pin, int value){
	if(value){
	*((gpio +(pin/32)+7)) |= 1<<(pin%32);
	}
	else{
	*((gpio +(pin/32)+10)) |= 1<<(pin%32);
	}
}
unsigned int readPin(unsigned int pin){
	return (((*(gpio +(pin/32)+13)) & (1<<pin)) == 1<<pin);
}
void setPwm(){
*(pwm) =0;
usleep(10);
*(pwm+1) =-1;
usleep(10);
*(pwm+4) = 25; 
usleep(10);
*(pwm) |= 0x0040;
usleep(10);
*(pwm) |= 0x0023;
sleep(2);

}
void reset(){

//*(pwm) =0;
for(int i = 0; i<1000000; i++){
	while((*(pwm+1) &0x1));
	*(pwm+6) = 0x0;
}
//}
//printf("Restting\n");
}
void setColor( unsigned int led){



/*
for(int i = 0; i<16; i++){
	while((*(pwm+1) &0x1));
	*(pwm+6) = 0xFE000000;
}
for(int i = 0; i<8;i++){
	while((*(pwm+1) &0x1));
	*(pwm+6) = 0xFFFFC000;
}
*/


for(int i = 0; i<24;i++){
while((*(pwm+1) &0x1));
	if((led>>i) & 0x1){
		*(pwm+6) = 0xFFFFC000;	
	}
	else{
		*(pwm+6) = 0xFE000000;
	}

}

/*
		for(int j = 0; j<3;j++){
			unsigned int word = 0;
			for(int i = 0; i<8;i++){
				
				//printf("%x\n",*(pwm+1) )
				if((led>>(i+j*8)) & 0x1){
					word|= 0x6<<(21-i*3);
				}
				else{
					word|= 0x4<<(21-i*3);
				}
				//printf("%x %x\n", readPin(20), readPin(21));
			}
			while(*(pwm+1) &0x1);;
			*(pwm+6) = word;
		}
*/
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

void set_pwm_clock(){

	*(pwm_clock + 40) = (0x5a << 24) | (1<<5);
	usleep(10);
	*(pwm_clock+41) = (0x5a<<24) | (25<<12) ;
	usleep(10);
	*(pwm_clock + 40) = (0x5a << 24) | 0x6;
	usleep(10);
	*(pwm_clock + 40) |= (0x5a << 24) | (1<<4);
	while(!(*(pwm_clock + 40)  & (1<<7)));
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
