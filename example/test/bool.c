#include "cst_assertions.h"

static const char *category = "Bool assertions";

// Force fail

TEST(category, "cst_isnum('a') == true") {
	ASSERT_TRUE(cst_isnum('a'));
}

TEST(category, "cst_isnum('1') == false") {
	ASSERT_FALSE(cst_isnum('1'));
}

// Pass

TEST(category, "cst_isnum('1') == true") {
	ASSERT_TRUE(cst_isnum('1'));
}

TEST(category, "cst_isnum('a') == false") {
	ASSERT_FALSE(cst_isnum('a'));
}
