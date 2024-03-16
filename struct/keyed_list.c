// keyed_list.h
//
// Created on: Jun 03, 2020
//     Author: Jeff Manzione

#include "struct/keyed_list.h"

#include "alloc/alloc.h"
#include "debug/debug.h"
#include "struct/struct_defaults.h"

void __keyedlist_init(KeyedList *klist, const char type_name[], size_t type_sz,
                      size_t table_sz) {
  ASSERT(NOT_NULL(klist));
  slist_init(&klist->_list, type_sz);
  map_init_default(&klist->_map);
}

void keyedlist_finalize(KeyedList *klist) {
  ASSERT(NOT_NULL(klist));
  map_finalize(&klist->_map);
  slist_finalize(&klist->_list);
}

void *keyedlist_insert(KeyedList *klist, const void *key, void **entry) {
  ASSERT(NOT_NULL(klist), NOT_NULL(key));
  void *existing = map_lookup(&klist->_map, key);
  if (NULL == existing) {
    *entry = slist_add_last(&klist->_list);
    map_insert(&klist->_map, key, *entry);
    return NULL;
  }
  *entry = existing;
  return existing;
}

void *keyedlist_lookup(KeyedList *klist, const void *key) {
  ASSERT(NOT_NULL(klist), NOT_NULL(key));
  return map_lookup(&klist->_map, key);
}

KL_iter keyedlist_iter(KeyedList *klist) {
  ASSERT(NOT_NULL(klist));
  KL_iter iter = {._iter = map_iter(&klist->_map)};
  return iter;
}

bool kl_has(KL_iter *iter) { return has(&iter->_iter); }

void kl_inc(KL_iter *iter) { inc(&iter->_iter); }

const void *kl_key(KL_iter *iter) { return key(&iter->_iter); }

const void *kl_value(KL_iter *iter) { return value(&iter->_iter); }