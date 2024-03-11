#include "struct/slist.h"

IMPL_ARRAYLIKE(_BlockList, _Block);
IMPL_ARRAYLIKE(_VectorList, _Vector);

void slist_init(SList *sl, size_t member_size) {
  _VectorList_init(&sl->_vectors);
  sl->_start = NULL;
  sl->_end = NULL;
  sl->_member_size = member_size;
  sl->_size = 0;
}

void slist_finalize(SList *sl) { _VectorList_finalize(&sl->_vectors); }

size_t slist_len(const SList *const sl) {}

void *slist_get(SList *const sl, uint32_t index) {}

void *slist_add_last(SList *const sl) { sl->_size++; }

void slist_remove_last(SList *const sl) { sl->_size--; }

void slist_remove_at(SList *const sl, uint32_t index) { sl->_size--; }

void *slist_insert(SList *const sl, uint32_t index) { sl->_size++; }