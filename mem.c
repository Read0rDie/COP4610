#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>

#define PAGE_SIZE (sysconf(_SC_PAGESIZE))
#define MIN_BLOCK_SIZE 32
#define null 0

//memory block macros
#define block_ptr(ptr) ((uint8_t*)ptr)
#define shift_ptr(ptr,size) (block_ptr(ptr)+size)
#define shift_block_ptr(block, size) ((mem_block*)(shift_ptr(block,size)))
#define block_data(block) (shift_ptr(block, sizeof(size_t)))
#define data_block(ptr) (shift_block_ptr(ptr, sizeof(size_t)))
#define block_end(block) (shift_block_ptr(block, block->size))
#define block_metadata(block) (block_ptr(block)-sizeof(size_t))

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

#define insert_block(old, new) \
	block_link(new, old->next); \
	block_link(old, new);

typedef struct mem_block_t
{
	size_t size;
	size_t status;
	struct mem_block_t *previous;
	struct mem_block_t *next;
} mem_block;

mem_block* start;
mem_block* end;
int aPolicy;

//declaration of functions
int Get_Opt_Size(int size);
int Get_Init_Size(int size);
mem_block* Split_Block(mem_block* block, size_t size);
mem_block* Find_Block(size_t size);
mem_block* Find_Smallest_Block(size_t size);
mem_block* Find_Largest_Block(size_t size);
int Mem_Init(int size, int policy);
void* Mem_Alloc(int size);
int Mem_Free(void* ptr);
int Mem_IsValid(void* ptr);
int Get_Size(void* ptr);
float Get_Fragmentation();
mem_block* Check_Memory(mem_block* block);
void Merge();
int Largest_Chunk_Size();
int Total_FreeMemory();

void Merge()
{
	mem_block* current = start;
	while(current != end)
	{	
		if(current->next != NULL)
		{
			if(current->next->status != 1)
			{
				int newSize = current->size + current->next->size;
				current->size = newSize;
				block_link(current, current->next->next);
			}
		}
		if(current->previous != NULL)
		{
			if(current->previous->status != 1)
			{
				int newSize = current->size + current->previous->size;
				current->size = newSize;
				block_link(current->previous->previous, current);
			}
		}
		current = current->next;
	}
}

mem_block* Check_Memory(mem_block* block)
{
	mem_block* temp = start;
	mem_block* found = NULL;
	while(temp != end)
	{
		if(block >= temp)
			found = temp;
		temp = temp->next;
	}
	return found;
}

int Get_Opt_Size(int size)
{
	int suitableSize = MIN_BLOCK_SIZE;
	while(suitableSize < size)
		suitableSize = suitableSize << 1;
	return suitableSize;
}

int Get_Init_Size(int size)
{
	int suitableSize = PAGE_SIZE;
	while(suitableSize < size)
	{
		suitableSize = suitableSize + PAGE_SIZE;
	}	
	return suitableSize;
}

mem_block* Split_Block(mem_block* block, size_t size)
{
	size_t remainder = block->size - size;
	if(remainder >= MIN_BLOCK_SIZE)
	{
		mem_block* newBlock = shift_block_ptr(block, size);
		newBlock->size = remainder;
		newBlock->status = 0;
		insert_block(block, newBlock);
		block->size = size;
		block->status = 1;
	} else
	{
		if(remainder >= 0)
			block->status = 1;
	}
	return block;
}

mem_block* Find_Block(size_t size)
{
	mem_block* temp = start;
	while(temp != end)
	{
		if(temp->size >= size && temp->size >= size)
			return Split_Block(temp, size);
		temp  = temp->next;
	}
	return NULL;
}

mem_block* Find_Smallest_Block(size_t size){

	mem_block* temp = start;
	mem_block* smallest = NULL;
	while(temp != end)
	{
		if(smallest == NULL)
		{
			if(temp->size >= size)
				smallest = temp;
		}
		else if(temp->size <= smallest->size && temp->size >= size)
		{
			smallest = temp;
			
		}	
		temp = temp->next;
	}
	if(smallest != NULL)
		return Split_Block(smallest, size);
	return NULL;
}

mem_block* Find_Largest_Block(size_t size){

	mem_block* temp = start;
	mem_block* largest = NULL;

	while(temp !=end)
	{
		if(largest == NULL)
		{
			if(temp->size >= size)
				largest = temp;
		}
		else if (temp->size >= largest->size && temp-> size >= size)
		{
			largest = temp;
		}
		temp = temp->next;
	}
	if(largest != NULL)
		return Split_Block(largest, size);
	return NULL;
}

int Largest_Chunk_Size()
{
	mem_block* temp;
	mem_block* largest = start;
	while(temp != end)
	{
		if(largest == NULL)
		{
			if(temp->status != 1)
				largest = temp;
		}
		else if(temp->size >= largest->size && temp->status != 1)
		{
			largest = temp;
		}
		temp = temp->next;
	}
	if(largest != NULL)
		return largest->size;
	return -1;
}

int Total_Free_Memory()
{
	int total = 0;
	mem_block* temp = start;
	while(temp != end)
	{
		if(temp->status != 1)
			total += temp->size;
		temp = temp->next;
	}
	return total;
}

int Mem_Init(int size, int policy)
{
	
	int pages;
	void *ptr;
	aPolicy = policy;
	int fd = open("/dev/zero", O_RDWR);
	size = size + MIN_BLOCK_SIZE;
	pages = Get_Init_Size(size);
	ptr = mmap(NULL, pages, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	start = (mem_block*)ptr;
	start->next = end;
	start->previous = NULL;
	start->size = size;
	start->status = 0;
//	block_data(start);
	end = block_end(start);	
	if(ptr == MAP_FAILED)
	{
		perror("mmap");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}


void* Mem_Alloc(int size)
{
	if (size == 0)
		return NULL;
	size += MIN_BLOCK_SIZE;
	size_t optSize = Get_Opt_Size(size);
	mem_block* newBlock;
	switch(aPolicy)
	{
		case 0:
			newBlock = Find_Block(optSize);
//			block_data(newBlock);
			break;
		case 1:
			newBlock = Find_Smallest_Block(optSize);
//			block_data(newBlock); 
			break;
		case 2:
			newBlock = Find_Largest_Block(optSize);
//			block_data(newBlock);
			break;
		default:
			return NULL;
			
	}
	return (void*)newBlock;	
}


int Mem_Free(void *ptr)
{
	if(Mem_IsValid((mem_block*)ptr))
	{
		mem_block* free = Check_Memory((mem_block*)ptr);
		free->status = 0;
		Merge();
		return 0;
	} else
	{
		return -1;
	}
}

int Mem_IsValid(void *ptr)
{
	mem_block* check = Check_Memory((mem_block*)ptr);
	if(check != NULL && check->status != 0)
		return 1;
	return 0;
}

int Mem_GetSize(void *ptr)
{
	mem_block* block = Check_Memory((mem_block*)ptr);
	if(block != NULL)
		return block->size;
	return -1;
}

float Mem_GetFragmentation()
{
	int chunk = Largest_Chunk_Size();
	int total = Total_Free_Memory();
	if(total == 0)
		return 1;
	return ((float)chunk/ (float)total);
	
}
