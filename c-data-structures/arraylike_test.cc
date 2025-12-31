#include "c-data-structures/arraylike.h"

#include <gtest/gtest.h>

namespace {
/* Instantiate an array type for testing */
DEFINE_ARRAYLIKE(IntArray, int);
IMPL_ARRAYLIKE(IntArray, int);

/* Test fixture to ensure proper setup / teardown */
class IntArrayTest : public ::testing::Test {
 protected:
  IntArray array{};

  void SetUp() override { ASSERT_TRUE(IntArray_init(&array)); }

  void TearDown() override { IntArray_finalize(&array); }
};

/* -------------------------------------------------------------
 * Initialization and basic state
 * ------------------------------------------------------------- */

TEST_F(IntArrayTest, StartsEmpty) {
  EXPECT_TRUE(IntArray_is_empty(&array));
  EXPECT_EQ(IntArray_size(&array), 0u);
}

TEST(IntArrayStandaloneTest, InitWithCapacity) {
  IntArray arr{};
  EXPECT_TRUE(IntArray_init_capacity(&arr, 16));
  EXPECT_EQ(arr.capacity, 16u);
  EXPECT_EQ(arr.size, 0u);
  IntArray_finalize(&arr);
}

TEST(IntArrayStandaloneTest, InitWithZeroCapacityFails) {
  IntArray arr{};
  EXPECT_FALSE(IntArray_init_capacity(&arr, 0));
}

/* -------------------------------------------------------------
 * Push / Pop Back
 * ------------------------------------------------------------- */

TEST_F(IntArrayTest, PushBackAndPopBack) {
  IntArray_push_back(&array, 10);
  IntArray_push_back(&array, 20);

  EXPECT_EQ(IntArray_size(&array), 2u);

  int value = 0;
  EXPECT_TRUE(IntArray_pop_back(&array, &value));
  EXPECT_EQ(value, 20);
  EXPECT_TRUE(IntArray_pop_back(&array, &value));
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(IntArray_is_empty(&array));
}

TEST_F(IntArrayTest, PushBackRef) {
  int* slot = IntArray_push_back_ref(&array);
  ASSERT_NE(slot, nullptr);
  *slot = 42;

  EXPECT_EQ(IntArray_size(&array), 1u);
  EXPECT_EQ(IntArray_get_unchecked(&array, 0), 42);
}

/* -------------------------------------------------------------
 * Push / Pop Front
 * ------------------------------------------------------------- */

TEST_F(IntArrayTest, PushFrontAndPopFront) {
  IntArray_push_front(&array, 1);
  IntArray_push_front(&array, 2);
  IntArray_push_front(&array, 3);

  EXPECT_EQ(IntArray_size(&array), 3u);

  int value = 0;
  EXPECT_TRUE(IntArray_pop_front(&array, &value));
  EXPECT_EQ(value, 3);
  EXPECT_TRUE(IntArray_pop_front(&array, &value));
  EXPECT_EQ(value, 2);
  EXPECT_TRUE(IntArray_pop_front(&array, &value));
  EXPECT_EQ(value, 1);
}

TEST_F(IntArrayTest, PushFrontRef) {
  int* slot = IntArray_push_front_ref(&array);
  ASSERT_NE(slot, nullptr);
  *slot = 99;

  EXPECT_EQ(IntArray_size(&array), 1u);
  EXPECT_EQ(IntArray_get_unchecked(&array, 0), 99);
}

/* -------------------------------------------------------------
 * Random access get / set
 * ------------------------------------------------------------- */

TEST_F(IntArrayTest, SetAndGetWithinBounds) {
  EXPECT_TRUE(IntArray_set(&array, 0, 10));
  EXPECT_TRUE(IntArray_set(&array, 1, 20));

  int value = 0;
  EXPECT_TRUE(IntArray_get(&array, 0, &value));
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(IntArray_get(&array, 1, &value));
  EXPECT_EQ(value, 20);
}

TEST_F(IntArrayTest, SetExpandsArray) {
  EXPECT_TRUE(IntArray_set(&array, 5, 55));
  EXPECT_EQ(IntArray_size(&array), 6u);
  EXPECT_EQ(IntArray_get_unchecked(&array, 5), 55);
}

TEST_F(IntArrayTest, SetNegativeIndexFails) {
  EXPECT_FALSE(IntArray_set(&array, -1, 123));
}

TEST_F(IntArrayTest, GetOutOfRangeFails) {
  int value = 0;
  EXPECT_FALSE(IntArray_get(&array, 0, &value));
}

/* -------------------------------------------------------------
 * Reference access
 * ------------------------------------------------------------- */

TEST_F(IntArrayTest, MutableRefAllowsModification) {
  IntArray_push_back(&array, 10);

  int* ref = nullptr;
  EXPECT_TRUE(IntArray_mutable_ref(&array, 0, &ref));
  *ref = 99;

  EXPECT_EQ(IntArray_get_unchecked(&array, 0), 99);
}

TEST_F(IntArrayTest, ConstRefAccess) {
  IntArray_push_back(&array, 42);

  const int* ref = nullptr;
  EXPECT_TRUE(IntArray_get_ref(&array, 0, &ref));
  EXPECT_EQ(*ref, 42);
}

/* -------------------------------------------------------------
 * Last element helpers
 * ------------------------------------------------------------- */

TEST_F(IntArrayTest, LastElementAccess) {
  IntArray_push_back(&array, 1);
  IntArray_push_back(&array, 2);
  IntArray_push_back(&array, 3);

  int value = 0;
  EXPECT_TRUE(IntArray_last(&array, &value));
  EXPECT_EQ(value, 3);

  EXPECT_EQ(IntArray_last_unchecked(&array), 3);
}

/* -------------------------------------------------------------
 * Removal
 * ------------------------------------------------------------- */

TEST_F(IntArrayTest, RemoveMiddleElement) {
  IntArray_push_back(&array, 1);
  IntArray_push_back(&array, 2);
  IntArray_push_back(&array, 3);

  int removed = 0;
  EXPECT_TRUE(IntArray_remove(&array, 1, &removed));
  EXPECT_EQ(removed, 2);
  EXPECT_EQ(IntArray_size(&array), 2u);

  EXPECT_EQ(IntArray_get_unchecked(&array, 0), 1);
  EXPECT_EQ(IntArray_get_unchecked(&array, 1), 3);
}

TEST_F(IntArrayTest, RemoveInvalidIndexFails) {
  int removed = 0;
  EXPECT_FALSE(IntArray_remove(&array, 0, &removed));
}

/* -------------------------------------------------------------
 * Shrinking
 * ------------------------------------------------------------- */

TEST_F(IntArrayTest, LeftShrink) {
  for (int i = 0; i < 5; ++i) {
    IntArray_push_back(&array, i);
  }

  EXPECT_TRUE(IntArray_lshrink(&array, 2));
  EXPECT_EQ(IntArray_size(&array), 3u);
  EXPECT_EQ(IntArray_get_unchecked(&array, 0), 2);
}

TEST_F(IntArrayTest, RightShrink) {
  for (int i = 0; i < 5; ++i) {
    IntArray_push_back(&array, i);
  }

  EXPECT_TRUE(IntArray_rshrink(&array, 2));
  EXPECT_EQ(IntArray_size(&array), 3u);
  EXPECT_EQ(IntArray_last_unchecked(&array), 2);
}

/* -------------------------------------------------------------
 * Copying and append
 * ------------------------------------------------------------- */

TEST_F(IntArrayTest, CopyCreatesIndependentArray) {
  IntArray_push_back(&array, 1);
  IntArray_push_back(&array, 2);

  IntArray* copy = IntArray_copy(&array);
  ASSERT_NE(copy, nullptr);

  EXPECT_EQ(IntArray_size(copy), 2u);
  EXPECT_EQ(IntArray_get_unchecked(copy, 0), 1);

  IntArray_push_back(&array, 3);
  EXPECT_EQ(IntArray_size(copy), 2u);

  IntArray_delete(copy);
}

TEST_F(IntArrayTest, AppendArray) {
  IntArray other{};
  ASSERT_TRUE(IntArray_init(&other));

  IntArray_push_back(&array, 1);
  IntArray_push_back(&array, 2);
  IntArray_push_back(&other, 3);
  IntArray_push_back(&other, 4);

  IntArray_append(&array, &other);
  EXPECT_EQ(IntArray_size(&array), 4u);
  EXPECT_EQ(IntArray_last_unchecked(&array), 4);

  IntArray_finalize(&other);
}

TEST_F(IntArrayTest, AppendRange) {
  IntArray other{};
  ASSERT_TRUE(IntArray_init(&other));

  for (int i = 0; i < 5; ++i) {
    IntArray_push_back(&other, i);
  }

  EXPECT_TRUE(IntArray_append_range(&array, &other, 1, 4));
  EXPECT_EQ(IntArray_size(&array), 3u);
  EXPECT_EQ(IntArray_get_unchecked(&array, 0), 1);
  EXPECT_EQ(IntArray_get_unchecked(&array, 2), 3);

  IntArray_finalize(&other);
}

/* -------------------------------------------------------------
 * Iteration
 * ------------------------------------------------------------- */

TEST_F(IntArrayTest, IteratorTraversesArray) {
  for (int i = 0; i < 4; ++i) {
    IntArray_push_back(&array, i * 10);
  }

  IntArrayIterator iter{};
  IntArray_iterator(&iter, &array);

  int expected = 0;
  while (IntArray_has_next(&iter)) {
    EXPECT_EQ(*IntArray_value(&iter), expected);
    expected += 10;
    IntArray_next(&iter);
  }
}
}  // namespace