#include "cst_assertions.h"
#include "cst_example.h"

static const char *category = "Numeric assertions";

// Integers

TEST(category, "cst_intsum(40, 2) == 24") {
	ASSERT_INT_EQUALS(cst_intsum(40, 2), 24);
}

TEST(category, "cst_intsum(40, 2) == 42") {
	ASSERT_INT_EQUALS(cst_intsum(40, 2), 42);
}

// Unsigned integers

TEST(category, "cst_uintsum(40, 2) == 24") {
	ASSERT_UINT_EQUALS(cst_uintsum(40, 2), 24);
}

TEST(category, "cst_uintsum(40, 2) == 42") {
	ASSERT_UINT_EQUALS(cst_uintsum(40, 2), 42);
}
