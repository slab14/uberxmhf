/*
 * XMHF rich guest app for uhcalltest hypapp
 * author: amit vasudevan (amitvasudevan@acm.org)
 * author: matt mccormack (matthew.mccormack@live.com)
 */

#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <elf.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////////
// base types

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;


#define PAGE_SHIFT 12
#define PAGEMAP_LENGTH 8


//////
// vmcall interface
//////
__attribute__ ((always_inline)) static inline void __vmcall(uint32_t eax, uint32_t ebx, uint32_t edx){
  asm volatile (
		"movl %0, %%eax \r\n"
		"movl %1, %%ebx \r\n"
		"movl %2, %%edx \r\n"
		"vmcall \r\n"
		: /*no output*/
		: "g" (eax), "g" (ebx), "g" (edx)
		: "%eax", "%ebx", "%edx"
		);
}


//////
// va_to_pa: virtual to physical address mapping
//////
static uint64_t va_to_pa(void *vaddr) {
  FILE *pagemap;
  unsigned long offset;
  uint64_t page_frame_number = 0;

  // open the pagemap file for the current process
  pagemap = fopen("/proc/self/pagemap", "rb");
  if(pagemap == NULL){
    printf("\n%s: unable to open pagemap file. exiting!", __FUNCTION__);
    exit(1);
  }

  // seek to the page that vaddr is on
  offset = (unsigned long)vaddr / getpagesize() * PAGEMAP_LENGTH;
  if(fseek(pagemap, (unsigned long)offset, SEEK_SET) != 0) {
    printf("\n%s: Failed to seek pagemap to proper location", __FUNCTION__);
    exit(1);
  }

  // The page frame number is in bits 0-54 so read the first 7 bytes and clear the 55th bit
  fread(&page_frame_number, 1, PAGEMAP_LENGTH-1, pagemap);

  page_frame_number &= 0x7FFFFFFFFFFFFF;

  fclose(pagemap);

  return (page_frame_number << PAGE_SHIFT);
}


//////
// uhcall
//////
bool uhcall(uint32_t uhcall_function, void *uhcall_buffer, uint32_t uhcall_buffer_len){
  uint64_t uhcall_buffer_paddr;
  if(uhcall_buffer==NULL && uhcall_buffer_len!=0){
    printf("%s: error: line %u\n", __FUNCTION__, __LINE__);
    return false;
  }
  if(uhcall_buffer!=NULL){
    if((((uint32_t)uhcall_buffer % 4096) + uhcall_buffer_len)>4096){
      printf("%s: error: line %u\n", __FUNCTION__, __LINE__);
      return false;
    }
  }
  uhcall_buffer_paddr=va_to_pa(uhcall_buffer);
  if(mlock(uhcall_buffer, uhcall_buffer_len)==-1){
    printf("%s: error: line %u\n", __FUNCTION__, __LINE__);
    return false;
  }
  __vmcall(uhcall_function, 0, uhcall_buffer_paddr);
  if(munlock(uhcall_buffer, uhcall_buffer_len)==-1){
    printf("%s: error: line %u\n", __FUNCTION__, __LINE__);
    return false;
  }
  return true;
}
 

//////
// uhcalltest test harness
//////
#define UAPP_UHSTATEDB_FUNCTION_INIT   0x60
#define UAPP_UHSTATEDB_FUNCTION_GET   0x61
#define UAPP_UHSTATEDB_FUNCTION_NEXT   0x62

#define MAX_STATES 10

typedef struct{
  uint32_t maxArray[MAX_STATES];
  uint32_t numStates;
  uint32_t vaddr;
  uint32_t deviceID;
  uint32_t stateVal;
}uhstatedb_param_t;

__attribute__ ((aligned(4096))) __attribute__((section(".data"))) uhstatedb_param_t uhcp;

void init(void *bufptr){
  uhstatedb_param_t *ptr_uhcp = (uhstatedb_param_t *)bufptr;
  if(!uhcall(UAPP_UHSTATEDB_FUNCTION_INIT, ptr_uhcp, sizeof(uhstatedb_param_t)))
    printf("hypercall FAILED\n");
  else {
    printf("hypercall SUCCESS\n");
    printf("stateDB initialized\n");
  }
}

void get(void * bufptr) {
  uhstatedb_param_t *ptr_uhcp = (uhstatedb_param_t *)bufptr;
  if(!uhcall(UAPP_UHSTATEDB_FUNCTION_GET, ptr_uhcp, sizeof(uhstatedb_param_t)))
    printf("hypercall FAILED\n");
  else {
    printf("hypercall SUCCESS\n");
    printf("State value for device: %d, -- %d\n", ptr_uhcp->deviceID, ptr_uhcp->stateVal);
  }
} 

void next(void *bufptr){
  uhstatedb_param_t *ptr_uhcp = (uhstatedb_param_t *)bufptr;
  if(!uhcall(UAPP_UHSTATEDB_FUNCTION_NEXT, ptr_uhcp, sizeof(uhstatedb_param_t)))
    printf("hypercall FAILED\n");
  else {
    printf("hypercall SUCCESS\n");
    printf("stateDB value updated\n");
  }
}



__attribute__ ((aligned(4096))) int main(){
  uint32_t numDevices=3;
  uint32_t maxArray[numDevices];
  int i;
  for(i=0;i<numDevices;i++)
    maxArray[i]=2*i+1;

  memcpy(&uhcp.maxArray, maxArray, sizeof(maxArray));
  uhcp.numStates=numDevices;

  printf("[] passing uhstaetDB_param_t to init\n");
  init((void *)&uhcp);

  uhcp.deviceID=0;
  printf("[] getting state value for device %d\n", uhcp.deviceID);
  get((void *)&uhcp);

  uhcp.deviceID=1;
  printf("[] transitioning state value for device %d\n", uhcp.deviceID);
  get((void *)&uhcp);
  next((void *)&uhcp);
  get((void *)&uhcp);

  uhcp.deviceID=0;
  printf("[] attempting to transition state value for device %d above it's max (updating 3x, max is 1)\n", uhcp.deviceID);
  printf(" -- initial value\n");
  get((void *)&uhcp);    
  next((void *)&uhcp);
  next((void *)&uhcp);
  next((void *)&uhcp);
  printf(" -- final value\n");  
  get((void *)&uhcp);

  uhcp.deviceID=2;
  printf("[] attempting to transition state value for device %d to 3 (max is 5)\n", uhcp.deviceID);  
  printf(" -- initial value\n");
  get((void *)&uhcp);    
  next((void *)&uhcp);
  next((void *)&uhcp);
  next((void *)&uhcp);
  printf(" -- final value\n");  
  get((void *)&uhcp);  

  printf("[] test complete\n");
  
  return 0;
}
