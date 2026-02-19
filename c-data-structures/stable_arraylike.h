#ifndef C_DATA_STRUCTURES_STABLE_ARRAYLIKE_H_
#define C_DATA_STRUCTURES_STABLE_ARRAYLIKE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef STABLE_ARRAY_BLOCK_SIZE
#define STABLE_ARRAY_BLOCK_SIZE 64
#endif

#define DEFINE_STABLE_ARRAYLIKE(name, type)                          \
                                                                     \
  typedef struct {                                                   \
    type **blocks;                                                   \
    size_t size;                                                     \
    size_t num_blocks;                                               \
    size_t capacity_blocks;                                          \
  } name;                                                            \
                                                                     \
  typedef struct {                                                   \
    name *array;                                                     \
    size_t index;                                                    \
  } name##Iterator;                                                  \
                                                                     \
  /* Initialization and lifetime management */                       \
  bool name##_init(name *);                                          \
                                                                     \
  name *name##_create();                                             \
                                                                     \
  void name##_finalize(name *);                                      \
  void name##_delete(name *);                                        \
                                                                     \
  /* Back operations */                                              \
  void name##_push_back(name *const, type);                          \
  type *name##_push_back_ref(name *const);                           \
  bool name##_pop_back(name *const, type *ptr);                      \
  type name##_pop_back_unchecked(name *const);                       \
                                                                     \
  /* Random access mutation */                                       \
  bool name##_set(name *const, int32_t index, type);                 \
  bool name##_set_ref(name *const array, int32_t index, type **ptr); \
  type *name##_set_ref_unchecked(name *const array, int32_t index);  \
                                                                     \
  /* Random access lookup */                                         \
  bool name##_get(name *const, int32_t, type *ptr);                  \
  type name##_get_unchecked(name *const, int32_t);                   \
  bool name##_get_ref(name *const, int32_t, const type **ptr);       \
  bool name##_mutable_ref(name *const, int32_t, type **ptr);         \
  const type *name##_get_ref_unchecked(name *const, int32_t);        \
  type *name##_mutable_ref_unchecked(name *const, int32_t);          \
  bool name##_last(name *const, type *ptr);                          \
  type name##_last_unchecked(name *const);                           \
  bool name##_last_ref(name *const, const type **ptr);               \
  const type *name##_last_ref_unchecked(name *const);                \
                                                                     \
  /* Size and state */                                               \
  size_t name##_size(const name *const);                             \
  bool name##_is_empty(const name *const);                           \
                                                                     \
  /* Iteration */                                                    \
  void name##_iterator(name##Iterator *, name *const);               \
  bool name##_has_next(const name##Iterator *const);                 \
  void name##_next(name##Iterator *);                                \
  const type *name##_value(const name##Iterator *const);             \
  type *name##_mutable_value(const name##Iterator *const)

