#ifndef STRUCT_SLIST_H_
#define STRUCT_SLIST_H_

#include <stdint.h>

#include "struct/alist.h"

typedef struct {
  size_t _member_size;
  size_t _size;
  AList _blocks;
} SList;

typedef struct {
  const SList *_list;
  int _i;
} SL_iter;

void slist_init(SList *sl, size_t member_size);
void slist_finalize(SList *sl);
size_t slist_len(const SList *const sl);
void *slist_get(SList *const sl, uint32_t index);
void *slist_add_last(SList *const sl);

SL_iter slist_iter(const SList *const sl);
bool sl_has(SL_iter *iter);
void sl_inc(SL_iter *iter);
void *sl_value(SL_iter *iter);

#endif /* STRUCT_SLIST_H_ */