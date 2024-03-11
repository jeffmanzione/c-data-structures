#ifndef STRUCT_SLIST_H_
#define STRUCT_SLIST_H_

#include <stdint.h>

typedef struct {
  void *_map;
  void *_start;
  void *_end;
  size_t _member_size;

} SList;

void slist_init(SList *sl, size_t member_size);
size_t slist_len(const SList *const sl);
void *slist_get(SList *const sl, uint32_t index);
void *slist_add_last(SList *const sl);
void slist_remove(SList *const sl);

#endif /* STRUCT_SLIST_H_ */