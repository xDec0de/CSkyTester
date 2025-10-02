#include "cst.h"

static const char *category = "Runnables";

CST_BEFORE_ALL(NULL) {
	printf(CST_BLUE"This runs before ANY test is done"CST_RES"\n");
}

CST_BEFORE_ALL(category) {
	printf(CST_BLUE"This runs BEFORE ALL runnables"CST_RES"\n");
}

TEST(category, "Example test") {
	ASSERT_TRUE(true);
}