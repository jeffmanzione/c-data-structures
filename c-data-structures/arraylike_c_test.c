// Tests that arraylike links when compiled as C without optimizations, where
// inline functions without an external definition cause undefined symbols.
#include <stdio.h>

#include "c-data-structures/arraylike.h"

DEFINE_ARRAYLIKE(IntArray, int);
IMPL_ARRAYLIKE(IntArray, int);

#define CHECK(cond)                                         \
  if (!(cond)) {                                            \
    fprintf(stderr, "%s:%d: CHECK(%s)\n", __FILE__, __LINE__, #cond); \
    return 1;                                               \
  }

int main(void) {
  IntArray array;
  CHECK(IntArray_init(&array));
  for (int i = 0; i < 100; ++i) {
    IntArray_push_back(&array, i);
  }
  CHECK(IntArray_size(&array) == 100);
  CHECK(IntArray_get_unchecked(&array, 99) == 99);
  IntArray_finalize(&array);
  return 0;
}
