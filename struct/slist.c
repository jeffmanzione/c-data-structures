#include "struct/slist.h"

#include "alloc/alloc.h"
#include "debug/debug.h"

#define SLIST_BLOCK_SIZE 4

void slist_init(SList *sl, size_t member_size) {
  ASSERT(NOT_NULL(sl));
  ASSERT(member_size > 0);
  alist_init(&sl->_blocks, char *, 6);
  sl->_member_size = member_size;
  sl->_size = 0;
}

void slist_finalize(SList *sl) {
  ASSERT(NOT_NULL(sl));
  AL_iter iter = alist_iter(&sl->_blocks);
  for (; al_has(&iter); al_inc(&iter)) {
    DEALLOC(*(char **)al_value(&iter));
  }
  alist_finalize(&sl->_blocks);
}

size_t slist_len(const SList *const sl) {
  ASSERT(NOT_NULL(sl));
  return sl->_size;
}

void *slist_get(SList *const sl, uint32_t index) {
  ASSERT(NOT_NULL(sl));
  ASSERT(index >= 0, index < sl->_size);
  char *block = *(char **)alist_get(&sl->_blocks, index / SLIST_BLOCK_SIZE);
  return block + (index % SLIST_BLOCK_SIZE) * sl->_member_size;
}

void *slist_add_last(SList *const sl) {
  ASSERT(NOT_NULL(sl));
  if (sl->_size == alist_len(&sl->_blocks) * SLIST_BLOCK_SIZE) {
    *(char **)alist_add(&sl->_blocks) =
        ALLOC_ARRAY2(char, sl->_member_size *SLIST_BLOCK_SIZE);
  }
  char *last = *(char **)alist_get(&sl->_blocks, alist_len(&sl->_blocks) - 1);
  void *to_return =
      last + (sl->_size % SLIST_BLOCK_SIZE) * sizeof(char) * sl->_member_size;
  sl->_size++;
  return to_return;
}

SL_iter slist_iter(const SList *const sl) {
  SL_iter iter = {._list = sl, ._i = 0};
  return iter;
}

bool sl_has(SL_iter *iter) { return iter->_i < slist_len(iter->_list); }

void sl_inc(SL_iter *iter) { ++iter->_i; }

void *sl_value(SL_iter *iter) {
  return slist_get((SList *)iter->_list, iter->_i);
}
