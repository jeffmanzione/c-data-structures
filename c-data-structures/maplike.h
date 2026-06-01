#ifndef COM_GITHUB_JEFFMANZIONE_C_DATA_STRUCTURES_MAPLIKE_H_
#define COM_GITHUB_JEFFMANZIONE_C_DATA_STRUCTURES_MAPLIKE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define MIN_(a, b) ((a) > (b) ? b : a)

// A decent small prime number to use as the starting size for the hashtable
#define MAPLIKE_DEFAULT_TABLE_SIZE 31

// Find the table position of a hash value.
#define MAPLIKE_LOOKUP_HASH_POSITION(hval, num_probes, capacity) \
  (((hval) + ((num_probes) * (num_probes))) % (capacity))

// Calculates a new reasonable size of the hash table given a current size.
#define MAPLIKE_CALCULATE_NEW_TABLE_SIZE(current_size) (((current_size)*2) + 1)

// Calculates the threshold for number of entries in the given hash table size
// before it efficieny starts to diminish and the table should be
// resized/rehashed.
#define MAPLIKE_CALCULATE_RESIZE_THRESHOLD(capacity) ((int)((capacity) / 2.f))

// Value for num_probes on a hash table entry when it has been tombstoned, i.e.,
// previously contained a value that was subsquently removed.
#define MAPLIKE_TOMBSTONE -1

// True if entry is a tombstone, i.e., previously contained a value that was
// subsequently removed, false otherwise.
#define MAPLIKE_IS_TOMBSTONE(entry) ((entry)->num_probes == -1)

// True if entry currently has no value and did not previously, false
// otherwise.
#define MAPLIKE_IS_EMPTY(entry) ((entry)->num_probes == 0)

// The max number of probes search for a value in the hash table before there is
// a clear performance bottleneck w.r.t. the size of the hash table.
#define MAPLIKE_MAX_PROBES_THRESHOLD(capacity) ((int)((capacity) / 2))

