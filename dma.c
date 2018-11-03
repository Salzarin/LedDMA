#include "dma.h"
#include "pwm.h"
#include <string.h>
#include <stdint.h>


uintptr_t virtTophys(void* virt){
	uint64_t PAGE_SIZE = 4096;	
	uint64_t pageInfo;
	uint64_t byteoffset = (uintptr_t)(virt)%PAGE_SIZE;	
	int file = open("/proc/self/pagemap", 'r');
    lseek(file, (uintptr_t)(virt)/PAGE_SIZE*8, SEEK_SET);
    read(file, &pageInfo, 8);
	pageInfo = pageInfo & ~(0x1ffull << 55);
	return (uintptr_t)(pageInfo*PAGE_SIZE +byteoffset);
}


void freeVirtPhysPage(void* virtAddr) {
    munlock(virtAddr, 4096);
    free(virtAddr);
}
void makeVirtPhysPage(void** virtAddr, void** physAddr) {
    	unsigned int PAGE_SIZE = 4096;
	*virtAddr = valloc(PAGE_SIZE); //allocate one page of RAM

    //force page into RAM and then lock it there:
    ((int*)*virtAddr)[0] = 1;
    mlock(*virtAddr, PAGE_SIZE);
    memset(*virtAddr, 0, PAGE_SIZE); //zero-fill the page for convenience

    //Magic to determine the physical address for this page:
    uint64_t pageInfo;
    int file = open("/proc/self/pagemap", 'r');
    lseek(file, ((size_t)*virtAddr)/PAGE_SIZE*8, SEEK_SET);
    read(file, &pageInfo, 8);

    *physAddr = (void*)(size_t)((pageInfo*PAGE_SIZE ));
    printf("makeVirtPhysPage virtual to phys: %p -> %p\n", *virtAddr, *physAddr);
}

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


void shutdown_dma(){
	volatile unsigned int* dma_channel = dma+0x500/4;
		*(dma_channel) &=~0x1;
		freeVirtPhysPage(virtwaitCbPage);
		freeVirtPhysPage(virtBlankSrcPage);
		//free(data);

}


int set_dma(){
	unsigned int led = 100;
	unsigned int wait_time = 200;
	volatile unsigned int* dma_channel = dma+0x500/4;
	unsigned int total_led = led+wait_time;
	data = malloc(4*3*led);
	printf("Setting up DMA %x\n", (uint32_t)(dma_channel));
	
	makeVirtPhysPage(&virtwaitCbPage, &physwaitCbPage);
	makeVirtPhysPage(&virtBlankSrcPage, &physBlankSrcPage);
	
	unsigned int * data_ptr = data;

	for(int i = 0; i<led;i++){
		if(i > 50){
		*data_ptr = makeWord(0xFF);
		data_ptr++;
		*data_ptr = makeWord(0x00);
		data_ptr++;
		*data_ptr = makeWord(0x00);
		data_ptr++;
		}
		else if( i > 25){
		*data_ptr = makeWord(0x00);
		data_ptr++;
		*data_ptr = makeWord(0xFF);
		data_ptr++;
		*data_ptr = makeWord(0x00);
		data_ptr++;
		}
		else{
		*data_ptr = makeWord(0x00);
		data_ptr++;
		*data_ptr = makeWord(0x00);
		data_ptr++;
		*data_ptr = makeWord(0xFF);
		data_ptr++;
		}
	}



	for(int i = 0; i<((led/40)+1); i++){
		printf("Page %d\n", i);
		makeVirtPhysPage(&virtCbPage[i], &physCbPage[i]);
		makeVirtPhysPage(&virtSrcPage[i], &physSrcPage[i]);
		led_cb[i] = (DMAControlBlock *)virtCbPage[i];
		SrcPages[i] = (unsigned int *)virtSrcPage[i];
		memcpy(SrcPages[i], data+i*3*40, 40*3*4);
		printf("%x %x %x\n", SrcPages[i],*(data+i*3*40),*SrcPages[i]);
	}
	printf("Building Control Blocks\n");
	DMAControlBlock * cb_ptr = led_cb[0];	
	
	
	unsigned int * BlankArray = (unsigned int*)(virtBlankSrcPage);
	memset(BlankArray,0,1);
	uint32_t physDest = 0x7E20C018;
	
	unsigned int * srcData = (unsigned int *)virtSrcPage[0];	
	for(int i = 0; i<(3*led);i++){
		if(!(i%120) && i!=0){
			printf("%d %d\n", i, i/120);
			//makeVirtPhysPage(&virtCbPage[i/150], &physCbPage[i/150]);
			//led_cb[i/120] = (DMAControlBlock *)virtCbPage[i/120];
			cb_ptr--;
			cb_ptr->NEXTCONBK = (uint32_t)(virtTophys(led_cb[i/120]));
			printf("Link to next block: %x %x\n",(uint32_t)led_cb[i/120], virtTophys(led_cb[i/150]));
			cb_ptr = led_cb[i/120];
			srcData = (unsigned int *)SrcPages[i/120];
			printf("Data Src to next block: %x %x\n", (uint32_t)srcData ,virtTophys(srcData));
		} 
		cb_ptr->TI = (5<<16)|(1<<6)| (1<<26)|(1<<1);
		cb_ptr->SOURCE_ADDR = (uint32_t)(virtTophys(srcData));
		cb_ptr->DEST_ADDR = (uint32_t)(physDest);
		cb_ptr->TXFR_LEN = 4;
		cb_ptr->STRIDE = 0;
		cb_ptr->NEXTCONBK = (uint32_t)(virtTophys(cb_ptr+1));
		cb_ptr++;

	}

	
	
	
	wait_cb = (DMAControlBlock *)(virtwaitCbPage);
	
	cb_ptr--;
	cb_ptr->NEXTCONBK = (uint32_t)(virtTophys(wait_cb));
	
	
	cb_ptr = wait_cb;	
	
	
	for(int i = 0; i<wait_time;i++){
		cb_ptr->TI = (5<<16)|(1<<6)| (1<<26)|(1<<1);
		cb_ptr->SOURCE_ADDR = (uint32_t)(virtTophys(BlankArray));
		cb_ptr->DEST_ADDR = (uint32_t)(physDest);
		cb_ptr->TXFR_LEN = 4;
		cb_ptr->STRIDE = 0;
		cb_ptr->NEXTCONBK = (uint32_t)(virtTophys(cb_ptr+1));
		cb_ptr++;
	}
	
	
	cb_ptr--;	
	cb_ptr->NEXTCONBK = virtTophys(led_cb[0]);

	
	
	printf("Destination Data: %x\n",physDest);

	*(dma_channel+0xff0/4)|= (1<<5);
	*(dma_channel)  |= (1<<31);
	usleep(100);
	*(dma_channel)  |= (1<<30);
	usleep(100);
	*(dma_channel) |=(3<<1) | (1<<8);
	
	*(dma_channel+1) = (uint32_t) virtTophys(led_cb[0]) ;
	*(dma_channel+8) |= 0x7;
	*(dma_channel) |=(1<<28)|(1<<29)| 0x1;
	//*(dma_channel) =0x10880001;


		//*(dma_channel) &=~0x1;
		//freeVirtPhysPage(virtCbPage);
		//freeVirtPhysPage(virtDestPage);
		//freeVirtPhysPage(virtSrcPage);
		//free(data);

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
