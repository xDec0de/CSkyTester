#include "cst.h"
#include <unistd.h>

static const char *category = "Tests for built-in tools";

CST_BEFORE_ALL(NULL) {
	printf(CST_BLUE"This runs before ANY test is done"CST_RES"\n");
}

CST_AFTER_ALL(NULL) {
	printf(CST_BLUE"This runs after ALL tests are done"CST_RES"\n");
}

CST_BEFORE_ALL(category) {
	printf(CST_BLUE"This runs BEFORE ALL tests in the \"%s\" category"CST_RES"\n", category);
}

CST_BEFORE_EACH(category) {
	printf(CST_BLUE"This runs BEFORE EACH test in the \"%s\" category"CST_RES"\n", category);
}

CST_AFTER_ALL(category) {
	printf(CST_BLUE"This runs AFTER ALL tests in the \"%s\" category"CST_RES"\n", category);
}

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

TEST(category, "Force timeout", 1) {
	while (true)
		sleep(1);
}

