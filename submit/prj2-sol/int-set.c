#include "int-set.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/** Abstract data type for set of int's.  Note that sets do not allow
 *  duplicates.
 */

typedef struct NodeStruct {
  int element;
  struct NodeStruct *succ;
} Node;

typedef struct {
  int nElements;
  Node dummy;
} Header;


/** Return a new empty int-set.  Returns NULL on error with errno set.
 */
void *newIntSet() {
  return calloc(1, sizeof(Header));
}

/** Return # of elements in intSet */
int nElementsIntSet(void *intSet) {
  const Header *header = (Header *)intSet;
  return header->nElements;
}

/** Return non-zero iff intSet contains element. */
int isInIntSet(void *intSet, int element) {
  Header *header = (Header *)intSet;
  for (Node *n = header->dummy.succ; n != NULL; n = n->succ) {
    if (n->element == element) return 1;
  }
  return 0;
}

/** Create a new Node with value linked into the set after Node n.
 *  Return pointer to the new Node, NULL on error. 
 */
static Node *linkNewNodeAfter(Node *n, int value){
  Node *new = malloc(sizeof(Node));
  if (!new) return NULL; //malloc failure
  new->element = value;
  new->succ = n->succ;
  n->succ = new;
  return new;
}

/** Remove Node after specified Node n, freeing it in memory.
 *  Link n to the successor of the unlinked Node.
 *  Return pointer to the new successor of n.
 */
static Node *unlinkNodeAfter(Node *n) {
  Node *temp = n->succ;
  n->succ = temp->succ;
  free(temp);
  return n->succ;
}

/** Change intSet by adding element to it.  Returns # of elements
 *  in intSet after addition.  Returns < 0 on error with errno
 *  set.
 */
int addIntSet(void *intSet, int element) {
  Header *header = (Header *)intSet;
  Node *n;
  for (n = &header->dummy; (n->succ != NULL) && (n->succ->element < element); n = n->succ){}
  assert(n->succ == NULL || n->succ->element >= element);
  if ((n->succ == NULL && n->element != element) || n->succ->element != element){
    if (!linkNewNodeAfter(n, element)) return -1;
    return ++header->nElements;
  }
  return header->nElements;
}

/** Change intSet by adding all elements in array elements[nElements] to
 *  it.  Returns # of elements in intSet after addition.  Returns
 *  < 0 on error with errno set.
 */
int addMultipleIntSet(void *intSet, const int elements[], int nElements) {
  int returnVal = -1;
  for (int i = 0; i < nElements; i++){
    if ((returnVal = addIntSet(intSet, elements[i])) < 0) break;
  }
  return returnVal;
}

/** Set intSetA to the union of intSetA and intSetB.  Return # of
 *  elements in the updated intSetA.  Returns < 0 on error.
 */
int unionIntSet(void *intSetA, void *intSetB) {
  Header *headerA = (Header *)intSetA;
  Header *headerB = (Header *)intSetB;
  Node *nAL;
  Node *nB;
  int addedElements = 0;
  for (nAL = &headerA->dummy, nB = headerB->dummy.succ; (nAL->succ != NULL) && (nB != NULL);){
    if (nAL->succ->element < nB->element){
      nAL = nAL->succ;
    }
    else if (nAL->succ->element == nB->element){
      nAL = nAL->succ;
      nB = nB->succ;
    }
    else if (nAL->succ->element > nB->element){
      nAL = linkNewNodeAfter(nAL, nB->element);
      addedElements++;
      nB = nB->succ;
    }
  }
  if (nB != NULL){
    for (; nB != NULL; nB = nB->succ){
      nAL = linkNewNodeAfter(nAL, nB->element);
      addedElements++;
    }
  }
  headerA->nElements += addedElements;
  return headerA->nElements;
}

/** Set intSetA to the intersection of intSetA and intSetB.  Return #
 *  of elements in the updated intSetA.  Returns < 0 on error.
 */
int intersectionIntSet(void *intSetA, void *intSetB) {
  Header *headerA = (Header *)intSetA;
  Header *headerB = (Header *)intSetB;
  Node *nAL;
  Node *nB;
  int removedElements = 0;

  for (nAL = &headerA->dummy, nB = headerB->dummy.succ; (nAL->succ != NULL) && (nB != NULL);){
    if (nAL->succ->element < nB->element) {
      unlinkNodeAfter(nAL);
      removedElements++;
    }
    else if (nAL->succ->element == nB->element){
      nAL = nAL->succ;
      nB = nB->succ;
    }
    else if (nAL->succ->element > nB->element){
      nB = nB->succ;
    }
  }

  if (nAL->succ != NULL){
    for (; nAL->succ != NULL;){
      unlinkNodeAfter(nAL);
      removedElements++;
    }
  }
  headerA->nElements -= removedElements;
  return headerA->nElements;
}

/** Free all resources used by previously created intSet. */
void freeIntSet(void *intSet) {
  Header *header = (Header *)intSet;
  Node *n1;
  for (Node *n = header->dummy.succ; n != NULL; n = n1){
    n1 = n->succ;
    free(n);
  }
  free(header);
}

/** Return a new iterator for intSet.  Returns NULL if intSet
 *  is empty.
 */
const void *newIntSetIterator(const void *intSet) {
  const Header *header = (Header *)intSet;
  return header->dummy.succ;
}

/** Return current element for intSetIterator. */
int intSetIteratorElement(const void *intSetIterator) {
  const Node *n = (Node *)intSetIterator;
  return n->element;
}

/** Step intSetIterator and return stepped iterator.  Return
 *  NULL if no more iterations are possible.
 */
const void *stepIntSetIterator(const void *intSetIterator) {
  const Node *n = (Node *)intSetIterator;
  return n->succ;
}
