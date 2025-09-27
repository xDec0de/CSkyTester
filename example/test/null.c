#include "cst_assertions.h"
#include "cst_example.h"

static const char *category = "NULL assertions";

// NOT NULL

TEST(category, "NULL == NOT NULL") {
	ASSERT_NOT_NULL(NULL);
}

TEST(category, "\"Hi\" == NOT NULL") {
	ASSERT_NOT_NULL("Hi");
}

// NULL

TEST(category, "\"Hi\" == NULL") {
	ASSERT_NULL("Hi");
}

TEST(category, "NULL == NULL") {
	ASSERT_NULL(NULL);
}
