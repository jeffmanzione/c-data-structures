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

#define DEFINE_STABLE_ARRAYLIKE(name, type)                          \
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

#ifdef __cplusplus
}
#endif

#endif /* C_DATA_STRUCTURES_STABLE_ARRAYLIKE_H_ */
