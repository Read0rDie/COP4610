#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mmap.h>

#define PAGE_SIZE (sysconf(_SC_PAGESIZE))
#define MIN_BLOCK_SIZE 32

//memory block macros

#define block_link(lb, rb) \
	rb->previous = lb; \
	lb->next = rb;

#define block_link_lswap(lb, b) \
	block_link(b->previous, lb); \
	block_link(lb, b);

#define block_link_rswap(rb, b) \
	block_link(rb, b->next); \
	block_link(b, rb);

#define unlink(b) \
	block_link(b->previous, b->next);

#define unlink_right(b) \
	unlink(b->next);

#define replace_block(old, new) \
	block_link(old->previous, new); \
	block_link(new, old->next);

struct mem_block
{
	size_t size;
	struct mem_block *previous;
	struct mem_block *next;
} mem_block;

mem_block *start;
start->previous = null;

static int Get_Opt_Size(int size)
{
	int suitableSize = MIN_BLOCK_SIZE;
	while(suitableSize < size)
		suitableSize = suitableSize << 1;
	return suitableSize;
}

int Mem_Init(int size, int policy)
{
	
	int sPage;
	void *ptr;

	int fd = open("/dev/zero", O_RDWR);
	int page = PAGE_SIZE;
	if(size % page != 0)
	{
		sPage = page * (size /page) + page;
	}	
	else
	{
		sPage = size;
	}
	sPage = sPage + MIN_BLOCK_SIZE;
	sPage = Get_Opt_Size(sPage);
	switch(policy)
	{
		case 0:
			ptr = mmap(NULL, sPage, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
			break;
		case 1:
			break;
		case 2:
			break;
	}
	if(ptr == MAP_FAILED)
	{
		perror("mmap");
		close(fd);
		exit(-1);
	}
	close(fd);
	return NULL;
}


void *Mem_Alloc(int size)
{

}


int Mem_Free(void *ptr)
{

}

int Mem_IsValid(void *ptr)
{

}

int Mem_GetSize(void *ptr)
{

}

float Mem_GetFragmentation()
{

}
