#include "cst_assertions.h"
#include "cst_example.h"

int	main(void)
{
	CST_TEST_NAME = "NULL == NULL";
	ASSERT_NULL(NULL);
}
