#include "cst_assertions.h"

static const char *category = "Tests for crash detection";

TEST(category, "Force SIGSEGV") {
	char *null_str = NULL;
	ASSERT_TRUE(null_str[0]);
}

TEST(category, "Force double free") {
	char	*ptr = malloc(sizeof(char *));

	free(ptr);
	free(ptr);
}
