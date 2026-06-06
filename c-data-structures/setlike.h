#ifndef COM_GITHUB_JEFFMANZIONE_C_DATA_STRUCTURES_SETLIKE_H_
#define COM_GITHUB_JEFFMANZIONE_C_DATA_STRUCTURES_SETLIKE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// A decent small prime number to use as the starting size for the hashtable
#define SETLIKE_DEFAULT_capacity 31

// Find the table position of a hash value.
#define SETLIKE_LOOKUP_HASH_POSITION(hval, num_probes, capacity) \
  (((hval) + ((num_probes) * (num_probes))) % (capacity))

// Calculates a new reasonable size of the hash table given a current size.
#define SETLIKE_CALCULATE_NEW_capacity(current_size) (((current_size)*2) + 1)

// Calculates the threshold for number of entries in the given hash table size
// before it efficieny starts to diminish and the table should be
// resized/rehashed.
#define SETLIKE_CALCULATE_RESIZE_THRESHOLD(capacity) ((int)((capacity) / 2.f))

// Value for num_probes on a hash table entry when it has been tombstoned, i.e.,
// previously contained a value that was subsquently removed.
#define SETLIKE_TOMBSTONE -1

// True if entry is a tombstone, i.e., previously contained a value that was
// subsequently removed, false otherwise.
#define SETLIKE_IS_TOMBSTONE(entry) ((entry)->num_probes == -1)

// True if entry currently has no value and did not previously, false
// otherwise.
#define SETLIKE_IS_EMPTY(entry) ((entry)->num_probes == 0)

// The max number of probes search for a value in the hash table before there is
// a clear performance bottleneck w.r.t. the size of the hash table.
#define SETLIKE_MAX_PROBES_THRESHOLD(capacity) ((int)((capacity) / 2))

// Expands to the header definitions for a hash set with the given name and
// value type.
#define DEFINE_SETLIKE(name, value_type)                                      \
                                                                              \
  typedef uint32_t (*name##HashFn)(const value_type, uint32_t size);          \
  typedef int32_t (*name##CompareFn)(const value_type, uint32_t,              \
                                     const value_type, uint32_t);             \
                                                                              \
  typedef struct name##Entry_ name##Entry;                                    \
                                                                              \
  typedef struct {                                                            \
    name##HashFn hash;                                                        \
    name##CompareFn compare;                                                  \
    uint32_t capacity, num_entries, resize_threshold;                         \
    name##Entry *table, *first, *last;                                        \
  } name;                                                                     \
                                                                              \
  typedef struct {                                                            \
    name##Entry *cur;                                                         \
  } name##Iterator;                                                           \
                                                                              \
  bool name##_init(name *hash_set, name##HashFn, name##CompareFn);            \
  bool name##_init_capacity(name *hash_set, uint32_t capacity, name##HashFn,  \
                            name##CompareFn);                                 \
                                                                              \
  name *name##_create(name##HashFn, name##CompareFn);                         \
  name *name##_create_capacity(uint32_t size, name##HashFn, name##CompareFn); \
                                                                              \
  void name##_finalize(name *);                                               \
                                                                              \
  void name##_delete(name *);                                                 \
                                                                              \
  bool name##_insert(name *, const value_type value, uint32_t value_size);    \
                                                                              \
  bool name##_remove(name *, const value_type value, uint32_t value_size);    \
                                                                              \
  bool name##_contains(const name *hash_set, const value_type value,          \
                       uint32_t value_size);                                  \
                                                                              \
  value_type name##_find(const name *hash_set, const value_type value,        \
                         uint32_t value_size, value_type default_value);      \
  value_type *name##_find_ref(const name *hash_set, const value_type value,   \
                              uint32_t value_size);                           \
                                                                              \
  uint32_t name##_size(const name *);                                         \
                                                                              \
  void name##_iterator(name##Iterator *, name *const);                        \
  bool name##_has_next(const name##Iterator *const);                          \
  void name##_next(name##Iterator *);                                         \
  const value_type *name##_value(const name##Iterator *const);                \
  value_type *name##_mutable_value(const name##Iterator *const)

