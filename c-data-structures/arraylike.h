#ifndef C_DATA_STRUCTURES_ARRAYLIKE_H_
#define C_DATA_STRUCTURES_ARRAYLIKE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file arraylike.h
 *
 * @brief Macro-based, type-safe dynamic array implementation for C.
 *
 * This header provides a pair of macros, DEFINE_ARRAYLIKE and IMPL_ARRAYLIKE,
 * that together generate a dynamic, array-backed sequence container for an
 * arbitrary element type.
 *
 * The design goals are:
 *  - Type safety without void pointers
 *  - Header-only API surface, implementation generated via macros
 *  - Familiar array semantics with additional convenience operations
 *  - Explicit checked vs. unchecked access patterns
 *
 * The generated container supports:
 *  - Push/pop at both ends
 *  - Random access get/set (with bounds-checked and unchecked variants)
 *  - Automatic capacity growth
 *  - Appending other arrays or ranges
 *  - Simple forward iteration
 *
 * Memory management:
 *  - The array owns a contiguous heap buffer (`table`)
 *  - Capacity grows in fixed-size chunks (DEFAULT_TABLE_SIZE)
 *  - Shrinking does not reduce capacity, only logical size
 *
 * Error handling:
 *  - Functions returning `bool` indicate failure for invalid indices or
 *    insufficient size
 *  - Functions suffixed with `_unchecked` assume valid preconditions and may
 *    invoke undefined behavior if misused
 *  - Allocation failures are guarded with `assert`
 *
 * Usage pattern:
 *
 *   // In a header or source file:
 *   DEFINE_ARRAYLIKE(IntArray, int);
 *
 *   // In exactly one source file:
 *   IMPL_ARRAYLIKE(IntArray, int);
 *
 *   // Use as:
 *   IntArray arr;
 *   IntArray_init(&arr);
 *   IntArray_push_back(&arr, 42);
 *
 * This API is intended for low-level systems code where performance,
 * predictability, and explicit control are preferred over opaque abstractions.
 */

/**
 * Default capacity granularity.
 *
 * All allocations are rounded up to a multiple of this value. This keeps
 * growth predictable and avoids frequent reallocations for small increments.
 */
#define DEFAULT_TABLE_SIZE 8

/**
 * @macro DEFINE_ARRAYLIKE
 *
 * @brief Declares a dynamic array type and its public API.
 *
 * This macro defines:
 *  - The array structure
 *  - An associated iterator type
 *  - Function prototypes for all supported operations
 *
 * @param name  Base name for the generated type and functions
 * @param type  Element type stored in the array
 */
