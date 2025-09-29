#include "cst.h"
#include <unistd.h>

static const char *category = "Tests for built-in tools";

TEST(category, "Force SIGSEGV") {
	char *null_str = NULL;
	ASSERT_TRUE(null_str[0]);
}

TEST(category, "Force double free") {
	char	*ptr = malloc(sizeof(char *));

	free(ptr);
	free(ptr);
}

TEST(category, "Force memory leak (Shouldn't pass)") {
	char *ptr = malloc(42);
	ASSERT_NULL(NULL);
}

TEST(category, "Force timeout") {
	while (true)
		sleep(1);
}
