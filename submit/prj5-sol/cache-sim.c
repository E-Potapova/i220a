#include "cache-sim.h"

#include "memalloc.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

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
  unsigned replacement = params->replacement;
  // 3D array (ragged?)
  unsigned int ***cache = mallocChk((numSets+1) * sizeof(unsigned int **));
  cache[0] = mallocChk(sizeof(unsigned int *));
  cache[0][0] = mallocChk(5 * sizeof(unsigned int));
  for (int i = 1; i < (numSets+1); i++) {
    cache[i] = mallocChk(numLines * sizeof(unsigned int *));
    for (int j = 0; j < numLines; j++) {
      // each line has [valid, tag, accessOrder], so size of 3
      cache[i][j] = mallocChk(3 * sizeof(unsigned int));
    }
  }
  cache[0][0][0] = setBits;
  cache[0][0][1] = numLines;
  cache[0][0][2] = lineBits;
  cache[0][0][3] = memAddrBits;
  cache[0][0][4] = replacement;
  return (void *)cache;
}

/** Free all resources used by cache-simulation structure *cache */
void
free_cache_sim(CacheSim *cache)
{
  unsigned ***cacheSim = (unsigned int ***)cache;
  unsigned int numSets = 1<<(cacheSim[0][0][0]);
  unsigned numLines = cacheSim[0][0][1];
  for (int i = 1; i < (numSets+1); i++) {
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
  // default to CACHE_HIT
  CacheResult r = { 0, 0 };
 
  unsigned setBits = cacheSim[0][0][0];
  unsigned numLines = cacheSim[0][0][1];
  unsigned lineBits = cacheSim[0][0][2];
  unsigned memAddrBits = cacheSim[0][0][3];
  unsigned replacement = cacheSim[0][0][4];
  
  unsigned long addrMask;
  // if using all 64 bits then method for creating mask will overflow and won't work
  if (memAddrBits == 64) addrMask = 0xFFFFFFFFFFFFFFFF;
  else addrMask = ((1UL<<memAddrBits) - 1);

  unsigned long tagMask = addrMask & ~((1UL<<(setBits+lineBits)) - 1);
  unsigned int tag = (addr & tagMask)>>(setBits+lineBits);

  unsigned long setMask = ((1<<(setBits+lineBits)) - 1) & ~((1<<lineBits) - 1);
  unsigned int setNum = (addr & setMask)>>lineBits;
  
  // get set pointed by address
  unsigned int **set = cacheSim[setNum+1];
  unsigned emptyLine = -1;
  for (int i = 0; i < numLines; i++) {
    // valid is set and tag matches tag from address, meaning HIT
    if ((set[i][0] == 1) && (set[i][1] == tag)) {
      // update access order of lines
      if (set[i][2] != numLines*2) {
	for (int l = 0; l < numLines; l++) {
	  if (i == l) set[l][2] = numLines*2;
	  else {
	    if (--set[l][2] < 0) set[l][2] = 0;
	  }
	}
      }
      return r;
    }
    // save first occurance of an invalid line in case we miss
    if ((emptyLine == -1) && (set[i][0] == 0)) {
      emptyLine = i;
    }
  }
  // MISS NO REPLACE
  if (emptyLine != -1) {
    set[emptyLine][0] = 1;
    set[emptyLine][1] = tag;
    // update access order
    if (set[emptyLine][2] != numLines*2) {
      for (int l = 0; l < numLines; l++) {
	if (l == emptyLine) set[emptyLine][2] = numLines*2;
	else {
	  if (--set[l][2] < 0) set[l][2] = 0;
	}
      }
    }
    CacheResult result = { 1, 0 };
    return result;
  }
  // MISS WITH REPLACE
  else {
    unsigned int lineToReplace = 0;
    // least recently used
    if (replacement == 0) {
      unsigned int lru = numLines*2 + 1;
      unsigned int lruLine = 0;
      for (int l = 0; l < numLines; l++) {
	if (set[l][2] < lru) {
	  lruLine = l;
	  lru = set[l][2];
	}
      }
      lineToReplace = lruLine;
    }
    // most recently used
    else if (replacement == 1) {
      unsigned int mru = 0;
      unsigned int mruLine = 0;
      for (int l = 0; l < numLines; l++) {
	if (set[l][2] > mru){
	  mruLine = l;
	  mru = set[l][2];
	}
      }
      lineToReplace = mruLine;
    }
    // random
    else if (replacement == 2) {
      lineToReplace = rand() % numLines;
    }
    // update access order
    if (set[lineToReplace][2] != numLines*2) {
      for (int l = 0; l < numLines; l++) {
	if (l == lineToReplace) set[lineToReplace][2] = numLines*2;
	else {
	  if (--set[l][2] < 0) set[l][2] = 0;
	}
      }
    }
    // replace specified line in set
    unsigned int replacedTag = set[lineToReplace][1];
    MemAddr replacedAddr = ((replacedTag<<setBits) | setNum)<<lineBits;
    set[lineToReplace][1] = tag;
    CacheResult result = { 2, replacedAddr };
    return result;
  }
  return r;
}
