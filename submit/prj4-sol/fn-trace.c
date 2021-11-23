#include "fn-trace.h"
#include "x86-64_lde.h"

#include "memalloc.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum { INIT_SIZE = 2 };
// abstract declarations
static inline bool is_call(unsigned);
static inline bool is_ret(unsigned);

struct FnsDataImpl {
  int index;
  int nAlloc;
  FnInfo *elements;
};


/** adds element to dynamic array
 */
static void add_element(FnInfo element, FnsData *array) {
  unsigned INC = 4;
  if (array->index >= array->nAlloc) {
    array->elements = reallocChk(array->elements, (array->nAlloc += INC) * sizeof(FnInfo));
  }
  
  array->elements[array->index++] = element;
}

/** compare function used for qsort
 */
static int cmp_info(const void *p1, const void *p2) {
  return ((FnInfo *)p1)->address - ((FnInfo *)p2)->address;
}

void create_info(void *root, FnsData *array) {
  unsigned length = 0;
  unsigned nOutCalls = 0;
  FnInfo info = { .address = root, .length = 0, .nInCalls = 1, .nOutCalls = 0 };
  
  unsigned op_code;
  unsigned op_length;
  Lde *lde = new_lde();
  unsigned char *instr = (unsigned char *)root;

  do {
    op_code = *instr;
    op_length = get_op_length(lde, instr);
    printf("opCode: %x, opLength: %d\n", op_code, op_length);
    if (is_call(op_code)) nOutCalls++;
    instr += op_length;
    length += op_length;
  } while (!is_ret(op_code));

  free_lde(lde);

  info.length = length; info.nOutCalls = nOutCalls;
  printf("root: %p, length: %d, inCalls: %d, outCalls %d\n", info.address, info.length, info.nInCalls, info.nOutCalls); 
}

/** Return pointer to opaque data structure containing collection of
 *  FnInfo's for functions which are callable directly or indirectly
 *  from the function whose address is rootFn.
 */
const FnsData *
new_fns_data(void *rootFn)
{
  FnsData array = { .index = 0, .nAlloc = 0, .elements = NULL};
  create_info(rootFn, &array);
  //qsort(array.elements, array.index, sizeof(FnInfo), cmp_info);
  free(array.elements);
  exit(0);

  return NULL;
}

/** Free all resources occupied by fnsData. fnsData must have been
 *  returned by new_fns_data().  It is not ok to use to fnsData after
 *  this call.
 */
void
free_fns_data(FnsData *fnsData)
{
  free(fnsData->elements);
  //TODO
}

/** Iterate over all FnInfo's in fnsData.  Make initial call with NULL
 *  lastFnInfo.  Keep calling with return value as lastFnInfo, until
 *  next_fn_info() returns NULL.  Values must be returned sorted in
 *  increasing order by fnInfoP->address.
 *
 *  Sample iteration:
 *
 *  for (FnInfo *fnInfoP = next_fn_info(fnsData, NULL); fnInfoP != NULL;
 *       fnInfoP = next_fn_info(fnsData, fnInfoP)) {
 *    //body of iteration
 *  }
 *
 */
const FnInfo *
next_fn_info(const FnsData *fnsData, const FnInfo *lastFnInfo)
{
  //TODO
  return NULL;
}


/** recognized opcodes for calls and returns */
enum {
  CALL_OP = 0xE8,    //used to identify an external call which is traced

  //used to recognize the end of a function
  RET_FAR_OP = 0xCB,
  RET_FAR_WITH_POP_OP = 0xCA,
  RET_NEAR_OP = 0xC3,
  RET_NEAR_WITH_POP_OP = 0xC2
};

static inline bool is_call(unsigned op) { return op == CALL_OP; }
static inline bool is_ret(unsigned op) {
  return
    op == RET_NEAR_OP || op == RET_NEAR_WITH_POP_OP ||
    op == RET_FAR_OP || op == RET_FAR_WITH_POP_OP;
}

