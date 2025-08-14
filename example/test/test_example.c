#include "sky_assertions.h"
#include <stdbool.h>

bool	sky_isnum(char ch);

int	main(void)
{
	__CST_FAIL_TIP__ = "Test failed intentionally to test format";
	ASSERT_FALSE(sky_isnum('1'));
}
