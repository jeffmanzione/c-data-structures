#include "c-data-structures/stable_maplike.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {
using testing::IsNull;
using testing::NotNull;

DEFINE_STABLE_MAPLIKE(IntToIntMap, int, int);
IMPL_STABLE_MAPLIKE(IntToIntMap, int, int);

uint32_t hash_int(const int i, uint32_t size) { return i; }
int32_t compare_ints(const int i1, uint32_t size1, const int i2,
                     uint32_t size2) {
  return i1 - i2;
}

/* Instantiate a map type for testing */
DEFINE_STABLE_MAPLIKE(StringToIntMap, char *, int);
IMPL_STABLE_MAPLIKE(StringToIntMap, char *, int);

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
#define FNV_32_PRIME (0x01000193)
#define FNV_1A_32_OFFSET (0x811C9DC5)

uint32_t hash_string(const char *ptr, uint32_t size) {
  unsigned char *s = (unsigned char *)ptr;
  uint32_t hval = FNV_1A_32_OFFSET;
  for (uint32_t i = 0; i < size; ++i) {
    hval *= FNV_32_PRIME;
    hval ^= (uint32_t)*s++;
  }
  return hval;
}

int32_t compare_strings(const char *ptr1, uint32_t size1, const char *ptr2,
                        uint32_t size2) {
  if (ptr1 == ptr2) {
    return 0;
  }
  if (NULL == ptr1) {
    return -1;
  }
  if (NULL == ptr2) {
    return 1;
  }
  return memcmp(ptr1, ptr2, std::max(size1, size2));
}

/* Test fixture to ensure proper setup / teardown */
class StringToIntMapTest : public ::testing::Test {
 protected:
  StringToIntMap map{};

  void SetUp() override {
    ASSERT_TRUE(StringToIntMap_init(&map, hash_string, compare_strings));
  }

  void TearDown() override { StringToIntMap_finalize(&map); }
};

/* -------------------------------------------------------------
 * Initialization and basic state
 * ------------------------------------------------------------- */

TEST(StringToIntMapStandaloneTest, Init) {
  StringToIntMap map{};
  EXPECT_TRUE(StringToIntMap_init(&map, hash_string, compare_strings));
  EXPECT_EQ(StringToIntMap_size(&map), 0);
  StringToIntMap_finalize(&map);
}

// /* -------------------------------------------------------------
//  * Insert / Remove
//  * ------------------------------------------------------------- */

TEST_F(StringToIntMapTest, InsertAndRemove) {
  int *a, *b, *c;
  EXPECT_TRUE(StringToIntMap_insert(&map, "a", strlen("a"), &a));
  *a = 10;
  EXPECT_TRUE(StringToIntMap_insert(&map, "b", strlen("b"), &b));
  *b = 20;
  EXPECT_TRUE(StringToIntMap_insert(&map, "c", strlen("c"), &c));
  *c = 30;
  EXPECT_EQ(StringToIntMap_size(&map), 3);
}

// /* -------------------------------------------------------------
//  * Find
//  * ------------------------------------------------------------- */

TEST_F(StringToIntMapTest, FindExistingValue) {
  int *a, *b, *c;
  EXPECT_TRUE(StringToIntMap_insert(&map, "a", strlen("a"), &a));
  *a = 10;
  EXPECT_TRUE(StringToIntMap_insert(&map, "b", strlen("b"), &b));
  *b = 20;
  EXPECT_TRUE(StringToIntMap_insert(&map, "c", strlen("c"), &c));
  *c = 30;
  EXPECT_EQ(StringToIntMap_size(&map), 3);

  EXPECT_EQ(StringToIntMap_find(&map, "a", strlen("a"), -1), 10);
  EXPECT_EQ(StringToIntMap_find(&map, "b", strlen("b"), -1), 20);
  EXPECT_EQ(StringToIntMap_find(&map, "c", strlen("c"), -1), 30);
}

