#include "cst.h"
#include "cst_example.h"

static const char *category = "Char assertions";

// Char

TEST(category, "cst_toupper('a') == 'B'") {
	ASSERT_CHAR_EQUALS(cst_toupper('a'), 'B');
}

TEST(category, "cst_toupper('a') == 'A'") {
	ASSERT_CHAR_EQUALS(cst_toupper('a'), 'A');
}

// Unsigned char

TEST(category, "cst_utoupper('a') == 'B'") {
	ASSERT_UCHAR_EQUALS(cst_utoupper((unsigned char) 'a'), (unsigned char) 'B');
}

TEST(category, "cst_utoupper('a') == 'A'") {
	ASSERT_UCHAR_EQUALS(cst_utoupper((unsigned char) 'a'), (unsigned char) 'A');
}
