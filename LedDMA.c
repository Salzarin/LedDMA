#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

volatile unsigned int * gpio;

void setup_gpio(){
	unsigned int memfd = open("/dev/gpiomem", O_RDWR | O_SYNC);
	void* gpio_map = mmap(
						NULL,
						0xB4,
						PROT_READ|PROT_WRITE,
						MAP_SHARED,
						memfd,
						0);
	if(gpio_map == MAP_FAILED){
		printf("Failed gpio map.");
		exit(-1);
	}
	gpio = (volatile unsigned int *) gpio_map;
}

void setPin(unsigned int pin, int value){
	if(value >0b111){
	return;
	}
	*(gpio + pin/10) &= ~(7<<((pin%10)*3));
	*(gpio + (pin/10)) |= (value<<((pin%10)*3));

}
int main(){

setup_gpio();
setPin(21, 1);

return 0;
}