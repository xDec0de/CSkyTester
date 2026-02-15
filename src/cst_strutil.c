#include <stdbool.h>
#include <string.h>

bool cst_str_equals(const char *s1, const char *s2)
{
	if (s1 == NULL && s2 == NULL)
		return (true);
	if ((s1 == NULL && s2 != NULL) || (s2 == NULL && s1 == NULL))
		return (false);
	return (strcmp(s1, s2) == 0);
}
