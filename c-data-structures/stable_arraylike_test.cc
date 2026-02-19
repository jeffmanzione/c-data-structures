#include "c-data-structures/stable_arraylike.h"

#include <gtest/gtest.h>
#include <stdint.h>

namespace {

// Instantiate the implementation
DEFINE_STABLE_ARRAYLIKE(StableIntArray, int);
IMPL_STABLE_ARRAYLIKE(StableIntArray, int);

/* Test fixture to ensure proper setup / teardown */
class StableIntArrayTest : public ::testing::Test {
 protected:
  StableIntArray array{};

  void SetUp() override { ASSERT_TRUE(StableIntArray_init(&array)); }

  void TearDown() override { StableIntArray_finalize(&array); }
};

/* -------------------------------------------------------------
 * Initialization and basic state
 * ------------------------------------------------------------- */

TEST_F(StableIntArrayTest, StartsEmpty) {
  EXPECT_TRUE(StableIntArray_is_empty(&array));
  EXPECT_EQ(StableIntArray_size(&array), 0u);
}

/* -------------------------------------------------------------
 * Push / Pop Back
 * ------------------------------------------------------------- */

TEST_F(StableIntArrayTest, PushBackAndPopBack) {
  StableIntArray_push_back(&array, 10);
  StableIntArray_push_back(&array, 20);

  EXPECT_EQ(StableIntArray_size(&array), 2u);

  int value = 0;
  EXPECT_TRUE(StableIntArray_pop_back(&array, &value));
  EXPECT_EQ(value, 20);
  EXPECT_TRUE(StableIntArray_pop_back(&array, &value));
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(StableIntArray_is_empty(&array));
}

TEST_F(StableIntArrayTest, PushBackRef) {
  int* slot = StableIntArray_push_back_ref(&array);
  ASSERT_NE(slot, nullptr);
  *slot = 42;

  EXPECT_EQ(StableIntArray_size(&array), 1u);
  EXPECT_EQ(StableIntArray_get_unchecked(&array, 0), 42);
}

/* -------------------------------------------------------------
 * Random access get / set
 * ------------------------------------------------------------- */

TEST_F(StableIntArrayTest, SetAndGetWithinBounds) {
  StableIntArray_push_back(&array, 10);
  StableIntArray_push_back(&array, 20);

  int value = 0;
  EXPECT_TRUE(StableIntArray_get(&array, 0, &value));
  EXPECT_EQ(value, 10);
  EXPECT_TRUE(StableIntArray_get(&array, 1, &value));
  EXPECT_EQ(value, 20);

  EXPECT_TRUE(StableIntArray_set(&array, 0, 11));
  EXPECT_TRUE(StableIntArray_set(&array, 1, 22));

  EXPECT_TRUE(StableIntArray_get(&array, 0, &value));
  EXPECT_EQ(value, 11);
  EXPECT_TRUE(StableIntArray_get(&array, 1, &value));
  EXPECT_EQ(value, 22);
}

TEST_F(StableIntArrayTest, SetNegativeIndexFails) {
  EXPECT_FALSE(StableIntArray_set(&array, -1, 123));
}

TEST_F(StableIntArrayTest, GetOutOfRangeFails) {
  int value = 0;
  EXPECT_FALSE(StableIntArray_get(&array, 0, &value));
}

/* -------------------------------------------------------------
 * Reference access
 * ------------------------------------------------------------- */

TEST_F(StableIntArrayTest, MutableRefAllowsModification) {
  StableIntArray_push_back(&array, 10);

  int* ref = nullptr;
  EXPECT_TRUE(StableIntArray_mutable_ref(&array, 0, &ref));
  *ref = 99;

  EXPECT_EQ(StableIntArray_get_unchecked(&array, 0), 99);
}

TEST_F(StableIntArrayTest, ConstRefAccess) {
  StableIntArray_push_back(&array, 42);

  const int* ref = nullptr;
  EXPECT_TRUE(StableIntArray_get_ref(&array, 0, &ref));
  EXPECT_EQ(*ref, 42);
}

/* -------------------------------------------------------------
 * Last element helpers
 * ------------------------------------------------------------- */

TEST_F(StableIntArrayTest, LastElementAccess) {
  StableIntArray_push_back(&array, 1);
  StableIntArray_push_back(&array, 2);
  StableIntArray_push_back(&array, 3);

  int value = 0;
  EXPECT_TRUE(StableIntArray_last(&array, &value));
  EXPECT_EQ(value, 3);

  EXPECT_EQ(StableIntArray_last_unchecked(&array), 3);
}

}  // namespace