#define DEFINE_ARRAYLIKE(name, type)                                          \
                                                                              \
  /**                                                                         \
   * Dynamic array structure.                                                 \
   *                                                                          \
   * - `capacity` is the allocated length of `table`                          \
   * - `size` is the number of logically present elements                     \
   * - `table` is a contiguous buffer of `capacity` elements                  \
   */                                                                         \
  typedef struct name##_ name;                                                \
  struct name##_ {                                                            \
    size_t capacity;                                                          \
    size_t size;                                                              \
    type *table;                                                              \
  };                                                                          \
                                                                              \
  /**                                                                         \
   * Forward iterator over the array.                                         \
   *                                                                          \
   * The iterator remains valid as long as the underlying array is not        \
   * structurally modified (push/pop/resize).                                 \
   */                                                                         \
  typedef struct {                                                            \
    int32_t index;                                                            \
    name *array;                                                              \
  } name##Iterator;                                                           \
                                                                              \
  /* Initialization and lifetime management */                                \
  bool name##_init_capacity(name *, size_t capacity);                         \
  bool name##_init(name *);                                                   \
                                                                              \
  name *name##_create();                                                      \
  name *name##_create_capacity(size_t capacity);                              \
  name *name##_create_copy(const type input[], size_t capacity);              \
                                                                              \
  void name##_finalize(name *);                                               \
  void name##_delete(name *);                                                 \
  void name##_clear(name *const);                                             \
                                                                              \
  /* Shrinking operations */                                                  \
  bool name##_lshrink(name *const array, size_t amount);                      \
  bool name##_rshrink(name *const array, size_t amount);                      \
                                                                              \
  /* Front operations */                                                      \
  void name##_push_front(name *const, type);                                  \
  type *name##_push_front_ref(name *const);                                   \
  bool name##_pop_front(name *const array, type *ptr);                        \
  type name##_pop_front_unchecked(name *const);                               \
                                                                              \
  /* Back operations */                                                       \
  void name##_push_back(name *const, type);                                   \
  type *name##_push_back_ref(name *const);                                    \
  bool name##_pop_back(name *const, type *ptr);                               \
  type name##_pop_back_unchecked(name *const);                                \
                                                                              \
  /* Random access mutation */                                                \
  bool name##_set(name *const, int32_t index, type);                          \
  bool name##_set_ref(name *const array, int32_t index, type **ptr);          \
  type *name##_set_ref_unchecked(name *const array, int32_t index);           \
                                                                              \
  /* Random access lookup */                                                  \
  bool name##_get(name *const, int32_t, type *ptr);                           \
  type name##_get_unchecked(name *const, int32_t);                            \
  bool name##_get_ref(name *const, int32_t, const type **ptr);                \
  bool name##_mutable_ref(name *const, int32_t, type **ptr);                  \
  const type *name##_get_ref_unchecked(name *const, int32_t);                 \
  type *name##_mutable_ref_unchecked(name *const, int32_t);                   \
  bool name##_last(name *const, type *ptr);                                   \
  type name##_last_unchecked(name *const);                                    \
  bool name##_last_ref(name *const, const type **ptr);                        \
  const type *name##_last_ref_unchecked(name *const);                         \
                                                                              \
  /* Removal */                                                               \
  bool name##_remove(name *const, int32_t, type *ptr);                        \
  type name##_remove_unchecked(name *const, int32_t);                         \
                                                                              \
  /* Size and state */                                                        \
  size_t name##_size(const name *const);                                      \
  bool name##_is_empty(const name *const);                                    \
                                                                              \
  /* Copying and concatenation */                                             \
  name *name##_copy(const name *const);                                       \
  void name##_append(name *const head, const name *const tail);               \
  bool name##_append_range(name *const head, const name *const tail,          \
                           int32_t tail_range_start, int32_t tail_range_end); \
                                                                              \
  /* Iteration */                                                             \
  void name##_iterator(name##Iterator *, name *const);                        \
  bool name##_has_next(const name##Iterator *const);                          \
  void name##_next(name##Iterator *);                                         \
  const type *name##_value(const name##Iterator *const);                      \
  type *name##_mutable_value(const name##Iterator *const)

/**
 * @macro IMPL_ARRAYLIKE
 *
 * @brief Generates the implementation for a previously declared array type.
 *
 * This macro must be invoked exactly once per array type, typically in a
 * `.c` file. It provides all function definitions declared by
 * DEFINE_ARRAYLIKE.
 *
 * The implementation assumes:
 *  - All pointers passed to checked functions are non-NULL
 *  - `_unchecked` functions are called only with valid indices
 */
