#include "c-data-structures/maplike.h"

#include <gtest/gtest.h>

namespace {

/* Instantiate a map type for testing */
DEFINE_MAPLIKE(StringToIntMap, char*, int);
IMPL_MAPLIKE(StringToIntMap, char*, int);

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
#define FNV_32_PRIME (0x01000193)
#define FNV_1A_32_OFFSET (0x811C9DC5)

uint32_t hash_int32(const int32_t num, uint32_t size) { return (uint32_t)num; }

int32_t compare_int32s(const int32_t num1, uint32_t size1, const int32_t num2,
                       uint32_t size2) {
  return num1 - num2;
}

uint32_t hash_string(const char* ptr, uint32_t size) {
  unsigned char* s = (unsigned char*)ptr;
  uint32_t hval = FNV_1A_32_OFFSET;
  for (uint32_t i = 0; i < size; ++i) {
    hval *= FNV_32_PRIME;
    hval ^= (uint32_t)*s++;
  }
  return hval;
}

int32_t compare_strings(const char* ptr1, uint32_t size1, const char* ptr2,
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

TEST(StringToIntMapStandaloneTest, InitWithCapacity) {
  StringToIntMap map{};
  EXPECT_TRUE(
      StringToIntMap_init_capacity(&map, 16, hash_string, compare_strings));
  EXPECT_EQ(map.capacity, 16u);
  EXPECT_EQ(StringToIntMap_size(&map), 0u);
  StringToIntMap_finalize(&map);
}

TEST(StringToIntMapStandaloneTest, InitWithZeroCapacityFails) {
  StringToIntMap map{};
  EXPECT_FALSE(
      StringToIntMap_init_capacity(&map, 0, hash_string, compare_strings));
}

// /* -------------------------------------------------------------
//  * Insert / Remove
//  * ------------------------------------------------------------- */

TEST_F(StringToIntMapTest, InsertAndRemove) {
  StringToIntMap_insert(&map, "a", strlen("a"), 10);
  StringToIntMap_insert(&map, "b", strlen("b"), 20);
  StringToIntMap_insert(&map, "c", strlen("c"), 30);

  EXPECT_EQ(StringToIntMap_size(&map), 3);

  int value = 0;
  EXPECT_TRUE(StringToIntMap_remove(&map, "a", strlen("a"), &value));
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(StringToIntMap_remove(&map, "b", strlen("b"), &value));
  EXPECT_EQ(value, 20);
  EXPECT_TRUE(StringToIntMap_remove(&map, "c", strlen("c"), &value));
  EXPECT_EQ(value, 30);

  EXPECT_EQ(StringToIntMap_size(&map), 0);
}

// /* -------------------------------------------------------------
//  * Find
//  * ------------------------------------------------------------- */

TEST_F(StringToIntMapTest, FindExistingValue) {
  StringToIntMap_insert(&map, "a", strlen("a"), 10);
  StringToIntMap_insert(&map, "b", strlen("b"), 20);
  StringToIntMap_insert(&map, "c", strlen("c"), 30);
  EXPECT_EQ(StringToIntMap_size(&map), 3);

  EXPECT_EQ(StringToIntMap_find(&map, "a", strlen("a"), -1), 10);
  EXPECT_EQ(StringToIntMap_find(&map, "b", strlen("b"), -1), 20);
  EXPECT_EQ(StringToIntMap_find(&map, "c", strlen("c"), -1), 30);
}

TEST_F(StringToIntMapTest, DoesNotFindValue) {
  StringToIntMap_insert(&map, "a", strlen("a"), 10);
  StringToIntMap_insert(&map, "b", strlen("b"), 20);
  StringToIntMap_insert(&map, "c", strlen("c"), 30);
  EXPECT_EQ(StringToIntMap_size(&map), 3);

  EXPECT_EQ(StringToIntMap_find(&map, "d", strlen("d"), -1), -1);

  StringToIntMap_insert(&map, "d", strlen("d"), 40);
  EXPECT_EQ(StringToIntMap_size(&map), 4);

  EXPECT_EQ(StringToIntMap_find(&map, "d", strlen("d"), -1), 40);
}

TEST_F(StringToIntMapTest, DoesNotFindAfterRemoval) {
  StringToIntMap_insert(&map, "a", strlen("a"), 10);
  StringToIntMap_insert(&map, "b", strlen("b"), 20);
  StringToIntMap_insert(&map, "c", strlen("c"), 30);
  EXPECT_EQ(StringToIntMap_size(&map), 3);

  EXPECT_EQ(StringToIntMap_find(&map, "a", strlen("a"), -1), 10);
  EXPECT_EQ(StringToIntMap_find(&map, "b", strlen("b"), -1), 20);
  EXPECT_EQ(StringToIntMap_find(&map, "c", strlen("c"), -1), 30);

  int value = 0;
  EXPECT_TRUE(StringToIntMap_remove(&map, "a", strlen("a"), &value));
  EXPECT_TRUE(StringToIntMap_remove(&map, "b", strlen("b"), &value));
  EXPECT_TRUE(StringToIntMap_remove(&map, "c", strlen("c"), &value));

  EXPECT_EQ(StringToIntMap_find(&map, "a", strlen("a"), -1), -1);
  EXPECT_EQ(StringToIntMap_find(&map, "b", strlen("b"), -1), -1);
  EXPECT_EQ(StringToIntMap_find(&map, "c", strlen("c"), -1), -1);
}

// /* -------------------------------------------------------------
//  * Contains
//  * ------------------------------------------------------------- */

TEST_F(StringToIntMapTest, Contains) {
  StringToIntMap_insert(&map, "a", strlen("a"), 10);
  StringToIntMap_insert(&map, "b", strlen("b"), 20);
  StringToIntMap_insert(&map, "c", strlen("c"), 30);

  EXPECT_TRUE(StringToIntMap_contains(&map, "a", strlen("a")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "b", strlen("b")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "c", strlen("c")));

  EXPECT_FALSE(StringToIntMap_contains(&map, "d", strlen("d")));
}

TEST_F(StringToIntMapTest, Contains2) {
  EXPECT_FALSE(StringToIntMap_contains(&map, "a", strlen("a")));
  EXPECT_FALSE(StringToIntMap_contains(&map, "b", strlen("b")));
  EXPECT_FALSE(StringToIntMap_contains(&map, "c", strlen("c")));

  StringToIntMap_insert(&map, "a", strlen("a"), 10);
  StringToIntMap_insert(&map, "b", strlen("b"), 20);
  StringToIntMap_insert(&map, "c", strlen("c"), 30);

  EXPECT_TRUE(StringToIntMap_contains(&map, "a", strlen("a")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "b", strlen("b")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "c", strlen("c")));

  EXPECT_FALSE(StringToIntMap_contains(&map, "d", strlen("d")));
  StringToIntMap_insert(&map, "d", strlen("d"), 40);
  EXPECT_TRUE(StringToIntMap_contains(&map, "d", strlen("d")));
}

TEST_F(StringToIntMapTest, DoesNotContainAfterRemoval) {
  StringToIntMap_insert(&map, "a", strlen("a"), 10);
  StringToIntMap_insert(&map, "b", strlen("b"), 20);
  StringToIntMap_insert(&map, "c", strlen("c"), 30);

  EXPECT_TRUE(StringToIntMap_contains(&map, "a", strlen("a")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "b", strlen("b")));
  EXPECT_TRUE(StringToIntMap_contains(&map, "c", strlen("c")));

  int value = 0;
  EXPECT_TRUE(StringToIntMap_remove(&map, "a", strlen("a"), &value));
  EXPECT_TRUE(StringToIntMap_remove(&map, "b", strlen("b"), &value));
  EXPECT_TRUE(StringToIntMap_remove(&map, "c", strlen("c"), &value));

  EXPECT_FALSE(StringToIntMap_contains(&map, "a", strlen("a")));
  EXPECT_FALSE(StringToIntMap_contains(&map, "b", strlen("b")));
  EXPECT_FALSE(StringToIntMap_contains(&map, "c", strlen("c")));
}

// /* -------------------------------------------------------------
//  * Iterator
//  * ------------------------------------------------------------- */

TEST_F(StringToIntMapTest, Iterator) {
  StringToIntMap_insert(&map, "a", strlen("a"), 10);
  StringToIntMap_insert(&map, "b", strlen("b"), 20);
  StringToIntMap_insert(&map, "c", strlen("c"), 30);

  StringToIntMapIterator it;
  StringToIntMap_iterator(&it, &map);
  EXPECT_TRUE(StringToIntMap_has_entry(&it));
  StringToIntMapPair* kv = StringToIntMap_entry(&it);
  EXPECT_EQ(kv->key, "a");
  EXPECT_EQ(kv->key_size, strlen("a"));
  EXPECT_EQ(kv->value, 10);
  StringToIntMap_next_entry(&it);

  EXPECT_TRUE(StringToIntMap_has_entry(&it));
  kv = StringToIntMap_entry(&it);
  EXPECT_EQ(kv->key, "b");
  EXPECT_EQ(kv->key_size, strlen("b"));
  EXPECT_EQ(kv->value, 20);
  StringToIntMap_next_entry(&it);

  EXPECT_TRUE(StringToIntMap_has_entry(&it));
  kv = StringToIntMap_entry(&it);
  EXPECT_EQ(kv->key, "c");
  EXPECT_EQ(kv->key_size, strlen("c"));
  EXPECT_EQ(kv->value, 30);
  StringToIntMap_next_entry(&it);

  EXPECT_FALSE(StringToIntMap_has_entry(&it));
}

}  // namespace