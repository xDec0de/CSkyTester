#include "sky_assertions.h"
#include <stdbool.h>

bool	sky_isnum(char ch);

int	main(void)
{
	ASSERT_TRUE(sky_isnum('0'));
	ASSERT_FALSE(sky_isnum('a'));
}
