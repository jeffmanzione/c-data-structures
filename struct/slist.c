#include "struct/slist.h"

#include "alloc/alloc.h"

#define SLIST_BLOCK_SIZE 32

void slist_init(SList *sl, size_t member_size) {
  __alist_init(&sl->_blocks, ALLOC_ARRAY(char, member_size *SLIST_BLOCK_SIZE),
               member_size, SLIST_BLOCK_SIZE);
  sl->_member_size = member_size;
  sl->_size = 0;
}

void slist_finalize(SList *sl) { alist_finalize(&sl->_blocks); }

size_t slist_len(const SList *const sl) { return sl->_size; }

void *slist_get(SList *const sl, uint32_t index) {}

void *slist_add_last(SList *const sl) {
  if (sl->_size == alist_len(&sl->_blocks) * SLIST_BLOCK_SIZE) {
    *(char *)alist_add(&sl->_blocks) =
        ALLOC_ARRAY(char, sl->_member_size *SLIST_BLOCK_SIZE);
  }
  char *last = alist_get(&sl->_blocks, alist_len(&sl->_blocks) - 1);
  sl->_size++;
}

void slist_remove_last(SList *const sl) { sl->_size--; }