// Expands to the header definitions for a hash set with the given name and
// value type.
//
// Generates the following for name=CatHashMap, key_type=CatKey, and
// value_type=Cat:
//
//   typedef struct {} CatHashMap;
//   typedef struct {} CatHashMapPair;
//   typedef struct {} CatHashMapIterator;
//   typedef uint32_t (*CatHashMapHashFn)(const CatKey, uint32_t size);
//   typedef int32_t (*CatHashMapCompareFn)(const CatKey, uint32_t, const
//                                          CatKey, uint32_t);
//   void CatHashMap_init(CatHashMap*, uint32_t capacity,
//                        CatHashMapHashFn, CatHashMapCompareFn);
//   CatHashMap* CatHashMap_init_capacity(CatHashMap*, uint32_t capacity,
//                                        CatHashMapHashFn,
//                                        CatHashMapCompareFn);
//   CatHashMap* CatHashMap_create(CatHashMapHashFn,
//                                 CatHashMapCompareFn);
//   CatHashMap* CatHashMap_create_capacity(uint32_t capacity, CatHashMapHashFn,
//                                          CatHashMapCompareFn);
//   void CatHashMap_finalize(CatHashMap*);
//   void CatHashMap_delete(CatHashMap*);
//   bool CatHashMap_insert(CatHashMap*, const CatKey, uint32_t, const Cat);
//   bool CatHashMap_remove(CatHashMap*, const CatKey, uint32_t, Cat*,
//                          uint32_t);
//   bool CatHashMap_contains(CatHashMap*, const CatKey, uint32_t);
//   Cat CatHashMap_find(const CatHashMap *hash_map, const CatKey key,
//                       uint32_t key_size, Cat default_value);
//   Cat *CatHashMap_find_ref(const CatHashMap *hash_map,
//                            const CatKey key, uint32_t key_size);
//   uint32_t CatHashMap_size(CatHashMap*);
#define DEFINE_MAPLIKE(name, key_type, value_type)                             \
                                                                               \
  typedef uint32_t (*name##HashFn)(const key_type, uint32_t size);             \
  typedef int32_t (*name##CompareFn)(const key_type, uint32_t, const key_type, \
                                     uint32_t);                                \
                                                                               \
  typedef struct name##Entry_ name##Entry;                                     \
                                                                               \
  typedef struct {                                                             \
    name##HashFn hash;                                                         \
    name##CompareFn compare;                                                   \
    uint32_t capacity, num_entries, resize_threshold;                          \
    name##Entry *table, *first, *last;                                         \
  } name;                                                                      \
                                                                               \
  typedef struct {                                                             \
    const key_type key;                                                        \
    uint32_t key_size;                                                         \
    value_type value;                                                          \
  } name##Pair;                                                                \
                                                                               \
  typedef struct {                                                             \
    name##Entry *cur;                                                          \
  } name##Iterator;                                                            \
                                                                               \
  bool name##_init(name *hash_map, name##HashFn, name##CompareFn);             \
  bool name##_init_capacity(name *hash_map, uint32_t capacity, name##HashFn,   \
                            name##CompareFn);                                  \
                                                                               \
  name *name##_create(name##HashFn, name##CompareFn);                          \
  name *name##_create_capacity(uint32_t size, name##HashFn, name##CompareFn);  \
                                                                               \
  void name##_finalize(name *);                                                \
                                                                               \
  void name##_delete(name *);                                                  \
                                                                               \
  bool name##_insert(name *, const key_type key, uint32_t key_size,            \
                     const value_type value);                                  \
                                                                               \
  bool name##_remove(name *, const key_type key, uint32_t key_size,            \
                     value_type *target_if_removed);                           \
                                                                               \
  bool name##_contains(const name *hash_map, const key_type key,               \
                       uint32_t key_size);                                     \
                                                                               \
  value_type name##_find(const name *hash_map, const key_type key,             \
                         uint32_t key_size, value_type default_value);         \
  value_type *name##_find_ref(const name *hash_map, const key_type key,        \
                              uint32_t key_size);                              \
                                                                               \
  uint32_t name##_size(const name *);                                          \
                                                                               \
  void name##_iterator(name##Iterator *, name *const);                         \
  bool name##_has_entry(const name##Iterator *const);                          \
  void name##_next_entry(name##Iterator *);                                    \
  name##Pair *name##_entry(const name##Iterator *const);                       \
  const name##Pair *name##_mutable_entry(const name##Iterator *const);         \
  const key_type name##_key(const name##Iterator *const);                      \
  const value_type *name##_value(const name##Iterator *const);                 \
  value_type *name##_mutable_value(const name##Iterator *const)

