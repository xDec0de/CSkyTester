#include "cst.h"
#include "cst_example.h"
#include <string.h>

static const char *category = "String assertions";

// Assert str equals

TEST(category, "\"Hi\" == \"Hi\"") {
	ASSERT_STR_EQUALS("Hi", "Hi");
}

TEST(category, "\"Hi\" == \"Bye\"") {
	ASSERT_STR_EQUALS("Hi", "Bye");
}


// Assert str NOT equals

TEST(category, "\"Hi\" != \"Bye\"") {
	ASSERT_STR_NOT_EQUALS("Hi", "Bye");
}

TEST(category, "\"Hi\" != \"Hi\"") {
	ASSERT_STR_NOT_EQUALS("Hi", "Hi");
}

// Free variants

TEST(category, "\"Hi\" == \"Hi\" (Free)") {
	ASSERT_STR_EQUALS_FREE(strdup("Hi"), "Hi");
}

TEST(category, "\"Hi\" != \"Bye\" (Free)") {
	ASSERT_STR_NOT_EQUALS_FREE(strdup("Hi"), "Bye");
}
