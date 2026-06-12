#ifndef COM_GITHUB_JEFFMANZIONE_C_DATA_STRUCTURES_STABLE_MAPLIKE_H_
#define COM_GITHUB_JEFFMANZIONE_C_DATA_STRUCTURES_STABLE_MAPLIKE_H_

// stable_maplike.h
//
// Created on: Jun 03, 2020
//     Author: Jeff Manzione

#include "c-data-structures/maplike.h"
#include "c-data-structures/stable_arraylike.h"

#define DEFINE_STABLE_MAPLIKE(name, key_type, value_type)                   \
                                                                            \
  DEFINE_STABLE_ARRAYLIKE(name##ValueArray, value_type);                    \
                                                                            \
  DEFINE_MAPLIKE(name##KVMap, key_type, value_type *);                      \
                                                                            \
  typedef struct {                                                          \
    name##ValueArray arr;                                                   \
    name##KVMap map;                                                        \
  } name;                                                                   \
                                                                            \
  typedef struct {                                                          \
    name##KVMapIterator it;                                                 \
  } name##Iterator;                                                         \
                                                                            \
  bool name##_init(name *kl, name##KVMapHashFn, name##KVMapCompareFn);      \
  void name##_finalize(name *kl);                                           \
                                                                            \
  bool name##_insert(name *kl, const key_type key,                          \
                     uint32_t key_size value_type **entry);                 \
  value_type name##_find(name *kl, const key_type key,                      \
                         uint32_t key_size value_type default_value);       \
  value_type *name##_find_ref(name *kl, const key_type key,                 \
                              uint32_t key_size);                           \
  bool name##_contains(const name *const kl, const key_type key,            \
                       uint32_t key_size);                                  \
                                                                            \
  uint32_t name##_size(const name *);                                       \
                                                                            \
  void name##_iterator(name##Iterator *, name *const);                      \
  bool name##_has_entry(const name##Iterator *const);                       \
  void name##_next_entry(name##Iterator *);                                 \
  name##KVMapPair *name##_entry(const name##Iterator *const);               \
  const name##KVMapPair *name##_mutable_entry(const name##Iterator *const); \
  const key_type name##_key(const name##Iterator *const);                   \
  const value_type *name##_value(const name##Iterator *const);              \
  value_type *name##_mutable_value(const name##Iterator *const)

#define IMPL_STABLE_MAPLIKE(name, key_type, value_type)                       \
                                                                              \
  IMPL_STABLE_ARRAYLIKE(name##ValueArray, value_type);                        \
  IMPL_MAPLIKE(name##KVMap, key_type, value_type *);                          \
                                                                              \
  bool name##_init(name *kl, name##KVMapHashFn hash_fn,                       \
                   name##KVMapCompareFn compare_fn) {                         \
    const bool array_init_success = name##ValueArray_init(&kl->arr);          \
    if (!array_init_success) {                                                \
      return false;                                                           \
    }                                                                         \
    const bool map_init_success =                                             \
        name##KVMap_init(&kl->map, hash_fn, compare_fn);                      \
    if (!map_init_success) {                                                  \
      name##ValueArray_finalize(&kl->arr);                                    \
      return false;                                                           \
    }                                                                         \
    return true;                                                              \
  }                                                                           \
                                                                              \
  void name##_finalize(name *kl) {                                            \
    name##ValueArray_finalize(&kl->arr);                                      \
    name##KVMap_finalize(&kl->map);                                           \
  }                                                                           \
                                                                              \
  bool name##_insert(name *kl, const key_type key, uint32_t key_size,         \
                     value_type **entry) {                                    \
    value_type **existing = name##KVMap_find_ref(&kl->map, key, key_size);    \
    if (existing == NULL) {                                                   \
      *entry = name##ValueArray_push_back_ref(&kl->arr);                      \
      return name##KVMap_insert(&kl->map, key, key_size, *entry);             \
    }                                                                         \
    *entry = *existing;                                                       \
    return false;                                                             \
  }                                                                           \
                                                                              \
  value_type name##_find(name *kl, const key_type key, uint32_t key_size,     \
                         value_type default_value) {                          \
    return *name##KVMap_find(&kl->map, key, key_size, &default_value);        \
  }                                                                           \
                                                                              \
  value_type *name##_find_ref(name *kl, const key_type key,                   \
                              uint32_t key_size) {                            \
    return *name##KVMap_find_ref(&kl->map, key, key_size);                    \
  }                                                                           \
                                                                              \
  bool name##_contains(const name *const kl, const key_type key,              \
                       uint32_t key_size) {                                   \
    return name##KVMap_contains(&kl->map, key, key_size);                     \
  }                                                                           \
                                                                              \
  uint32_t name##_size(const name *kl) { return name##KVMap_size(&kl->map); } \
  void name##_iterator(name##Iterator *it, name *const kl) {                  \
    name##KVMap_iterator(&it->it, &kl->map);                                  \
  }                                                                           \
                                                                              \
  bool name##_has_entry(const name##Iterator *const it) {                     \
    return name##KVMap_has_entry(&it->it);                                    \
  }                                                                           \
                                                                              \
  void name##_next_entry(name##Iterator *it) {                                \
    name##KVMap_next_entry(&it->it);                                          \
  }                                                                           \
                                                                              \
  name##KVMapPair *name##_entry(const name##Iterator *const it) {             \
    return name##KVMap_entry(&it->it);                                        \
  }                                                                           \
                                                                              \
  const name##KVMapPair *name##_mutable_entry(                                \
      const name##Iterator *const it) {                                       \
    return name##KVMap_mutable_entry(&it->it);                                \
  }                                                                           \
                                                                              \
  const key_type name##_key(const name##Iterator *const it) {                 \
    return name##KVMap_key(&it->it);                                          \
  }                                                                           \
                                                                              \
  const value_type *name##_value(const name##Iterator *const it) {            \
    return *name##KVMap_value(&it->it);                                       \
  }                                                                           \
                                                                              \
  value_type *name##_mutable_value(const name##Iterator *const it) {          \
    return *name##KVMap_mutable_value(&it->it);                               \
  }

#endif /* COM_GITHUB_JEFFMANZIONE_C_DATA_STRUCTURES_STABLE_MAPLIKE_H_ */