#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

volatile unsigned int * gpio;

void setup_gpio(){
	unsigned int memfd = open("/dev/gpiomem", O_RDWR | O_SYNC);
	void gpio_map = mmap(
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


int main(){

setup_gpio();


return 0;
}