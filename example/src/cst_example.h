#ifndef CST_EXAMPLE_H
# define CST_EXAMPLE_H

#include <stdbool.h>

/* cst_isnum.c */

bool			cst_isnum(char ch);

/* cst_mem.c */

void			cst_force_leak(void);
void			cst_force_double_free(void);

/* cst_sum.c */

int				cst_intsum(int a, int b);
int				cst_uintsum(unsigned int a, unsigned int b);

/* cst_toupper.c */

char			cst_toupper(char c);
unsigned char	cst_utoupper(unsigned char c);

#endif