// Expands to the implementation for a hash set with the given name and value
// type.
#define IMPL_MAPLIKE(name, key_type, value_type)                               \
                                                                               \
  struct name##Entry_ {                                                        \
    name##Pair pair;                                                           \
    uint32_t hash_value;                                                       \
    int32_t num_probes;                                                        \
    name##Entry *prev, *next;                                                  \
  };                                                                           \
                                                                               \
  static bool name##_attempt_insert_internal(                                  \
      name *hash_map, const key_type key, uint32_t key_size,                   \
      const value_type value, uint32_t hval, name##Entry *table,               \
      uint32_t capacity, name##Entry **first, name##Entry **last,              \
      bool *probe_limit_exceeded) {                                            \
    int num_probes = 0;                                                        \
    int num_tombstones_encountered = 0;                                        \
    name##Entry *first_empty = NULL;                                           \
    int num_probes_at_first_empty = -1;                                        \
    while (true) {                                                             \
      const int hash_position =                                                \
          MAPLIKE_LOOKUP_HASH_POSITION(hval, num_probes, capacity);            \
      num_probes++;                                                            \
      name##Entry *entry = table + hash_position;                              \
      /* Position is vacant. */                                                \
      if (entry->num_probes == 0) {                                            \
        /* Use the previously empty slot if we don't find our element. */      \
        if (first_empty != NULL) {                                             \
          entry = first_empty;                                                 \
          num_probes = num_probes_at_first_empty;                              \
        }                                                                      \
        /* Take the vacant spot. */                                            \
        entry->pair.key = key;                                                 \
        entry->pair.key_size = key_size;                                       \
        entry->pair.value = (value_type)value;                                 \
        entry->hash_value = hval;                                              \
        entry->num_probes = num_probes;                                        \
        entry->prev = *last;                                                   \
        entry->next = NULL;                                                    \
        if (*last != NULL) {                                                   \
          entry->prev->next = entry;                                           \
        }                                                                      \
        *last = entry;                                                         \
        if (*first == NULL) {                                                  \
          *first = entry;                                                      \
        }                                                                      \
        return true;                                                           \
      }                                                                        \
      /* Spot is vacant but previously used, mark it so we can use it later.   \
       */                                                                      \
      if (MAPLIKE_IS_TOMBSTONE(entry)) {                                       \
        num_tombstones_encountered++;                                          \
        /* Returns early if there is a severe performance bottleneck so the    \
         * table can be rehashed. */                                           \
        if (num_tombstones_encountered >                                       \
            MAPLIKE_MAX_PROBES_THRESHOLD(capacity)) {                          \
          *probe_limit_exceeded = true;                                        \
          return false;                                                        \
        }                                                                      \
        if (first_empty == NULL) {                                             \
          first_empty = entry;                                                 \
          num_probes_at_first_empty = num_probes;                              \
        }                                                                      \
        continue;                                                              \
      }                                                                        \
      /* Pair is already present in the table, so the mission is accomplished. \
       */                                                                      \
      if (hval == entry->hash_value) {                                         \
        if (hash_map->compare(key, key_size, entry->pair.key,                  \
                              entry->pair.key_size) == 0) {                    \
          entry->pair.value = (value_type)value;                               \
          return false;                                                        \
        }                                                                      \
      }                                                                        \
      /* Rob this entry if it did fewer probes. */                             \
      if (entry->num_probes < num_probes) {                                    \
        name##Entry tmp_entry = *entry;                                        \
        /* Take its spot. */                                                   \
        entry->pair.key = key;                                                 \
        entry->pair.key_size = key_size;                                       \
        entry->pair.value = (value_type)value;                                 \
        entry->hash_value = hval;                                              \
        entry->num_probes = num_probes;                                        \
        /* It is the new insertion. */                                         \
        key = tmp_entry.pair.key;                                              \
        key_size = tmp_entry.pair.key_size;                                    \
        value = tmp_entry.pair.value;                                          \
        hval = tmp_entry.hash_value;                                           \
        num_probes = tmp_entry.num_probes;                                     \
        first_empty = NULL;                                                    \
        num_probes_at_first_empty = -1;                                        \
      }                                                                        \
    }                                                                          \
  }                                                                            \
                                                                               \
  static void name##_resize_table(name *hash_map) {                            \
    const uint32_t new_capacity =                                              \
        MAPLIKE_CALCULATE_NEW_TABLE_SIZE(hash_map->capacity);                  \
    name##Entry *new_table =                                                   \
        (name##Entry *)calloc(sizeof(name##Entry), new_capacity);              \
    name##Entry *new_first = NULL;                                             \
    name##Entry *new_last = NULL;                                              \
                                                                               \
    for (name##Entry *entry = hash_map->first; entry != NULL;                  \
         entry = entry->next) {                                                \
      bool probe_limit_exceeded = false;                                       \
      name##_attempt_insert_internal(                                          \
          hash_map, entry->pair.key, entry->pair.key_size, entry->pair.value,  \
          entry->hash_value, new_table, new_capacity, &new_first, &new_last,   \
          &probe_limit_exceeded);                                              \
      if (probe_limit_exceeded) {                                              \
        /* Should never happen */                                              \
      }                                                                        \
    }                                                                          \
                                                                               \
    free(hash_map->table);                                                     \
    hash_map->table = new_table;                                               \
    hash_map->capacity = new_capacity;                                         \
    hash_map->first = new_first;                                               \
    hash_map->last = new_last;                                                 \
    hash_map->resize_threshold =                                               \
        MAPLIKE_CALCULATE_RESIZE_THRESHOLD(new_capacity);                      \
  }                                                                            \
                                                                               \
  name *name##_create_capacity(uint32_t capacity, name##HashFn hash,           \
                               name##CompareFn compare) {                      \
    name *hash_map = (name *)calloc(sizeof(name), 1);                          \
    name##_init_capacity(hash_map, capacity, hash, compare);                   \
    return hash_map;                                                           \
  }                                                                            \
                                                                               \
  name *name##_create(name##HashFn hash, name##CompareFn compare) {            \
    return name##_create_capacity(MAPLIKE_DEFAULT_TABLE_SIZE, hash, compare);  \
  }                                                                            \
                                                                               \
  bool name##_init_capacity(name *hash_map, uint32_t capacity,                 \
                            name##HashFn hash, name##CompareFn compare) {      \
    if (capacity == 0) {                                                       \
      return false;                                                            \
    }                                                                          \
    hash_map->hash = hash;                                                     \
    hash_map->compare = compare;                                               \
    hash_map->capacity = capacity;                                             \
    hash_map->resize_threshold = MAPLIKE_CALCULATE_RESIZE_THRESHOLD(capacity); \
    hash_map->table = NULL;                                                    \
    hash_map->first = NULL;                                                    \
    hash_map->last = NULL;                                                     \
    hash_map->num_entries = 0;                                                 \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool name##_init(name *hash_map, name##HashFn hash,                          \
                   name##CompareFn compare) {                                  \
    return name##_init_capacity(hash_map, MAPLIKE_DEFAULT_TABLE_SIZE, hash,    \
                                compare);                                      \
  }                                                                            \
                                                                               \
  void name##_finalize(name *hash_map) {                                       \
    if (hash_map->table == NULL) {                                             \
      return;                                                                  \
    }                                                                          \
    free(hash_map->table);                                                     \
  }                                                                            \
                                                                               \
  void name##_delete(name *hash_map) {                                         \
    name##_finalize(hash_map);                                                 \
    free(hash_map);                                                            \
  }                                                                            \
                                                                               \
  bool name##_insert(name *hash_map, const key_type key, uint32_t key_size,    \
                     const value_type value) {                                 \
    if (hash_map->table == NULL) {                                             \
      hash_map->table =                                                        \
          (name##Entry *)calloc(sizeof(name##Entry), hash_map->capacity);      \
    } else if (hash_map->num_entries > hash_map->resize_threshold) {           \
      name##_resize_table(hash_map);                                           \
    }                                                                          \
    bool probe_limit_exceeded = false;                                         \
    bool was_inserted = name##_attempt_insert_internal(                        \
        hash_map, key, key_size, value, hash_map->hash(key, key_size),         \
        hash_map->table, hash_map->capacity, &hash_map->first,                 \
        &hash_map->last, &probe_limit_exceeded);                               \
    /* Maps may have a lot of removed spots. If this causes a performance      \
     * slowdown, then it is better to rehash the map. */                       \
    if (probe_limit_exceeded) {                                                \
      name##_resize_table(hash_map);                                           \
      probe_limit_exceeded = false;                                            \
      was_inserted = name##_attempt_insert_internal(                           \
          hash_map, key, key_size, value, hash_map->hash(key, key_size),       \
          hash_map->table, hash_map->capacity, &hash_map->first,               \
          &hash_map->last, &probe_limit_exceeded);                             \
      if (probe_limit_exceeded) {                                              \
        /* This should never happen. */                                        \
      }                                                                        \
    }                                                                          \
    if (was_inserted) {                                                        \
      hash_map->num_entries++;                                                 \
    }                                                                          \
    return was_inserted;                                                       \
  }                                                                            \
                                                                               \
  static name##Entry *name##_find_entry(                                       \
      const name *hash_map, const key_type key, uint32_t key_size,             \
      name##Entry *table, uint32_t capacity) {                                 \
    const uint32_t hval = hash_map->hash(key, key_size);                       \
    int num_probes = 0;                                                        \
    while (true) {                                                             \
      int table_index =                                                        \
          MAPLIKE_LOOKUP_HASH_POSITION(hval, num_probes, capacity);            \
      ++num_probes;                                                            \
      name##Entry *entry = table + table_index;                                \
      if (MAPLIKE_IS_EMPTY(entry)) {                                           \
        return NULL;                                                           \
      }                                                                        \
      if (MAPLIKE_IS_TOMBSTONE(entry)) {                                       \
        continue;                                                              \
      }                                                                        \
      if (hval == entry->hash_value) {                                         \
        if (hash_map->compare(key, key_size, entry->pair.key,                  \
                              entry->pair.key_size) == 0) {                    \
          return entry;                                                        \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }                                                                            \
                                                                               \
  bool name##_remove(name *hash_map, const key_type key, uint32_t key_size,    \
                     value_type *target_if_removed) {                          \
    if (hash_map->table == NULL) {                                             \
      return false;                                                            \
    }                                                                          \
    name##Entry *entry = name##_find_entry(                                    \
        hash_map, key, key_size, hash_map->table, hash_map->capacity);         \
    if (entry == NULL) {                                                       \
      return false;                                                            \
    }                                                                          \
    if (target_if_removed != NULL) {                                           \
      *target_if_removed = entry->pair.value;                                  \
    }                                                                          \
    if (hash_map->last == entry) {                                             \
      hash_map->last = entry->prev;                                            \
    } else {                                                                   \
      entry->next->prev = entry->prev;                                         \
    }                                                                          \
    if (hash_map->first == entry) {                                            \
      hash_map->first = entry->next;                                           \
    } else {                                                                   \
      entry->prev->next = entry->next;                                         \
    }                                                                          \
    entry->num_probes = MAPLIKE_TOMBSTONE;                                     \
    hash_map->num_entries--;                                                   \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool name##_contains(const name *hash_map, const key_type key,               \
                       uint32_t key_size) {                                    \
    if (hash_map->table == NULL) {                                             \
      return false;                                                            \
    }                                                                          \
    name##Entry *entry = name##_find_entry(                                    \
        hash_map, key, key_size, hash_map->table, hash_map->capacity);         \
    if (entry == NULL) {                                                       \
      return false;                                                            \
    }                                                                          \
    return true;                                                               \
  }                                                                            \
                                                                               \
  value_type name##_find(const name *hash_map, const key_type key,             \
                         uint32_t key_size, value_type default_value) {        \
    value_type *found = name##_find_ref(hash_map, key, key_size);              \
    if (found == NULL) {                                                       \
      return default_value;                                                    \
    }                                                                          \
    return *found;                                                             \
  }                                                                            \
                                                                               \
  value_type *name##_find_ref(const name *hash_map, const key_type key,        \
                              uint32_t key_size) {                             \
    if (hash_map->table == NULL) {                                             \
      return NULL;                                                             \
    }                                                                          \
    name##Entry *entry = name##_find_entry(                                    \
        hash_map, key, key_size, hash_map->table, hash_map->capacity);         \
    if (entry == NULL) {                                                       \
      return NULL;                                                             \
    }                                                                          \
    return &entry->pair.value;                                                 \
  }                                                                            \
                                                                               \
  uint32_t name##_size(const name *hash_map) { return hash_map->num_entries; } \
                                                                               \
  void name##_iterator(name##Iterator *it, name *const map) {                  \
    it->cur = map->first;                                                      \
  }                                                                            \
                                                                               \
  bool name##_has_entry(const name##Iterator *const it) {                      \
    return it->cur != NULL;                                                    \
  }                                                                            \
                                                                               \
  void name##_next_entry(name##Iterator *it) { it->cur = it->cur->next; }      \
                                                                               \
  name##Pair *name##_entry(const name##Iterator *const it) {                   \
    return &it->cur->pair;                                                     \
  }                                                                            \
                                                                               \
  const name##Pair *name##_mutable_entry(const name##Iterator *const it) {     \
    return &it->cur->pair;                                                     \
  }                                                                            \
                                                                               \
  const key_type name##_key(const name##Iterator *const it) {                  \
    return it->cur->pair.key;                                                  \
  }                                                                            \
                                                                               \
  const value_type *name##_value(const name##Iterator *const it) {             \
    return (const value_type *)&it->cur->pair.value;                           \
  }                                                                            \
                                                                               \
  value_type *name##_mutable_value(const name##Iterator *const it) {           \
    return &it->cur->pair.value;                                               \
  }

#ifdef __cplusplus
}
#endif

#endif /* COM_GITHUB_JEFFMANZIONE_C_DATA_STRUCTURES_MAPLIKE_H_ */