TEST_F(StringToIntMapTest, DoesNotFindValue) {
  int *a, *b, *c;
  EXPECT_TRUE(StringToIntMap_insert(&map, "a", strlen("a"), &a));
  *a = 10;
  EXPECT_TRUE(StringToIntMap_insert(&map, "b", strlen("b"), &b));
  *b = 20;
  EXPECT_TRUE(StringToIntMap_insert(&map, "c", strlen("c"), &c));
  *c = 30;
  EXPECT_EQ(StringToIntMap_size(&map), 3);

  EXPECT_EQ(StringToIntMap_find(&map, "d", strlen("d"), -1), -1);

  int *d;
  StringToIntMap_insert(&map, "d", strlen("d"), &d);
  *d = 40;

  EXPECT_EQ(StringToIntMap_size(&map), 4);

  EXPECT_EQ(StringToIntMap_find(&map, "d", strlen("d"), -1), 40);
}

TEST_F(StringToIntMapTest, DoesNotFindAfterRemoval) {
  int *a, *b, *c;
  EXPECT_TRUE(StringToIntMap_insert(&map, "a", strlen("a"), &a));
  *a = 10;
  EXPECT_TRUE(StringToIntMap_insert(&map, "b", strlen("b"), &b));
  *b = 20;
  EXPECT_TRUE(StringToIntMap_insert(&map, "c", strlen("c"), &c));
  *c = 30;
  EXPECT_EQ(StringToIntMap_size(&map), 3);

  EXPECT_EQ(StringToIntMap_find(&map, "a", strlen("a"), -1), 10);
  EXPECT_EQ(StringToIntMap_find(&map, "b", strlen("b"), -1), 20);
  EXPECT_EQ(StringToIntMap_find(&map, "c", strlen("c"), -1), 30);
}

// /* -------------------------------------------------------------
//  * Contains
//  * ------------------------------------------------------------- */

TEST_F(StringToIntMapTest, Contains) {
  int *a, *b, *c;
  EXPECT_TRUE(StringToIntMap_insert(&map, "a", strlen("a"), &a));
  *a = 10;
  EXPECT_TRUE(StringToIntMap_insert(&map, "b", strlen("b"), &b));
  *b = 20;
  EXPECT_TRUE(StringToIntMap_insert(&map, "c", strlen("c"), &c));
  *c = 30;

  EXPECT_TRUE(StringToIntMap_contains(&map, "a", strlen("a")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "b", strlen("b")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "c", strlen("c")));

  EXPECT_FALSE(StringToIntMap_contains(&map, "d", strlen("d")));
}

TEST_F(StringToIntMapTest, Contains2) {
  EXPECT_FALSE(StringToIntMap_contains(&map, "a", strlen("a")));
  EXPECT_FALSE(StringToIntMap_contains(&map, "b", strlen("b")));
  EXPECT_FALSE(StringToIntMap_contains(&map, "c", strlen("c")));

  int *a, *b, *c;
  EXPECT_TRUE(StringToIntMap_insert(&map, "a", strlen("a"), &a));
  *a = 10;
  EXPECT_TRUE(StringToIntMap_insert(&map, "b", strlen("b"), &b));
  *b = 20;
  EXPECT_TRUE(StringToIntMap_insert(&map, "c", strlen("c"), &c));
  *c = 30;

  EXPECT_TRUE(StringToIntMap_contains(&map, "a", strlen("a")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "b", strlen("b")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "c", strlen("c")));

  EXPECT_FALSE(StringToIntMap_contains(&map, "d", strlen("d")));
  int *d;
  StringToIntMap_insert(&map, "d", strlen("d"), &d);
  *d = 40;
  EXPECT_TRUE(StringToIntMap_contains(&map, "d", strlen("d")));
}

TEST_F(StringToIntMapTest, DoesNotContainAfterRemoval) {
  int *a, *b, *c;
  EXPECT_TRUE(StringToIntMap_insert(&map, "a", strlen("a"), &a));
  *a = 10;
  EXPECT_TRUE(StringToIntMap_insert(&map, "b", strlen("b"), &b));
  *b = 20;
  EXPECT_TRUE(StringToIntMap_insert(&map, "c", strlen("c"), &c));
  *c = 30;

  EXPECT_TRUE(StringToIntMap_contains(&map, "a", strlen("a")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "b", strlen("b")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "c", strlen("c")));
}

