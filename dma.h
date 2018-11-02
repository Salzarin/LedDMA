#ifndef DMA_H
#define DMA_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

volatile unsigned int *dma;
unsigned int *data;
uintptr_t virtTophys(void* virt);
void freeVirtPhysPage(void* virtAddr);
void makeVirtPhysPage(void** virtAddr, void** physAddr);


typedef struct DMAChannelHeader{
	volatile unsigned int CS;
	volatile unsigned int CONBLK_ADDR;
	volatile unsigned int TI;
	volatile unsigned int SOURCE_ADDR;
	volatile unsigned int DEST_ADDR;
	volatile unsigned int TXFR_LEN;
	volatile unsigned int STRIDE;
	volatile unsigned int NEXTCONBK;
	volatile unsigned int DEBUG;

} DMAChannelHeader;

typedef struct DMAControlBlock{
	volatile unsigned int TI;
	volatile unsigned int SOURCE_ADDR;
	volatile unsigned int DEST_ADDR;
	volatile unsigned int TXFR_LEN;
	volatile unsigned int STRIDE;
	volatile unsigned int NEXTCONBK;
	volatile unsigned int padding1;
	volatile unsigned int padding2;
} DMAControlBlock;



void *virtSrcPage[256], *physSrcPage[256];
void *virtBlankSrcPage, *physBlankSrcPage;

void *virtCbPage[256], *physCbPage[256];
void *virtwaitCbPage, *physwaitCbPage;


DMAControlBlock *led_cb[256];
DMAControlBlock *wait_cb;

void setup_dma();
int set_dma();
unsigned int makeWord(unsigned char led);


#endif
