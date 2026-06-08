extern "C" {
#include "c-data-structures/setlike.h"
}

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdint.h>

#include <algorithm>

namespace {
using testing::IsNull;
using testing::NotNull;

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
  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));

  // Verify
  EXPECT_TRUE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  EXPECT_FALSE(Int32HashSet_contains(&hash_set, 50, sizeof(int32_t)));

  Int32HashSet_finalize(&hash_set);
}

TEST(Int32HashSetTest, InsertN) {
  Int32HashSet hash_set;
  Int32HashSet_init(&hash_set, hash_int32, compare_int32s);

  // Insert
  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 20, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 30, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 40, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 50, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 60, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 70, sizeof(int32_t)));

  // Verify
  EXPECT_TRUE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_contains(&hash_set, 30, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_contains(&hash_set, 40, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_contains(&hash_set, 50, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_contains(&hash_set, 60, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_contains(&hash_set, 70, sizeof(int32_t)));

  EXPECT_FALSE(Int32HashSet_contains(&hash_set, 75, sizeof(int32_t)));
  EXPECT_FALSE(Int32HashSet_contains(&hash_set, 85, sizeof(int32_t)));
  EXPECT_FALSE(Int32HashSet_contains(&hash_set, 95, sizeof(int32_t)));

  Int32HashSet_finalize(&hash_set);
}

TEST(Int32HashSetTest, Remove) {
  Int32HashSet hash_set;
  Int32HashSet_init(&hash_set, hash_int32, compare_int32s);

  // Insert
  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_find(&hash_set, 10, sizeof(int32_t), -1), 10);
  EXPECT_THAT(Int32HashSet_find_ref(&hash_set, 10, sizeof(int32_t)), NotNull());
  EXPECT_EQ(*Int32HashSet_find_ref(&hash_set, 10, sizeof(int32_t)), 10);
  EXPECT_FALSE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_find(&hash_set, 20, sizeof(int32_t), -1), -1);
  EXPECT_THAT(Int32HashSet_find_ref(&hash_set, 20, sizeof(int32_t)), IsNull());

  // Remove
  EXPECT_TRUE(Int32HashSet_remove(&hash_set, 10, sizeof(int32_t)));
  EXPECT_FALSE(Int32HashSet_remove(&hash_set, 20, sizeof(int32_t)));

  // Verify

  EXPECT_FALSE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_find(&hash_set, 10, sizeof(int32_t), -1), -1);
  EXPECT_THAT(Int32HashSet_find_ref(&hash_set, 10, sizeof(int32_t)), IsNull());

  EXPECT_FALSE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_find(&hash_set, 20, sizeof(int32_t), -1), -1);
  EXPECT_THAT(Int32HashSet_find_ref(&hash_set, 20, sizeof(int32_t)), IsNull());

  Int32HashSet_finalize(&hash_set);
}

