#include "mem_pool.h"
#include "log.h"

struct mem_pool
mem_pool_init(uint32_t blocks, uint32_t mem_size)
{
	struct mem_pool pool;
	
	pool.mem = malloc(blocks * mem_size);

	if (pool.mem == NULL) 
	{
		log_warn(
			"Memory pool failed to allocate memory in %u blocks with size of %u",
			blocks,
			mem_size);
	}

	pool.blocks = blocks;
	pool.mem_size = mem_size;

	void *end = pool.mem + (blocks * mem_size);
	for (void *v = pool.mem; v < end; v+= mem_size)
	{
		mem_pool_put(&pool, v);
	}

	log_trace("Memory pool initialized.");

	return pool;

}

void
mem_pool_destroy(struct mem_pool *pool)
{
	free(pool->mem);
}

void*
mem_pool_get(struct mem_pool *pool)
{
	if (pool->head == NULL)
	{
		log_warn("Memory pool head is null?");
		return NULL;
	}

	void *tmp = pool->head;

	pool->head = *pool->head;

	return tmp;
}

void
mem_pool_put(struct mem_pool *pool, void *mem)
{
	void *tmp = pool->head;

	pool->head = mem;

	*pool->head = tmp;
}