#define IMPL_STABLE_ARRAYLIKE(name, type)                                    \
                                                                             \
  /* --- Internal Helper: Accessor --- */                                    \
  static inline type *name##_internal_get(const name *const array,           \
                                          int32_t index) {                   \
    if (index < 0 || (size_t)index >= array->size) return NULL;              \
    size_t block_idx = index / STABLE_ARRAY_BLOCK_SIZE;                      \
    size_t offset = index % STABLE_ARRAY_BLOCK_SIZE;                         \
    return &array->blocks[block_idx][offset];                                \
  }                                                                          \
                                                                             \
  /* --- Initialization and lifetime management --- */                       \
  bool name##_init(name *array) {                                            \
    array->size = 0;                                                         \
    array->capacity_blocks = 4;                                              \
    array->num_blocks = 0;                                                   \
    array->blocks = (type **)calloc(array->capacity_blocks, sizeof(type *)); \
    return array->blocks != NULL;                                            \
  }                                                                          \
                                                                             \
  name *name##_create() {                                                    \
    name *array = (name *)malloc(sizeof(name));                              \
    if (array && !name##_init(array)) {                                      \
      free(array);                                                           \
      return NULL;                                                           \
    }                                                                        \
    return array;                                                            \
  }                                                                          \
                                                                             \
  void name##_finalize(name *array) {                                        \
    if (!array) return;                                                      \
    for (size_t i = 0; i < array->num_blocks; ++i) {                         \
      free(array->blocks[i]);                                                \
    }                                                                        \
    free(array->blocks);                                                     \
    array->blocks = NULL;                                                    \
    array->size = 0;                                                         \
  }                                                                          \
                                                                             \
  void name##_delete(name *array) {                                          \
    if (!array) return;                                                      \
    name##_finalize(array);                                                  \
    free(array);                                                             \
  }                                                                          \
                                                                             \
  /* --- Back operations --- */                                              \
  type *name##_push_back_ref(name *const array) {                            \
    size_t block_idx = array->size / STABLE_ARRAY_BLOCK_SIZE;                \
    if (block_idx >= array->num_blocks) {                                    \
      /* Need new block */                                                   \
      if (block_idx >= array->capacity_blocks) {                             \
        size_t new_cap = array->capacity_blocks * 2;                         \
        type **new_blocks =                                                  \
            (type **)realloc(array->blocks, new_cap * sizeof(type *));       \
        if (!new_blocks) return NULL;                                        \
        array->blocks = new_blocks;                                          \
        array->capacity_blocks = new_cap;                                    \
      }                                                                      \
      array->blocks[block_idx] =                                             \
          (type *)malloc(STABLE_ARRAY_BLOCK_SIZE * sizeof(type));            \
      if (!array->blocks[block_idx]) return NULL;                            \
      array->num_blocks++;                                                   \
    }                                                                        \
    type *res =                                                              \
        &array->blocks[block_idx][array->size % STABLE_ARRAY_BLOCK_SIZE];    \
    array->size++;                                                           \
    return res;                                                              \
  }                                                                          \
                                                                             \
  void name##_push_back(name *const array, type value) {                     \
    type *slot = name##_push_back_ref(array);                                \
    if (slot) *slot = value;                                                 \
  }                                                                          \
                                                                             \
  bool name##_pop_back(name *const array, type *ptr) {                       \
    if (array->size == 0) return false;                                      \
    if (ptr)                                                                 \
      *ptr = name##_pop_back_unchecked(array);                               \
    else                                                                     \
      array->size--;                                                         \
    return true;                                                             \
  }                                                                          \
                                                                             \
  type name##_pop_back_unchecked(name *const array) {                        \
    type val = *name##_internal_get(array, (int32_t)array->size - 1);        \
    array->size--;                                                           \
    return val;                                                              \
  }                                                                          \
                                                                             \
  /* --- Random access mutation --- */                                       \
  bool name##_set(name *const array, int32_t index, type value) {            \
    type *slot = name##_internal_get(array, index);                          \
    if (!slot) return false;                                                 \
    *slot = value;                                                           \
    return true;                                                             \
  }                                                                          \
                                                                             \
  bool name##_set_ref(name *const array, int32_t index, type **ptr) {        \
    type *slot = name##_internal_get(array, index);                          \
    if (!slot) return false;                                                 \
    if (ptr) *ptr = slot;                                                    \
    return true;                                                             \
  }                                                                          \
                                                                             \
  type *name##_set_ref_unchecked(name *const array, int32_t index) {         \
    return name##_internal_get(array, index);                                \
  }                                                                          \
                                                                             \
  /* --- Random access lookup --- */                                         \
  bool name##_get(name *const array, int32_t index, type *ptr) {             \
    type *slot = name##_internal_get(array, index);                          \
    if (!slot) return false;                                                 \
    if (ptr) *ptr = *slot;                                                   \
    return true;                                                             \
  }                                                                          \
                                                                             \
  type name##_get_unchecked(name *const array, int32_t index) {              \
    return *name##_internal_get(array, index);                               \
  }                                                                          \
                                                                             \
  bool name##_get_ref(name *const array, int32_t index, const type **ptr) {  \
    type *slot = name##_internal_get(array, index);                          \
    if (!slot) return false;                                                 \
    if (ptr) *ptr = (const type *)slot;                                      \
    return true;                                                             \
  }                                                                          \
                                                                             \
  bool name##_mutable_ref(name *const array, int32_t index, type **ptr) {    \
    type *slot = name##_internal_get(array, index);                          \
    if (!slot) return false;                                                 \
    if (ptr) *ptr = slot;                                                    \
    return true;                                                             \
  }                                                                          \
                                                                             \
  const type *name##_get_ref_unchecked(name *const array, int32_t index) {   \
    return (const type *)name##_internal_get(array, index);                  \
  }                                                                          \
                                                                             \
  type *name##_mutable_ref_unchecked(name *const array, int32_t index) {     \
    return name##_internal_get(array, index);                                \
  }                                                                          \
                                                                             \
  bool name##_last(name *const array, type *ptr) {                           \
    return name##_get(array, (int32_t)array->size - 1, ptr);                 \
  }                                                                          \
                                                                             \
  type name##_last_unchecked(name *const array) {                            \
    return name##_get_unchecked(array, (int32_t)array->size - 1);            \
  }                                                                          \
                                                                             \
  bool name##_last_ref(name *const array, const type **ptr) {                \
    return name##_get_ref(array, (int32_t)array->size - 1, ptr);             \
  }                                                                          \
                                                                             \
  const type *name##_last_ref_unchecked(name *const array) {                 \
    return name##_get_ref_unchecked(array, (int32_t)array->size - 1);        \
  }                                                                          \
                                                                             \
  /* --- Size and state --- */                                               \
  size_t name##_size(const name *const array) { return array->size; }        \
  bool name##_is_empty(const name *const array) { return array->size == 0; } \
                                                                             \
  /* --- Iteration --- */                                                    \
  void name##_iterator(name##Iterator *it, name *const array) {              \
    it->array = array;                                                       \
    it->index = 0;                                                           \
  }                                                                          \
                                                                             \
  bool name##_has_next(const name##Iterator *const it) {                     \
    return it->index < it->array->size;                                      \
  }                                                                          \
                                                                             \
  void name##_next(name##Iterator *it) { it->index++; }                      \
                                                                             \
  const type *name##_value(const name##Iterator *const it) {                 \
    return name##_get_ref_unchecked(it->array, (int32_t)it->index);          \
  }                                                                          \
                                                                             \
  type *name##_mutable_value(const name##Iterator *const it) {               \
    return name##_mutable_ref_unchecked(it->array, (int32_t)it->index);      \
  }

#ifdef __cplusplus
}
#endif

#endif /* C_DATA_STRUCTURES_STABLE_ARRAYLIKE_H_ */
