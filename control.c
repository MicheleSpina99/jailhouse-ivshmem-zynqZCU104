#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>

#define PCI_SHARED_MEM_BASE (0x7fb01000) // R/W zone


/* This file must be crosscompiled and runned on the linux root cell */

static inline void timespec_diff(struct timespec *a, struct timespec *b, struct timespec *result) {
    result->tv_sec  = a->tv_sec  - b->tv_sec;
    result->tv_nsec = a->tv_nsec - b->tv_nsec;
    if (result->tv_nsec < 0) {
        --result->tv_sec;
        result->tv_nsec += 1000000000L;
    }
}

int main(int argc, char *argv[]) {
	printf("\nStart main\n");
		
	struct timespec t1; //start destroy
	struct timespec t2; //bare metal restored (send ack)
	struct timespec t3; //diff
	long long ns;
    
    // Truncate offset to a multiple of the page size, or mmap will fail.
	off_t offset = PCI_SHARED_MEM_BASE;
	size_t pagesize = sysconf(_SC_PAGE_SIZE);
	off_t page_base = (offset / pagesize) * pagesize;
	off_t page_offset = offset - page_base;
    
    
    /* Necessary to save tests in a .txt */
	int fd = open("/dev/mem", O_RDWR); //file descriptor

	/* shared memory pointer */
	unsigned int * mem = mmap((void*)0x7fb01000, page_offset + (28*4) , PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, offset);
	if (mem == MAP_FAILED) {
		perror("Can't map memory");
        	return -1;
    	}
	   
	/*pointer check*/
    printf("[LNX]\n L'area di memoria puntata e' %p \n", mem);


	int PING = 0;

	printf("[LNX]\n-----Start ping-----\n");

	printf("[LNX] \nWaiting for response...\n");
	int rip=0;
//	system("./stress-ng --class cpu-cache --all 1 --metrics-brief -t6000 &");

	while(rip<5475){
		int s=0;
		
		while(mem[PING] != 1 && s<3){
			usleep(1000*5);
			s++;
		}
		if(s<3){
			printf("[LNX] \nResponse received! Changing flag...\n");
			mem[PING] = 0;
		} else {
			printf("\nResponse not received in time! I am about to restore the cell...\n");
			
			clock_gettime(CLOCK_REALTIME, &t1);
			
			system("sh destroy.sh"); //a simple script that perform the restore by using destroy or shutdown
			
			while(mem[PING] != 1);
			
			clock_gettime(CLOCK_REALTIME, &t2);
			

			timespec_diff(&t2,&t1,&t3);
			
			ns = (t3.tv_sec * 1000000000) + t3.tv_nsec;
 			
			FILE *fpt = fopen("time.txt", "a");			
			fprintf(fpt,"%lld\n",ns);
			fclose(fpt);
			
			rip++;
			while(mem[PING] != 1);
		}
	}
    

    return 0;

}
