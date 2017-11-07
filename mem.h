/* DO NOT CHANGE THIS FILE */                                              
                                                                           
#ifndef MEM_H                                                              
#define MEM_H                                                              
                                                                           
#define MEM_POLICY_FIRSTFIT 0                                              
#define MEM_POLICY_BESTFIT  1                                              
#define MEM_POLICY_WORSTFIT 2                                              
                                                                           
/* This function is called one time by a process using Mem_*               
 * routines. size is the number of bytes that you should request from      
 * the OS using mmap(). Note that you may need to round up this amount     
 * so that you request memory in units of the page size (see the           
 * manpages for getpagesize()). Note that you need to use this             
 * allocated memory for your own data structures as well; that is,         
 * your infrastructure for tracking the mapping from addresses to          
 * memory objects has to be placed in this region as well (it’s            
 * self-contained).  policy indicates the method for managing the free     
 * list (0 for first-fit, 1 for best-fit=1, and 2 for worst-fit) when      
 * choosing a chunk of memory for allocation. First-fit uses the first     
 * free chunk that is big enough; best-fit uses the smallest chunk         
 * that is big enough; and worst-fit uses the largest chunk. The           
 * function returns 0 if successful; otherwise, the function returns       
 * -1. */                                                                  
int Mem_Init(int size, int policy);                                        
                                                                           
/* This function is similar to the library function malloc().              
 * Mem_Alloc takes as input the size in bytes of the object to be          
 * allocated and returns a pointer to the start of that object. The        
 * function returns NULL if there is not enough free space within          
 * memory region allocated by Mem_Init to satisfy this request. Free       
 * space is selected according to the policy specified by Mem_Init. */     
void* Mem_Alloc(int size);                                                 
                                                                           
/* This function frees the memory object that ptr falls within. Just       
 * like with the standard free() , if ptr is NULL, then no operation       
 * is performed. The function returns 0 on success and -1 if ptr does      
 * not fall within a currently allocated object (note that this            
 * includes the case where the object was already freed with               
 * Mem_Free). */                                                           
int Mem_Free(void* ptr);                                                   
                                                                           
/* This function returns 1 if ptr falls within a currently allocated       
 * object and 0 if it does not. You may find this function useful when     
 * debugging your memory allocator. */                                     
int Mem_IsValid(void* ptr);                                                
                                                                           
/* If ptr falls within the range of a currently allocated object, then     
 * this function returns the size in bytes of that object; otherwise,      
 * the function returns -1. You may find this function useful when         
 * debugging your memory allocator. */                                     
int Mem_GetSize(void* ptr);                                                
                                                                           
/* This function returns the fragmentation factor, which is defined as     
 * the size of the largest free chunk divided by the total free            
 * memory.  If the fragmentation factor is 1, it means there’s no          
 * fragmentation (i.e., there’s only one free chunk); if it is close       
 * to 0, the entire free memory is fragmented among small chunks. If       
 * "mem.h" 62L, 2883C                                                         
