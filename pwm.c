#include "pwm.h"


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

void setPwm(){
*(pwm) =0;
usleep(10);
*(pwm+1) =-1;
usleep(10);
*(pwm+4) = 32; 
usleep(10);
*(pwm) |= 0x0040;
usleep(10);
*(pwm) |= 0x0023;
usleep(10);
*(pwm+2) = (1<<31)| (1<<8)|1; 
sleep(2);

}

void set_pwm_clock(unsigned int type, unsigned int IDIV, unsigned int FDIV){

	*(pwm_clock + 40) = (0x5a << 24) | (1<<5);
	usleep(10);
	*(pwm_clock+41) = (0x5a<<24) | (IDIV<<13) | FDIV ;
	usleep(10);
	*(pwm_clock + 40) = (0x5a << 24) | type;
	usleep(10);
	*(pwm_clock + 40) |= (0x5a << 24) | (1<<4);
	while(!(*(pwm_clock + 40)  & (1<<7)));
}
