#ifndef STRUCT_SLIST_H_
#define STRUCT_SLIST_H_

#include <stdint.h>

#include "struct/alist.h"

typedef struct {
  size_t _member_size;
  size_t _size;
  AList blocks;
} SList;

void slist_init(SList *sl, size_t member_size);
void slist_finalize(SList *sl);
size_t slist_len(const SList *const sl);
void *slist_get(SList *const sl, uint32_t index);
void *slist_add_last(SList *const sl);
void slist_remove_last(SList *const sl);

#endif /* STRUCT_SLIST_H_ */