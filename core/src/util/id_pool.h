#ifndef ID_POOL__
#define ID_POOL__

#include <inttypes.h>

uint16_t*
id_pool_init(uint16_t size);

uint16_t
id_pool_get(uint16_t *pool, uint16_t size);

void
id_pool_put(uint16_t *pool, uint16_t id, uint16_t size);

void
id_pool_destroy(uint16_t *pool);

#endif

