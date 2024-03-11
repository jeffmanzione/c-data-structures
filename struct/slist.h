#ifndef STRUCT_SLIST_H_
#define STRUCT_SLIST_H_

#include <stdint.h>

#include "struct/arraylike.h"

typedef struct {
  void *_start;
  uint32_t _length;
  size_t _max_length;
} _Block;

DEFINE_ARRAYLIKE(_BlockList, _Block);

typedef struct {
  void *_arr;
  _BlockList _blocks;
} _Vector;

DEFINE_ARRAYLIKE(_VectorList, _Vector);

typedef struct {
  _VectorList _vectors;
  void *_start;
  void *_end;
  size_t _member_size;
  size_t _size;

} SList;

void slist_init(SList *sl, size_t member_size);
void slist_finalize(SList *sl);
size_t slist_len(const SList *const sl);
void *slist_get(SList *const sl, uint32_t index);
void *slist_add_last(SList *const sl);
void slist_remove_last(SList *const sl);
void slist_remove_at(SList *const sl, uint32_t index);
void *slist_insert(SList *const sl, uint32_t index);

#endif /* STRUCT_SLIST_H_ */