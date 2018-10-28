#include "dma.h"
#include "pwm.h"

unsigned int makeWord(unsigned char led){
	unsigned int word =0;
	for(int i = 0; i<8;i++){
		if((led>>i) & 0x1){
			word |= 0xC<<((7-i)*4);
		}
		else{
			word |= 0x8<<((7-i)*4);
		}
	}
	return word;
}

int set_dma(){
	unsigned int led = 1;
	data = malloc(led*3);
	for(int i = 0; i<led;i++){
		&data = makeWord(0xFF);
		data++;
		&data = makeWord(0x00);
		data++;
		&data = makeWord(0x00);
		data++;
	}
	
	cbData = malloc(led*3*sizeof(DMAControlBlock));
	for(int i = 0; i<(led*3);i++){
		cbData->TI = (5<<16) | (1<<6) | (1<<26);
		cbData->SOURCE_ADDR = data+i;
		cbData->DEST_ADDR = pwm+6;
		cbData->TXFR_LEN = 4;
		cbData->STRIDE = 1;
		cbDATA->NEXTCONBK = cbData+i+1;
	}
	
	
return 0;
}


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