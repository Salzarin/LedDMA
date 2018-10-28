#include "gpio.h"


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