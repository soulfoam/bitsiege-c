#ifndef MEM_POOL__
#define MEM_POOL__

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

struct 
mem_pool
{
	void **head;
	void *mem;
	uint32_t blocks;
	uint32_t mem_size;
};

struct mem_pool
mem_pool_init(uint32_t blocks, uint32_t mem_size);

void
mem_pool_destroy(struct mem_pool *pool);

void*
mem_pool_get(struct mem_pool *pool);

void
mem_pool_put(struct mem_pool *pool, void *mem);

#endif


