extern "C" {
#include "c-data-structures/setlike.h"
}

#include <gtest/gtest.h>
#include <stdint.h>

#include <algorithm>

namespace {

DEFINE_SETLIKE(Int32HashSet, int32_t);
IMPL_SETLIKE(Int32HashSet, int32_t);

DEFINE_SETLIKE(StringHashSet, char *);
IMPL_SETLIKE(StringHashSet, char *);

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
#define FNV_32_PRIME (0x01000193)
#define FNV_1A_32_OFFSET (0x811C9DC5)

uint32_t hash_int32(const int32_t num, uint32_t size) { return (uint32_t)num; }

int32_t compare_int32s(const int32_t num1, uint32_t size1, const int32_t num2,
                       uint32_t size2) {
  return num1 - num2;
}

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

TEST(Int32HashSetTest, Init) {
  Int32HashSet hash_set;
  Int32HashSet_init(&hash_set, hash_int32, compare_int32s);
  Int32HashSet_finalize(&hash_set);
}

TEST(Int32HashSetTest, Create) {
  Int32HashSet *hash_set = Int32HashSet_create(hash_int32, compare_int32s);
  Int32HashSet_delete(hash_set);
}

TEST(Int32HashSetTest, Insert) {
  Int32HashSet hash_set;
  Int32HashSet_init(&hash_set, hash_int32, compare_int32s);
  // Insert
  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));

  // Verify
  ASSERT_TRUE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  ASSERT_FALSE(Int32HashSet_contains(&hash_set, 50, sizeof(int32_t)));

  Int32HashSet_finalize(&hash_set);
}

TEST(Int32HashSetTest, InsertN) {
  Int32HashSet hash_set;
  Int32HashSet_init(&hash_set, hash_int32, compare_int32s);

  // Insert
  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 20, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 30, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 40, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 50, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 60, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 70, sizeof(int32_t)));

  // Verify
  ASSERT_TRUE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_contains(&hash_set, 30, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_contains(&hash_set, 40, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_contains(&hash_set, 50, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_contains(&hash_set, 60, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_contains(&hash_set, 70, sizeof(int32_t)));

  ASSERT_FALSE(Int32HashSet_contains(&hash_set, 75, sizeof(int32_t)));
  ASSERT_FALSE(Int32HashSet_contains(&hash_set, 85, sizeof(int32_t)));
  ASSERT_FALSE(Int32HashSet_contains(&hash_set, 95, sizeof(int32_t)));

  Int32HashSet_finalize(&hash_set);
}

TEST(Int32HashSetTest, Remove) {
  Int32HashSet hash_set;
  Int32HashSet_init(&hash_set, hash_int32, compare_int32s);

  // Insert
  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  ASSERT_FALSE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));

  // Remove
  ASSERT_TRUE(Int32HashSet_remove(&hash_set, 10, sizeof(int32_t)));
  ASSERT_FALSE(Int32HashSet_remove(&hash_set, 20, sizeof(int32_t)));

  // Verify
  ASSERT_FALSE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  ASSERT_FALSE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));

  Int32HashSet_finalize(&hash_set);
}

TEST(Int32HashSetTest, Reinsert) {
  Int32HashSet hash_set;
  Int32HashSet_init(&hash_set, hash_int32, compare_int32s);

  // Insert
  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));
  ASSERT_TRUE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  ASSERT_FALSE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));

  // Remove
  ASSERT_TRUE(Int32HashSet_remove(&hash_set, 10, sizeof(int32_t)));
  ASSERT_FALSE(Int32HashSet_remove(&hash_set, 20, sizeof(int32_t)));

  // Verify removal
  ASSERT_FALSE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  ASSERT_FALSE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));

  // Reinsert
  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));

  // Verify reinsertion
  ASSERT_TRUE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  ASSERT_FALSE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));

  Int32HashSet_finalize(&hash_set);
}

