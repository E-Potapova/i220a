#include "cache-sim.h"

#include "memalloc.h"

#include <stdbool.h>
#include <stddef.h>

/** Create and return a new cache-simulation structure for a
 *  cache for main memory withe the specified cache parameters params.
 *  No guarantee that *params is valid after this call.
 */
CacheSim *
new_cache_sim(const CacheParams *params)
{
  unsigned setBits = params->nSetBits;
  unsigned int numSets = 1<<setBits;
  unsigned numLines = params->nLinesPerSet;
  unsigned lineBits = params->nLineBits;
  unsigned memAddrBits = params->nMemAddrBits;
  // 3D array (ragged?)
  unsigned int ***cache = mallocChk(numSets * sizeof(unsigned int **));
  cache[0] = mallocChk(sizeof(unsigned int *));
  cache[0][0] = mallocChk(4 * sizeof(unsigned int));
  for (int i = 1; i < numSets; i++) {
    cache[i] = mallocChk(numLines * sizeof(unsigned int *));
    for (int j = 0; j < numLines; j++) {
      // each line has [valid, tag], so size of 2
      cache[i][j] = mallocChk(2 * sizeof(unsigned int));
    }
  }
  cache[0][0][0] = setBits;
  cache[0][0][1] = numLines;
  cache[0][0][2] = lineBits;
  cache[0][0][3] = memAddrBits;
  return (void *)cache;
}

/** Free all resources used by cache-simulation structure *cache */
void
free_cache_sim(CacheSim *cache)
{
  unsigned int ***cacheSim = (unsigned int ***)cache;
  unsigned int numSets = 1<<(cacheSim[0][0][0]);
  unsigned int numLines = cacheSim[0][0][1];
  for (int i = 1; i < numSets; i++) {
    for (int j = 0; j < numLines; j++) {
      free(cacheSim[i][j]);
    }
    free(cacheSim[i]);
  }
  free(cacheSim[0][0]);
  free(cacheSim[0]);
  free(cacheSim);
}

/** Return result for requesting addr from cache */
CacheResult
cache_sim_result(CacheSim *cache, MemAddr addr)
{
  unsigned int ***cacheSim = (unsigned int ***)cache;
  CacheResult result = { 0, 0 };
  // do cache stuff
  unsigned int setBits = cacheSim[0][0][0];
  unsigned int numLines = cacheSim[0][0][1];
  unsigned int lineBits = cacheSim[0][0][2];
  unsigned int memAddrBits = cacheSim[0][0][3];

  unsigned long tagMask = ((1<<memAddrBits) - 1) & ~((1<<(setBits+lineBits)) - 1);
  unsigned int tag = (addr & tagMask)>>(setBits+lineBits);

  unsigned long setMask = ((1<<(setBits+lineBits)) - 1) & ~((1<<lineBits) - 1);
  unsigned int set = (addr & setMask)>>lineBits;

  // get set from cacheSim[set]
  // look through each line checking if line[0] == 1 && line[1] == tag
  // if yes, its in the cache and a HIT
  // if not, its a MISS; place into cache
  // if valid was already set in all lines of set, we are REPLACING
  // if valid was not set in at least one line, we are NOT replacing
  return result;
}