#define IMPL_ARRAYLIKE(name, type)                                             \
                                                                               \
  bool name##_init_capacity(name *array, size_t capacity) {                    \
    if (capacity == 0) {                                                       \
      return false;                                                            \
    }                                                                          \
    array->table = (type *)calloc((array->capacity = capacity), sizeof(type)); \
    assert(array->table != NULL);                                              \
    array->size = 0;                                                           \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool name##_init(name *array) {                                              \
    return name##_init_capacity(array, DEFAULT_TABLE_SIZE);                    \
  }                                                                            \
                                                                               \
  name *name##_create() {                                                      \
    name *array = (name *)malloc(sizeof(name));                                \
    assert(array != NULL);                                                     \
    name##_init(array);                                                        \
    return array;                                                              \
  }                                                                            \
                                                                               \
  name *name##_create_capacity(size_t capacity) {                              \
    name *array = (name *)malloc(sizeof(name));                                \
    assert(array != NULL);                                                     \
    name##_init_capacity(array, capacity);                                     \
    return array;                                                              \
  }                                                                            \
                                                                               \
  name *name##_create_copy(const type input[], size_t capacity) {              \
    name *array = (name *)malloc(sizeof(name));                                \
    assert(array != NULL);                                                     \
    name##_init_capacity(                                                      \
        array, ((capacity + DEFAULT_TABLE_SIZE - 1) / DEFAULT_TABLE_SIZE) *    \
                   DEFAULT_TABLE_SIZE);                                        \
    memmove(array->table, input, capacity * sizeof(type));                     \
    array->size = capacity;                                                    \
    return array;                                                              \
  }                                                                            \
                                                                               \
  void name##_finalize(name *array) {                                          \
    assert(array != NULL);                                                     \
    free(array->table);                                                        \
  }                                                                            \
                                                                               \
  void name##_delete(name *array) {                                            \
    assert(array != NULL);                                                     \
    name##_finalize(array);                                                    \
    free(array);                                                               \
  }                                                                            \
                                                                               \
  inline void name##_ensure_capacity(name *const array,                        \
                                     size_t need_to_accomodate) {              \
    assert(array != NULL);                                                     \
    size_t new_capacity =                                                      \
        ((need_to_accomodate + DEFAULT_TABLE_SIZE - 1) / DEFAULT_TABLE_SIZE) * \
        DEFAULT_TABLE_SIZE;                                                    \
    if (new_capacity <= array->capacity) {                                     \
      return;                                                                  \
    }                                                                          \
    array->table = (type *)realloc(array->table, sizeof(type) * new_capacity); \
    assert(array->table != NULL);                                              \
    array->capacity = new_capacity;                                            \
    memset(array->table + array->size, 0x0,                                    \
           (array->capacity - array->size) * sizeof(type));                    \
  }                                                                            \
                                                                               \
  static inline void name##_shift_left(name *const array, int32_t start,       \
                                       int32_t amount) {                       \
    assert(amount > 0 && start >= amount);                                     \
    memmove(array->table + start - amount, array->table + start,               \
            (array->size - start) * sizeof(type));                             \
  }                                                                            \
                                                                               \
  static inline void name##_shift_right(name *const array, int32_t start,      \
                                        int32_t amount) {                      \
    assert(amount > 0);                                                        \
    name##_ensure_capacity(array, array->size + amount);                       \
    memmove(array->table + start + amount, array->table + start,               \
            (array->size - start) * sizeof(type));                             \
    memset(array->table + start, 0x0, amount * sizeof(type));                  \
  }                                                                            \
                                                                               \
  void name##_clear(name *const array) {                                       \
    assert(array != NULL);                                                     \
    array->size = 0;                                                           \
  }                                                                            \
                                                                               \
  void name##_push_front(name *const array, type elt) {                        \
    assert(array != NULL);                                                     \
    if (array->size > 0) {                                                     \
      name##_shift_right(array, 0, 1);                                         \
    }                                                                          \
    array->table[0] = elt;                                                     \
    array->size++;                                                             \
  }                                                                            \
                                                                               \
  type *name##_push_front_ref(name *const array) {                             \
    assert(array != NULL);                                                     \
    if (array->size > 0) {                                                     \
      name##_shift_right(array, 0, 1);                                         \
    }                                                                          \
    array->size++;                                                             \
    return array->table;                                                       \
  }                                                                            \
                                                                               \
  type name##_pop_front_unchecked(name *const array) {                         \
    assert(array != NULL);                                                     \
    type to_return = array->table[0];                                          \
    if (array->size > 1) {                                                     \
      name##_shift_left(array, 1, 1);                                          \
    }                                                                          \
    array->size--;                                                             \
    return to_return;                                                          \
  }                                                                            \
                                                                               \
  bool name##_pop_front(name *const array, type *ptr) {                        \
    assert(array != NULL);                                                     \
    if (array->size == 0) {                                                    \
      return false;                                                            \
    }                                                                          \
    *ptr = array->table[0];                                                    \
    if (array->size > 1) {                                                     \
      name##_shift_left(array, 1, 1);                                          \
    }                                                                          \
    array->size--;                                                             \
    return true;                                                               \
  }                                                                            \
                                                                               \
  void name##_push_back(name *const array, type elt) {                         \
    assert(array != NULL);                                                     \
    name##_ensure_capacity(array, array->size + 1);                            \
    array->table[array->size++] = elt;                                         \
  }                                                                            \
                                                                               \
  type *name##_push_back_ref(name *const array) {                              \
    assert(array != NULL);                                                     \
    name##_ensure_capacity(array, array->size + 1);                            \
    return array->table + array->size++;                                       \
  }                                                                            \
                                                                               \
  type name##_pop_back_unchecked(name *const array) {                          \
    assert(array != NULL);                                                     \
    type to_return = array->table[array->size - 1];                            \
    array->size--;                                                             \
    return to_return;                                                          \
  }                                                                            \
                                                                               \
  bool name##_pop_back(name *const array, type *ptr) {                         \
    assert(array != NULL);                                                     \
    if (array->size == 0) {                                                    \
      return false;                                                            \
    }                                                                          \
    *ptr = array->table[array->size - 1];                                      \
    array->size--;                                                             \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool name##_lshrink(name *const array, size_t amount) {                      \
    assert(array != NULL);                                                     \
    if (array->size < amount) {                                                \
      return false;                                                            \
    }                                                                          \
    name##_shift_left(array, amount, amount);                                  \
    array->size -= amount;                                                     \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool name##_rshrink(name *const array, size_t amount) {                      \
    assert(array != NULL);                                                     \
    if (array->size < amount) {                                                \
      return false;                                                            \
    }                                                                          \
    array->size -= amount;                                                     \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool name##_set(name *const array, int32_t index, type elt) {                \
    assert(array != NULL);                                                     \
    if (index < 0) {                                                           \
      return false;                                                            \
    }                                                                          \
    if ((size_t)index >= array->size) {                                        \
      name##_ensure_capacity(array, index + 1);                                \
      array->size = index + 1;                                                 \
    }                                                                          \
    array->table[index] = elt;                                                 \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool name##_set_ref(name *const array, int32_t index, type **ptr) {          \
    assert(array != NULL);                                                     \
    if (index < 0) {                                                           \
      return false;                                                            \
    }                                                                          \
    if ((size_t)index >= array->size) {                                        \
      name##_ensure_capacity(array, index + 1);                                \
      array->size = index + 1;                                                 \
    }                                                                          \
    *ptr = &array->table[index];                                               \
    return true;                                                               \
  }                                                                            \
                                                                               \
  type *name##_set_ref_unchecked(name *const array, int32_t index) {           \
    assert(array != NULL);                                                     \
    if ((size_t)index >= array->size) {                                        \
      name##_ensure_capacity(array, index + 1);                                \
      array->size = index + 1;                                                 \
    }                                                                          \
    return &array->table[index];                                               \
  }                                                                            \
                                                                               \
  bool name##_get(name *const array, int32_t index, type *ptr) {               \
    assert(array != NULL);                                                     \
    if (index < 0 || (size_t)index >= array->size) {                           \
      return false;                                                            \
    }                                                                          \
    *ptr = array->table[index];                                                \
    return true;                                                               \
  }                                                                            \
                                                                               \
  type name##_get_unchecked(name *const array, int32_t index) {                \
    assert(array != NULL);                                                     \
    return array->table[index];                                                \
  }                                                                            \
                                                                               \
  bool name##_get_ref(name *const array, int32_t index, const type **ptr) {    \
    if (index < 0 || (size_t)index >= array->size) {                           \
      return false;                                                            \
    }                                                                          \
    *ptr = &array->table[index];                                               \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool name##_mutable_ref(name *const array, int32_t index, type **ptr) {      \
    assert(array != NULL);                                                     \
    if (index < 0 || (size_t)index >= array->size) {                           \
      return false;                                                            \
    }                                                                          \
    *ptr = &array->table[index];                                               \
    return true;                                                               \
  }                                                                            \
                                                                               \
  const type *name##_get_ref_unchecked(name *const array, int32_t index) {     \
    return name##_mutable_ref_unchecked(array, index);                         \
  }                                                                            \
                                                                               \
  type *name##_mutable_ref_unchecked(name *const array, int32_t index) {       \
    assert(array != NULL);                                                     \
    return &array->table[index];                                               \
  }                                                                            \
                                                                               \
  bool name##_last(name *const array, type *ptr) {                             \
    assert(array != NULL);                                                     \
    return name##_get(array, name##_size(array) - 1, ptr);                     \
  }                                                                            \
                                                                               \
  type name##_last_unchecked(name *const array) {                              \
    assert(array != NULL);                                                     \
    return name##_get_unchecked(array, name##_size(array) - 1);                \
  }                                                                            \
                                                                               \
  bool name##_last_ref(name *const array, const type **ptr) {                  \
    assert(array != NULL);                                                     \
    return name##_get_ref(array, name##_size(array) - 1, ptr);                 \
  }                                                                            \
                                                                               \
  const type *name##_last_ref_unchecked(name *const array) {                   \
    assert(array != NULL);                                                     \
    return name##_get_ref_unchecked(array, name##_size(array) - 1);            \
  }                                                                            \
                                                                               \
  bool name##_remove(name *const array, int32_t index, type *ptr) {            \
    assert(array != NULL);                                                     \
    if (index < 0 || (size_t)index >= array->size) {                           \
      return false;                                                            \
    }                                                                          \
    *ptr = array->table[index];                                                \
    if (array->size > 1) {                                                     \
      name##_shift_left(array, index + 1, 1);                                  \
    }                                                                          \
    array->size--;                                                             \
    return true;                                                               \
  }                                                                            \
                                                                               \
  type name##_remove_unchecked(name *const array, int32_t index) {             \
    assert(array != NULL);                                                     \
    type to_return = array->table[index];                                      \
    if (array->size > 1) {                                                     \
      name##_shift_left(array, index + 1, 1);                                  \
    }                                                                          \
    array->size--;                                                             \
    return to_return;                                                          \
  }                                                                            \
                                                                               \
  size_t name##_size(const name *const array) {                                \
    assert(array != NULL);                                                     \
    return array->size;                                                        \
  }                                                                            \
                                                                               \
  bool name##_is_empty(const name *const array) {                              \
    assert(array != NULL);                                                     \
    return array->size == 0;                                                   \
  }                                                                            \
                                                                               \
  name *name##_copy(const name *const array) {                                 \
    assert(array != NULL);                                                     \
    name *copy = (name *)malloc(sizeof(name));                                 \
    assert(copy != NULL);                                                      \
    *copy = *array;                                                            \
    copy->table = (type *)malloc(sizeof(type) * array->capacity);              \
    assert(copy->table != NULL);                                               \
    memcpy(copy->table, array->table, sizeof(type) * array->size);             \
    return copy;                                                               \
  }                                                                            \
                                                                               \
  void name##_append(name *const head, const name *const tail) {               \
    assert(head != NULL && tail != NULL);                                      \
    name##_ensure_capacity(head, head->size + tail->size);                     \
    memcpy(head->table + head->size, tail->table, sizeof(type) * tail->size);  \
    head->size += tail->size;                                                  \
  }                                                                            \
                                                                               \
  bool name##_append_range(name *const head, const name *const tail,           \
                           int32_t tail_range_start, int32_t tail_range_end) { \
    assert(head != NULL && tail != NULL);                                      \
    if (tail_range_start < 0 || tail_range_start > tail_range_end ||           \
        (size_t)tail_range_end > tail->size) {                                 \
      return false;                                                            \
    }                                                                          \
    name##_ensure_capacity(head,                                               \
                           head->size + tail_range_end - tail_range_start);    \
    memcpy(head->table + head->size, tail->table + tail_range_start,           \
           sizeof(type) * (tail_range_end - tail_range_start));                \
    head->size += (tail_range_end - tail_range_start);                         \
    return true;                                                               \
  }                                                                            \
                                                                               \
  void name##_iterator(name##Iterator *iter, name *const array) {              \
    assert(iter != NULL && array != NULL);                                     \
    iter->index = 0;                                                           \
    iter->array = array;                                                       \
  }                                                                            \
                                                                               \
  bool name##_has_next(const name##Iterator *const iter) {                     \
    assert(iter != NULL);                                                      \
    return (size_t)iter->index < iter->array->size;                            \
  }                                                                            \
                                                                               \
  void name##_next(name##Iterator *iter) {                                     \
    assert(iter != NULL && (size_t)iter->index < iter->array->size);           \
    iter->index++;                                                             \
  }                                                                            \
                                                                               \
  const type *name##_value(const name##Iterator *const iter) {                 \
    assert(iter != NULL);                                                      \
    return name##_get_ref_unchecked(iter->array, iter->index);                 \
  }                                                                            \
                                                                               \
  type *name##_mutable_value(const name##Iterator *const iter) {               \
    assert(iter != NULL);                                                      \
    return name##_mutable_ref_unchecked(iter->array, iter->index);             \
  }

#ifdef __cplusplus
}
#endif

#endif /* C_DATA_STRUCTURES_ARRAYLIKE_H_ */