// /* -------------------------------------------------------------
//  * Iterator
//  * ------------------------------------------------------------- */

TEST_F(StringToIntMapTest, Iterator) {
  int *a, *b, *c;
  EXPECT_TRUE(StringToIntMap_insert(&map, "a", strlen("a"), &a));
  *a = 10;
  EXPECT_TRUE(StringToIntMap_insert(&map, "b", strlen("b"), &b));
  *b = 20;
  EXPECT_TRUE(StringToIntMap_insert(&map, "c", strlen("c"), &c));
  *c = 30;

  StringToIntMapIterator it;
  StringToIntMap_iterator(&it, &map);
  EXPECT_TRUE(StringToIntMap_has_entry(&it));
  const StringToIntMapPair *kv = StringToIntMap_entry(&it);
  EXPECT_EQ(kv->key, "a");
  EXPECT_EQ(kv->key_size, strlen("a"));
  EXPECT_EQ(*kv->value, 10);
  StringToIntMap_next_entry(&it);

  EXPECT_TRUE(StringToIntMap_has_entry(&it));
  kv = StringToIntMap_entry(&it);
  EXPECT_EQ(kv->key, "b");
  EXPECT_EQ(kv->key_size, strlen("b"));
  EXPECT_EQ(*kv->value, 20);
  StringToIntMap_next_entry(&it);

  EXPECT_TRUE(StringToIntMap_has_entry(&it));
  kv = StringToIntMap_entry(&it);
  EXPECT_EQ(kv->key, "c");
  EXPECT_EQ(kv->key_size, strlen("c"));
  EXPECT_EQ(*kv->value, 30);
  StringToIntMap_next_entry(&it);

  EXPECT_FALSE(StringToIntMap_has_entry(&it));
}

/* Test fixture to ensure proper setup / teardown */
class IntToIntMapTest : public ::testing::Test {
 protected:
  IntToIntMap map{};

  void SetUp() override {
    ASSERT_TRUE(IntToIntMap_init(&map, hash_int, compare_ints));
  }

  void TearDown() override { IntToIntMap_finalize(&map); }
};

TEST_F(IntToIntMapTest, TestIntKey) {
  // IntToIntMap_insert(&map, 0, sizeof(int), 0);
  int *val;
  IntToIntMap_insert(&map, 1, sizeof(int), &val);
  *val = 11;
  IntToIntMap_insert(&map, 2, sizeof(int), &val);
  *val = 22;
  IntToIntMap_insert(&map, 3, sizeof(int), &val);
  *val = 33;

  EXPECT_FALSE(IntToIntMap_contains(&map, 0, sizeof(int)));
  EXPECT_TRUE(IntToIntMap_contains(&map, 1, sizeof(int)));
  EXPECT_TRUE(IntToIntMap_contains(&map, 2, sizeof(int)));
  EXPECT_TRUE(IntToIntMap_contains(&map, 3, sizeof(int)));

  EXPECT_EQ(IntToIntMap_find(&map, 0, sizeof(int), -1), -1);
  EXPECT_THAT(IntToIntMap_find_ref(&map, 0, sizeof(int)), IsNull());
  EXPECT_EQ(IntToIntMap_find(&map, 1, sizeof(int), -1), 11);
  EXPECT_EQ(*IntToIntMap_find_ref(&map, 1, sizeof(int)), 11);
  EXPECT_EQ(IntToIntMap_find(&map, 2, sizeof(int), -1), 22);
  EXPECT_EQ(*IntToIntMap_find_ref(&map, 2, sizeof(int)), 22);
  EXPECT_EQ(IntToIntMap_find(&map, 3, sizeof(int), -1), 33);
  EXPECT_EQ(*IntToIntMap_find_ref(&map, 3, sizeof(int)), 33);
}

}  // namespace