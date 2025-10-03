#include "cst.h"

static const char *category = "Hooks";

CST_BEFORE_ALL(NULL) {
	printf(CST_BLUE"This runs before ANY test is done"CST_RES"\n");
}

CST_AFTER_ALL(NULL) {
	printf(CST_BLUE"This runs after ALL tests are done"CST_RES"\n");
}

CST_BEFORE_ALL(category) {
	printf(CST_BLUE"This runs BEFORE ALL tests in the \"hooks\" category"CST_RES"\n");
}

CST_AFTER_ALL(category) {
	printf(CST_BLUE"This runs AFTER ALL tests in the \"hooks\" category"CST_RES"\n");
}

TEST(category, "Example test") {
	ASSERT_TRUE(true);
}

TEST(category, "Another example test") {
	ASSERT_TRUE(true);
}