// Expands to the implementation for a hash set with the given name and value
// type.
#define IMPL_SETLIKE(name, value_type)                                         \
                                                                               \
  struct name##Entry_ {                                                        \
    value_type value;                                                          \
    uint32_t value_size;                                                       \
    uint32_t hash_value;                                                       \
    int32_t num_probes;                                                        \
    name##Entry *prev, *next;                                                  \
  };                                                                           \
                                                                               \
  static bool name##_attempt_insert_internal(                                  \
      name *hash_set, value_type value, uint32_t value_size, uint32_t hval,    \
      name##Entry *table, uint32_t capacity, name##Entry **first,              \
      name##Entry **last, bool *probe_limit_exceeded) {                        \
    int num_probes = 0;                                                        \
    int num_tombstones_encountered = 0;                                        \
    name##Entry *first_empty = NULL;                                           \
    int num_probes_at_first_empty = -1;                                        \
    while (true) {                                                             \
      const int hash_position =                                                \
          SETLIKE_LOOKUP_HASH_POSITION(hval, num_probes, capacity);            \
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
        entry->value = (value_type)value;                                      \
        entry->value_size = value_size;                                        \
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
      if (SETLIKE_IS_TOMBSTONE(entry)) {                                       \
        num_tombstones_encountered++;                                          \
        /* Returns early if there is a severe performance bottleneck so the    \
         * table can be rehashed. */                                           \
        if (num_tombstones_encountered >                                       \
            SETLIKE_MAX_PROBES_THRESHOLD(capacity)) {                          \
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
        if (hash_set->compare(value, value_size, entry->value,                 \
                              entry->value_size) == 0) {                       \
          entry->value = (value_type)value;                                    \
          return false;                                                        \
        }                                                                      \
      }                                                                        \
      /* Rob this entry if it did fewer probes. */                             \
      if (entry->num_probes < num_probes) {                                    \
        name##Entry tmp_entry = *entry;                                        \
        /* Take its spot. */                                                   \
        entry->value = (value_type)value;                                      \
        entry->value_size = value_size;                                        \
        entry->hash_value = hval;                                              \
        entry->num_probes = num_probes;                                        \
        /* It is the new insertion. */                                         \
        value = tmp_entry.value;                                               \
        value_size = tmp_entry.value_size;                                     \
        hval = tmp_entry.hash_value;                                           \
        num_probes = tmp_entry.num_probes;                                     \
        first_empty = NULL;                                                    \
        num_probes_at_first_empty = -1;                                        \
      }                                                                        \
    }                                                                          \
  }                                                                            \
                                                                               \
  static void name##_resize_table(name *hash_set) {                            \
    const uint32_t new_capacity =                                              \
        SETLIKE_CALCULATE_NEW_capacity(hash_set->capacity);                    \
    name##Entry *new_table =                                                   \
        (name##Entry *)calloc(sizeof(name##Entry), new_capacity);              \
    name##Entry *new_first = NULL;                                             \
    name##Entry *new_last = NULL;                                              \
                                                                               \
    for (name##Entry *entry = hash_set->first; entry != NULL;                  \
         entry = entry->next) {                                                \
      bool probe_limit_exceeded = false;                                       \
      name##_attempt_insert_internal(hash_set, entry->value,                   \
                                     entry->value_size, entry->hash_value,     \
                                     new_table, new_capacity, &new_first,      \
                                     &new_last, &probe_limit_exceeded);        \
      if (probe_limit_exceeded) {                                              \
        /* Should never happen */                                              \
      }                                                                        \
    }                                                                          \
                                                                               \
    free(hash_set->table);                                                     \
    hash_set->table = new_table;                                               \
    hash_set->capacity = new_capacity;                                         \
    hash_set->first = new_first;                                               \
    hash_set->last = new_last;                                                 \
    hash_set->resize_threshold =                                               \
        SETLIKE_CALCULATE_RESIZE_THRESHOLD(new_capacity);                      \
  }                                                                            \
                                                                               \
  name *name##_create(name##HashFn hash, name##CompareFn compare) {            \
    return name##_create_capacity(SETLIKE_DEFAULT_capacity, hash, compare);    \
  }                                                                            \
                                                                               \
  name *name##_create_capacity(uint32_t start_size, name##HashFn hash,         \
                               name##CompareFn compare) {                      \
    name *hash_set = (name *)calloc(sizeof(name), 1);                          \
    name##_init_capacity(hash_set, start_size, hash, compare);                 \
    return hash_set;                                                           \
  }                                                                            \
                                                                               \
  bool name##_init(name *hash_set, name##HashFn hash,                          \
                   name##CompareFn compare) {                                  \
    return name##_init_capacity(hash_set, SETLIKE_DEFAULT_capacity, hash,      \
                                compare);                                      \
  }                                                                            \
                                                                               \
  bool name##_init_capacity(name *hash_set, uint32_t capacity,                 \
                            name##HashFn hash, name##CompareFn compare) {      \
    if (capacity == 0) {                                                       \
      return false;                                                            \
    }                                                                          \
    hash_set->hash = hash;                                                     \
    hash_set->compare = compare;                                               \
    hash_set->capacity = capacity;                                             \
    hash_set->resize_threshold = SETLIKE_CALCULATE_RESIZE_THRESHOLD(capacity); \
    hash_set->table = NULL;                                                    \
    hash_set->first = NULL;                                                    \
    hash_set->last = NULL;                                                     \
    hash_set->num_entries = 0;                                                 \
    return true;                                                               \
  }                                                                            \
                                                                               \
  void name##_finalize(name *hash_set) {                                       \
    if (hash_set->table == NULL) {                                             \
      return;                                                                  \
    }                                                                          \
    free(hash_set->table);                                                     \
  }                                                                            \
                                                                               \
  void name##_delete(name *hash_set) {                                         \
    name##_finalize(hash_set);                                                 \
    free(hash_set);                                                            \
  }                                                                            \
                                                                               \
  bool name##_insert(name *hash_set, const value_type value,                   \
                     uint32_t value_size) {                                    \
    if (hash_set->table == NULL) {                                             \
      hash_set->table =                                                        \
          (name##Entry *)calloc(sizeof(name##Entry), hash_set->capacity);      \
    } else if (hash_set->num_entries > hash_set->resize_threshold) {           \
      name##_resize_table(hash_set);                                           \
    }                                                                          \
    bool probe_limit_exceeded = false;                                         \
    bool was_inserted = name##_attempt_insert_internal(                        \
        hash_set, (value_type)value, value_size,                               \
        hash_set->hash(value, value_size), hash_set->table,                    \
        hash_set->capacity, &hash_set->first, &hash_set->last,                 \
        &probe_limit_exceeded);                                                \
    /* hash_sets may have a lot of removed spots. If this causes a performance \
     * slowdown, then it is better to rehash the hash_set. */                  \
    if (probe_limit_exceeded) {                                                \
      name##_resize_table(hash_set);                                           \
      probe_limit_exceeded = false;                                            \
      was_inserted = name##_attempt_insert_internal(                           \
          hash_set, (value_type)value, value_size,                             \
          hash_set->hash(value, value_size), hash_set->table,                  \
          hash_set->capacity, &hash_set->first, &hash_set->last,               \
          &probe_limit_exceeded);                                              \
      if (probe_limit_exceeded) {                                              \
        /* This should never happen. */                                        \
      }                                                                        \
    }                                                                          \
    if (was_inserted) {                                                        \
      hash_set->num_entries++;                                                 \
    }                                                                          \
    return was_inserted;                                                       \
  }                                                                            \
                                                                               \
  static name##Entry *name##_find_entry(                                       \
      const name *hash_set, const value_type value, uint32_t value_size,       \
      name##Entry *table, uint32_t capacity) {                                 \
    const uint32_t hval = hash_set->hash(value, value_size);                   \
    int num_probes = 0;                                                        \
    while (true) {                                                             \
      int table_index =                                                        \
          SETLIKE_LOOKUP_HASH_POSITION(hval, num_probes, capacity);            \
      ++num_probes;                                                            \
      name##Entry *entry = table + table_index;                                \
      if (SETLIKE_IS_EMPTY(entry)) {                                           \
        return NULL;                                                           \
      }                                                                        \
      if (SETLIKE_IS_TOMBSTONE(entry)) {                                       \
        continue;                                                              \
      }                                                                        \
      if (hval == entry->hash_value) {                                         \
        if (hash_set->compare(value, value_size, entry->value,                 \
                              entry->value_size) == 0) {                       \
          return entry;                                                        \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }                                                                            \
                                                                               \
  bool name##_remove(name *hash_set, const value_type value,                   \
                     uint32_t value_size) {                                    \
    if (hash_set->table == NULL) {                                             \
      return false;                                                            \
    }                                                                          \
    name##Entry *entry = name##_find_entry(                                    \
        hash_set, value, value_size, hash_set->table, hash_set->capacity);     \
    if (entry == NULL) {                                                       \
      return false;                                                            \
    }                                                                          \
    if (hash_set->last == entry) {                                             \
      hash_set->last = entry->prev;                                            \
    } else {                                                                   \
      entry->next->prev = entry->prev;                                         \
    }                                                                          \
    if (hash_set->first == entry) {                                            \
      hash_set->first = entry->next;                                           \
    } else {                                                                   \
      entry->prev->next = entry->next;                                         \
    }                                                                          \
    entry->num_probes = SETLIKE_TOMBSTONE;                                     \
    hash_set->num_entries--;                                                   \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool name##_contains(const name *hash_set, const value_type value,           \
                       uint32_t value_size) {                                  \
    if (hash_set->table == NULL) {                                             \
      return false;                                                            \
    }                                                                          \
    name##Entry *entry = name##_find_entry(                                    \
        hash_set, value, value_size, hash_set->table, hash_set->capacity);     \
    if (entry == NULL) {                                                       \
      return false;                                                            \
    }                                                                          \
    return true;                                                               \
  }                                                                            \
  value_type name##_find(const name *hash_set, const value_type value,         \
                         uint32_t value_size, value_type default_value) {      \
    value_type *found = name##_find_ref(hash_set, value, value_size);          \
    if (found == NULL) {                                                       \
      return default_value;                                                    \
    }                                                                          \
    return *found;                                                             \
  }                                                                            \
                                                                               \
  value_type *name##_find_ref(const name *hash_set, const value_type value,    \
                              uint32_t value_size) {                           \
    if (hash_set->table == NULL) {                                             \
      return NULL;                                                             \
    }                                                                          \
    name##Entry *entry = name##_find_entry(                                    \
        hash_set, value, value_size, hash_set->table, hash_set->capacity);     \
    if (entry == NULL) {                                                       \
      return NULL;                                                             \
    }                                                                          \
    return &entry->value;                                                      \
  }                                                                            \
                                                                               \
  uint32_t name##_size(const name *hash_set) { return hash_set->num_entries; } \
                                                                               \
  void name##_iterator(name##Iterator *it, name *const hash_set) {             \
    it->cur = hash_set->first;                                                 \
  }                                                                            \
                                                                               \
  bool name##_has_next(name##Iterator *const it) { return it->cur != NULL; }   \
                                                                               \
  void name##_next(name##Iterator *it) { it->cur = it->cur->next; }            \
                                                                               \
  const value_type *name##_value(const name##Iterator *const it) {             \
    return (const value_type *)&it->cur->value;                                \
  }                                                                            \
                                                                               \
  value_type *name##_mutable_value(const name##Iterator *const it) {           \
    return &it->cur->value;                                                    \
  }                                                                            \
                                                                               \
  uint32_t name##_value_size(const name##Iterator *const it) {                 \
    return it->cur->value_size;                                                \
  }

#ifdef __cplusplus
}
#endif

#endif /* COM_GITHUB_JEFFMANZIONE_C_DATA_STRUCTURES_SETLIKE_H_ */