// alist.h
//
// Created on: Jan 14, 2018
//     Author: Jeff

#ifndef STRUCT_ALIST_H_
#define STRUCT_ALIST_H_

#include <stddef.h>
#include <stdint.h>

#include "alloc/alloc.h"
#include "util/util.h"

typedef void (*ESwapper)(void *, void *);
typedef void (*EAction)(void *);

#define DEFAULT_ARRAY_SZ 6

typedef struct {
  char *_arr;
  size_t _len, _table_sz, _obj_sz;
} AList;

// Initializes an alist.
#define alist_init(e, type, table_sz)                                          \
  __alist_init(e, ALLOC_ARRAY(type, table_sz), sizeof(type), table_sz)
void __alist_init(AList *e, void *arr, size_t obj_sz, size_t table_sz);

// Allocates an alist and initializes it.
#define alist_create(type, table_sz)                                           \
  __alist_create(ALLOC_ARRAY(type, table_sz), sizeof(type), table_sz)
AList *__alist_create(void *arr, size_t obj_sz, size_t table_sz);

// Cleans up an alist. It cannot be used after alist_finalize() is called.
void alist_finalize(AList *const a);

// Cleans up an alist and deallocates it.
void alist_delete(AList *const a);

// Appends an item to an alist.
size_t alist_append(AList *const a, const void *v);

// Allocates the space for a new element in an alist and returns a pointer to
// it.
void *alist_add(AList *const a);

// Removes the last element of the list.
void alist_remove_last(AList *const a);

// Removes the element at the given index in the list.
void alist_remove_at(AList *const e, int index);

// Returns a pointer to the item at index [i].
void *alist_get(const AList *const a, uint32_t i);

// Returns the number of items in the alist.
size_t alist_len(const AList *const a);

// Sorts an alist using the comparator and swapper.
void alist_sort(AList *const a, Comparator c, ESwapper eswap);

// Iterates through an alist executing action() on each item sequentially.
void alist_iterate(const AList *const a, EAction action);

typedef struct {
  const AList *_list;
  int _i;
  bool _is_reverse;
} AL_iter;

// Returns an iterator on the alist.
AL_iter alist_iter(const AList *const a);

// Returns an iterator from the end to the beginning of the alist.
AL_iter alist_riter(const AList *const a);

// Returns the value at the current position of the interator in the alist.
void *al_value(AL_iter *iter);

// Increments the iterator to the next item in the alist.
void al_inc(AL_iter *iter);

// Returns true if there are more items remaining in the alist.
bool al_has(AL_iter *iter);

#endif /* STRUCT_ALIST_H_ */