TEST(Int32HashSetTest, Reinsert) {
  Int32HashSet hash_set;
  Int32HashSet_init(&hash_set, hash_int32, compare_int32s);

  // Insert
  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));
  EXPECT_TRUE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_find(&hash_set, 10, sizeof(int32_t), -1), 10);
  EXPECT_THAT(Int32HashSet_find_ref(&hash_set, 10, sizeof(int32_t)), NotNull());
  EXPECT_EQ(*Int32HashSet_find_ref(&hash_set, 10, sizeof(int32_t)), 10);
  EXPECT_FALSE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_find(&hash_set, 20, sizeof(int32_t), -1), -1);
  EXPECT_THAT(Int32HashSet_find_ref(&hash_set, 20, sizeof(int32_t)), IsNull());

  // Remove
  EXPECT_TRUE(Int32HashSet_remove(&hash_set, 10, sizeof(int32_t)));
  EXPECT_FALSE(Int32HashSet_remove(&hash_set, 20, sizeof(int32_t)));

  // Verify removal
  EXPECT_FALSE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_find(&hash_set, 10, sizeof(int32_t), -1), -1);
  EXPECT_THAT(Int32HashSet_find_ref(&hash_set, 10, sizeof(int32_t)), IsNull());

  EXPECT_FALSE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_find(&hash_set, 20, sizeof(int32_t), -1), -1);
  EXPECT_THAT(Int32HashSet_find_ref(&hash_set, 20, sizeof(int32_t)), IsNull());

  // Reinsert
  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));

  // Verify reinsertion
  EXPECT_TRUE(Int32HashSet_contains(&hash_set, 10, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_find(&hash_set, 10, sizeof(int32_t), -1), 10);
  EXPECT_THAT(Int32HashSet_find_ref(&hash_set, 10, sizeof(int32_t)), NotNull());
  EXPECT_EQ(*Int32HashSet_find_ref(&hash_set, 10, sizeof(int32_t)), 10);

  EXPECT_FALSE(Int32HashSet_contains(&hash_set, 20, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_find(&hash_set, 20, sizeof(int32_t), -1), -1);
  EXPECT_THAT(Int32HashSet_find_ref(&hash_set, 20, sizeof(int32_t)), IsNull());

  Int32HashSet_finalize(&hash_set);
}

TEST(Int32HashSetTest, Size) {
  Int32HashSet hash_set;
  Int32HashSet_init(&hash_set, hash_int32, compare_int32s);

  EXPECT_EQ(Int32HashSet_size(&hash_set), 0);

  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 10, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 1);

  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 20, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 2);

  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 30, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 3);

  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 40, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 4);

  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 50, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 5);

  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 60, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 6);

  EXPECT_TRUE(Int32HashSet_insert(&hash_set, 70, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 7);

  EXPECT_TRUE(Int32HashSet_remove(&hash_set, 10, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 6);

  EXPECT_TRUE(Int32HashSet_remove(&hash_set, 20, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 5);

  EXPECT_TRUE(Int32HashSet_remove(&hash_set, 30, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 4);

  EXPECT_TRUE(Int32HashSet_remove(&hash_set, 40, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 3);

  EXPECT_TRUE(Int32HashSet_remove(&hash_set, 50, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 2);

  EXPECT_TRUE(Int32HashSet_remove(&hash_set, 60, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 1);

  EXPECT_TRUE(Int32HashSet_remove(&hash_set, 70, sizeof(int32_t)));
  EXPECT_EQ(Int32HashSet_size(&hash_set), 0);

  Int32HashSet_finalize(&hash_set);
}

TEST(Int32HashSetTest, Iterator) {
  Int32HashSet hash_set;
  Int32HashSet_init(&hash_set, hash_int32, compare_int32s);

  Int32HashSet_insert(&hash_set, 10, sizeof(int32_t));
  Int32HashSet_insert(&hash_set, 20, sizeof(int32_t));
  Int32HashSet_insert(&hash_set, 30, sizeof(int32_t));

  Int32HashSetIterator it;
  Int32HashSet_iterator(&it, &hash_set);

  EXPECT_TRUE(Int32HashSet_has_next(&it));
  EXPECT_EQ(Int32HashSet_value_size(&it), sizeof(int32_t));
  EXPECT_EQ(*Int32HashSet_value(&it), 10);
  EXPECT_EQ(*Int32HashSet_mutable_value(&it), 10);
  Int32HashSet_next(&it);

  EXPECT_TRUE(Int32HashSet_has_next(&it));
  EXPECT_EQ(Int32HashSet_value_size(&it), sizeof(int32_t));
  EXPECT_EQ(*Int32HashSet_value(&it), 20);
  EXPECT_EQ(*Int32HashSet_mutable_value(&it), 20);
  Int32HashSet_next(&it);

  EXPECT_TRUE(Int32HashSet_has_next(&it));
  EXPECT_EQ(Int32HashSet_value_size(&it), sizeof(int32_t));
  EXPECT_EQ(*Int32HashSet_value(&it), 30);
  EXPECT_EQ(*Int32HashSet_mutable_value(&it), 30);
  Int32HashSet_next(&it);

  EXPECT_FALSE(Int32HashSet_has_next(&it));
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
  EXPECT_TRUE(StringHashSet_insert(&hash_set, "cat", strlen("cat")));

  // Verify
  EXPECT_TRUE(StringHashSet_contains(&hash_set, "cat", strlen("cat")));
  EXPECT_FALSE(StringHashSet_contains(&hash_set, "hat", strlen("hat")));

  StringHashSet_finalize(&hash_set);
}

TEST(StringHashSetTest, InsertN) {
  StringHashSet hash_set;
  StringHashSet_init(&hash_set, hash_string, compare_strings);

  // Insert
  EXPECT_TRUE(StringHashSet_insert(&hash_set, "cat", strlen("cat")));
  EXPECT_TRUE(StringHashSet_insert(&hash_set, "in", strlen("in")));
  EXPECT_TRUE(StringHashSet_insert(&hash_set, "the", strlen("the")));
  EXPECT_TRUE(StringHashSet_insert(&hash_set, "hat", strlen("hat")));
  EXPECT_TRUE(StringHashSet_insert(&hash_set, "red", strlen("red")));
  EXPECT_TRUE(StringHashSet_insert(&hash_set, "blue", strlen("blue")));
  EXPECT_TRUE(StringHashSet_insert(&hash_set, "fish", strlen("fish")));

  // Verify
  EXPECT_TRUE(StringHashSet_contains(&hash_set, "cat", strlen("cat")));
  EXPECT_TRUE(StringHashSet_contains(&hash_set, "in", strlen("in")));
  EXPECT_TRUE(StringHashSet_contains(&hash_set, "the", strlen("the")));
  EXPECT_TRUE(StringHashSet_contains(&hash_set, "hat", strlen("hat")));
  EXPECT_TRUE(StringHashSet_contains(&hash_set, "red", strlen("red")));
  EXPECT_TRUE(StringHashSet_contains(&hash_set, "blue", strlen("blue")));
  EXPECT_TRUE(StringHashSet_contains(&hash_set, "fish", strlen("fish")));

  EXPECT_FALSE(StringHashSet_contains(&hash_set, "lorax", strlen("lorax")));
  EXPECT_FALSE(StringHashSet_contains(&hash_set, "borax", strlen("borax")));
  EXPECT_FALSE(
      StringHashSet_contains(&hash_set, "floor wax", strlen("floor wax")));

  StringHashSet_finalize(&hash_set);
}

TEST(StringHashSetTest, Remove) {
  StringHashSet hash_set;
  StringHashSet_init(&hash_set, hash_string, compare_strings);

  // Insert
  EXPECT_TRUE(StringHashSet_insert(&hash_set, "cat", strlen("cat")));
  EXPECT_TRUE(StringHashSet_contains(&hash_set, "cat", strlen("cat")));
  EXPECT_FALSE(StringHashSet_contains(&hash_set, "hat", strlen("hat")));

  // Remove
  EXPECT_TRUE(StringHashSet_remove(&hash_set, "cat", strlen("cat")));
  EXPECT_FALSE(StringHashSet_remove(&hash_set, "hat", strlen("hat")));

  // Verify
  EXPECT_FALSE(StringHashSet_contains(&hash_set, "cat", strlen("cat")));
  EXPECT_FALSE(StringHashSet_contains(&hash_set, "hat", strlen("hat")));

  StringHashSet_finalize(&hash_set);
}

TEST(StringHashSetTest, Reinsert) {
  StringHashSet hash_set;
  StringHashSet_init(&hash_set, hash_string, compare_strings);

  // Insert
  EXPECT_TRUE(StringHashSet_insert(&hash_set, "cat", strlen("cat")));
  EXPECT_STREQ(StringHashSet_find(&hash_set, "cat", strlen("cat"), (char *)""),
               "cat");
  EXPECT_STREQ(*StringHashSet_find_ref(&hash_set, "cat", strlen("cat")), "cat");
  EXPECT_TRUE(StringHashSet_contains(&hash_set, "cat", strlen("cat")));
  EXPECT_FALSE(StringHashSet_contains(&hash_set, "hat", strlen("hat")));

  // Remove
  EXPECT_TRUE(StringHashSet_remove(&hash_set, "cat", strlen("cat")));
  EXPECT_FALSE(StringHashSet_remove(&hash_set, "hat", strlen("hat")));

  // Verify removal
  EXPECT_FALSE(StringHashSet_contains(&hash_set, "cat", strlen("cat")));
  EXPECT_FALSE(StringHashSet_contains(&hash_set, "hat", strlen("hat")));

  // Reinsert
  EXPECT_TRUE(StringHashSet_insert(&hash_set, "cat", strlen("cat")));

  // Verify reinsertion
  EXPECT_TRUE(StringHashSet_contains(&hash_set, "cat", strlen("cat")));
  EXPECT_STREQ(StringHashSet_find(&hash_set, "cat", strlen("cat"), (char *)""),
               "cat");
  EXPECT_STREQ(*StringHashSet_find_ref(&hash_set, "cat", strlen("cat")), "cat");
  EXPECT_FALSE(StringHashSet_contains(&hash_set, "hat", strlen("hat")));

  StringHashSet_finalize(&hash_set);
}

TEST(StringHashSetTest, Size) {
  StringHashSet hash_set;
  StringHashSet_init(&hash_set, hash_string, compare_strings);

  EXPECT_EQ(StringHashSet_size(&hash_set), 0);

  EXPECT_TRUE(StringHashSet_insert(&hash_set, "cat", strlen("cat")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 1);

  EXPECT_TRUE(StringHashSet_insert(&hash_set, "in", strlen("in")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 2);

  EXPECT_TRUE(StringHashSet_insert(&hash_set, "the", strlen("the")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 3);

  EXPECT_TRUE(StringHashSet_insert(&hash_set, "hat", strlen("hat")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 4);

  EXPECT_TRUE(StringHashSet_insert(&hash_set, "red", strlen("red")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 5);

  EXPECT_TRUE(StringHashSet_insert(&hash_set, "blue", strlen("blue")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 6);

  EXPECT_TRUE(StringHashSet_insert(&hash_set, "fish", strlen("fish")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 7);

  EXPECT_TRUE(StringHashSet_remove(&hash_set, "cat", strlen("cat")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 6);

  EXPECT_TRUE(StringHashSet_remove(&hash_set, "in", strlen("in")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 5);

  EXPECT_TRUE(StringHashSet_remove(&hash_set, "the", strlen("the")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 4);

  EXPECT_TRUE(StringHashSet_remove(&hash_set, "hat", strlen("hat")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 3);

  EXPECT_TRUE(StringHashSet_remove(&hash_set, "red", strlen("red")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 2);

  EXPECT_TRUE(StringHashSet_remove(&hash_set, "blue", strlen("blue")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 1);

  EXPECT_TRUE(StringHashSet_remove(&hash_set, "fish", strlen("fish")));
  EXPECT_EQ(StringHashSet_size(&hash_set), 0);

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
  EXPECT_STREQ(*StringHashSet_mutable_value(&it), "a");
  StringHashSet_next(&it);

  EXPECT_TRUE(StringHashSet_has_next(&it));
  EXPECT_EQ(StringHashSet_value_size(&it), strlen("b"));
  EXPECT_EQ(*StringHashSet_value(&it), "b");
  EXPECT_STREQ(*StringHashSet_mutable_value(&it), "b");
  StringHashSet_next(&it);

  EXPECT_TRUE(StringHashSet_has_next(&it));
  EXPECT_EQ(StringHashSet_value_size(&it), strlen("c"));
  EXPECT_EQ(*StringHashSet_value(&it), "c");
  EXPECT_STREQ(*StringHashSet_mutable_value(&it), "c");
  StringHashSet_next(&it);

  EXPECT_FALSE(StringHashSet_has_next(&it));
}

}  // namespace