TEST(Int32HashSetTest, Size) {
  Int32HashSet hash_set;
  Int32HashSet_init(&hash_set, hash_int32, compare_int32s);

  ASSERT_EQ(Int32HashSet_size(&hash_set), 0);

  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 1);

  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 20, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 2);

  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 30, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 3);

  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 40, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 4);

  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 50, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 5);

  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 60, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 6);

  ASSERT_TRUE(Int32HashSet_insert(&hash_set, 70, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 7);

  ASSERT_TRUE(Int32HashSet_remove(&hash_set, 10, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 6);

  ASSERT_TRUE(Int32HashSet_remove(&hash_set, 20, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 5);

  ASSERT_TRUE(Int32HashSet_remove(&hash_set, 30, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 4);

  ASSERT_TRUE(Int32HashSet_remove(&hash_set, 40, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 3);

  ASSERT_TRUE(Int32HashSet_remove(&hash_set, 50, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 2);

  ASSERT_TRUE(Int32HashSet_remove(&hash_set, 60, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 1);

  ASSERT_TRUE(Int32HashSet_remove(&hash_set, 70, sizeof(int32_t)));
  ASSERT_EQ(Int32HashSet_size(&hash_set), 0);

  Int32HashSet_finalize(&hash_set);
}

TEST(StringHashSetTest, Init) {
  StringHashSet hash_set;
  StringHashSet_init(&hash_set, hash_string, compare_strings);
  StringHashSet_finalize(&hash_set);
}

TEST(StringHashSetTest, Create) {
  StringHashSet *hash_set = StringHashSet_create(hash_string, compare_strings);
  StringHashSet_delete(hash_set);
}

TEST(StringHashSetTest, Insert) {
  StringHashSet hash_set;
  StringHashSet_init(&hash_set, hash_string, compare_strings);
  // Insert
  ASSERT_TRUE(StringHashSet_insert(&hash_set, "cat", sizeof("cat")));

  // Verify
  ASSERT_TRUE(StringHashSet_contains(&hash_set, "cat", sizeof("cat")));
  ASSERT_FALSE(StringHashSet_contains(&hash_set, "hat", sizeof("hat")));

  StringHashSet_finalize(&hash_set);
}

TEST(StringHashSetTest, InsertN) {
  StringHashSet hash_set;
  StringHashSet_init(&hash_set, hash_string, compare_strings);

  // Insert
  ASSERT_TRUE(StringHashSet_insert(&hash_set, "cat", sizeof("cat")));
  ASSERT_TRUE(StringHashSet_insert(&hash_set, "in", sizeof("in")));
  ASSERT_TRUE(StringHashSet_insert(&hash_set, "the", sizeof("the")));
  ASSERT_TRUE(StringHashSet_insert(&hash_set, "hat", sizeof("hat")));
  ASSERT_TRUE(StringHashSet_insert(&hash_set, "red", sizeof("red")));
  ASSERT_TRUE(StringHashSet_insert(&hash_set, "blue", sizeof("blue")));
  ASSERT_TRUE(StringHashSet_insert(&hash_set, "fish", sizeof("fish")));

  // Verify
  ASSERT_TRUE(StringHashSet_contains(&hash_set, "cat", sizeof("cat")));
  ASSERT_TRUE(StringHashSet_contains(&hash_set, "in", sizeof("in")));
  ASSERT_TRUE(StringHashSet_contains(&hash_set, "the", sizeof("the")));
  ASSERT_TRUE(StringHashSet_contains(&hash_set, "hat", sizeof("hat")));
  ASSERT_TRUE(StringHashSet_contains(&hash_set, "red", sizeof("red")));
  ASSERT_TRUE(StringHashSet_contains(&hash_set, "blue", sizeof("blue")));
  ASSERT_TRUE(StringHashSet_contains(&hash_set, "fish", sizeof("fish")));

  ASSERT_FALSE(StringHashSet_contains(&hash_set, "lorax", sizeof("lorax")));
  ASSERT_FALSE(StringHashSet_contains(&hash_set, "borax", sizeof("borax")));
  ASSERT_FALSE(
      StringHashSet_contains(&hash_set, "floor wax", sizeof("floor wax")));

  StringHashSet_finalize(&hash_set);
}

TEST(StringHashSetTest, Remove) {
  StringHashSet hash_set;
  StringHashSet_init(&hash_set, hash_string, compare_strings);

  // Insert
  ASSERT_TRUE(StringHashSet_insert(&hash_set, "cat", sizeof("cat")));
  ASSERT_TRUE(StringHashSet_contains(&hash_set, "cat", sizeof("cat")));
  ASSERT_FALSE(StringHashSet_contains(&hash_set, "hat", sizeof("hat")));

  // Remove
  ASSERT_TRUE(StringHashSet_remove(&hash_set, "cat", sizeof("cat")));
  ASSERT_FALSE(StringHashSet_remove(&hash_set, "hat", sizeof("hat")));

  // Verify
  ASSERT_FALSE(StringHashSet_contains(&hash_set, "cat", sizeof("cat")));
  ASSERT_FALSE(StringHashSet_contains(&hash_set, "hat", sizeof("hat")));

  StringHashSet_finalize(&hash_set);
}

TEST(StringHashSetTest, Reinsert) {
  StringHashSet hash_set;
  StringHashSet_init(&hash_set, hash_string, compare_strings);

  // Insert
  ASSERT_TRUE(StringHashSet_insert(&hash_set, "cat", sizeof("cat")));
  ASSERT_TRUE(StringHashSet_contains(&hash_set, "cat", sizeof("cat")));
  ASSERT_FALSE(StringHashSet_contains(&hash_set, "hat", sizeof("hat")));

  // Remove
  ASSERT_TRUE(StringHashSet_remove(&hash_set, "cat", sizeof("cat")));
  ASSERT_FALSE(StringHashSet_remove(&hash_set, "hat", sizeof("hat")));

  // Verify removal
  ASSERT_FALSE(StringHashSet_contains(&hash_set, "cat", sizeof("cat")));
  ASSERT_FALSE(StringHashSet_contains(&hash_set, "hat", sizeof("hat")));

  // Reinsert
  ASSERT_TRUE(StringHashSet_insert(&hash_set, "cat", sizeof("cat")));

  // Verify reinsertion
  ASSERT_TRUE(StringHashSet_contains(&hash_set, "cat", sizeof("cat")));
  ASSERT_FALSE(StringHashSet_contains(&hash_set, "hat", sizeof("hat")));

  StringHashSet_finalize(&hash_set);
}

TEST(StringHashSetTest, Size) {
  StringHashSet hash_set;
  StringHashSet_init(&hash_set, hash_string, compare_strings);

  ASSERT_EQ(StringHashSet_size(&hash_set), 0);

  ASSERT_TRUE(StringHashSet_insert(&hash_set, "cat", sizeof("cat")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 1);

  ASSERT_TRUE(StringHashSet_insert(&hash_set, "in", sizeof("in")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 2);

  ASSERT_TRUE(StringHashSet_insert(&hash_set, "the", sizeof("the")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 3);

  ASSERT_TRUE(StringHashSet_insert(&hash_set, "hat", sizeof("hat")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 4);

  ASSERT_TRUE(StringHashSet_insert(&hash_set, "red", sizeof("red")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 5);

  ASSERT_TRUE(StringHashSet_insert(&hash_set, "blue", sizeof("blue")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 6);

  ASSERT_TRUE(StringHashSet_insert(&hash_set, "fish", sizeof("fish")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 7);

  ASSERT_TRUE(StringHashSet_remove(&hash_set, "cat", sizeof("cat")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 6);

  ASSERT_TRUE(StringHashSet_remove(&hash_set, "in", sizeof("in")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 5);

  ASSERT_TRUE(StringHashSet_remove(&hash_set, "the", sizeof("the")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 4);

  ASSERT_TRUE(StringHashSet_remove(&hash_set, "hat", sizeof("hat")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 3);

  ASSERT_TRUE(StringHashSet_remove(&hash_set, "red", sizeof("red")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 2);

  ASSERT_TRUE(StringHashSet_remove(&hash_set, "blue", sizeof("blue")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 1);

  ASSERT_TRUE(StringHashSet_remove(&hash_set, "fish", sizeof("fish")));
  ASSERT_EQ(StringHashSet_size(&hash_set), 0);

  StringHashSet_finalize(&hash_set);
}

TEST(StringHashSetTest, Iterator) {
  StringHashSet hash_set;
  StringHashSet_init(&hash_set, hash_string, compare_strings);

  StringHashSet_insert(&hash_set, "a", strlen("a"));
  StringHashSet_insert(&hash_set, "b", strlen("b"));
  StringHashSet_insert(&hash_set, "c", strlen("c"));

  StringHashSetIterator it;
  StringHashSet_iterator(&it, &hash_set);

  EXPECT_TRUE(StringHashSet_has_next(&it));
  EXPECT_EQ(StringHashSet_value_size(&it), strlen("a"));
  EXPECT_STREQ(*StringHashSet_value(&it), "a");
  StringHashSet_next(&it);

  EXPECT_TRUE(StringHashSet_has_next(&it));
  EXPECT_EQ(StringHashSet_value_size(&it), strlen("b"));
  EXPECT_EQ(*StringHashSet_value(&it), "b");
  StringHashSet_next(&it);

  EXPECT_TRUE(StringHashSet_has_next(&it));
  EXPECT_EQ(StringHashSet_value_size(&it), strlen("c"));
  EXPECT_EQ(*StringHashSet_value(&it), "c");
  StringHashSet_next(&it);

  EXPECT_FALSE(StringHashSet_has_next(&it));
}

}  // namespace