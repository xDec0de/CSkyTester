char	cst_toupper(char c)
{
	if (c >= 'a' && c <= 'z')
		c -= 32;
	return (c);
}

unsigned char	cst_utoupper(unsigned char c)
{
	if (c >= 'a' && c <= 'z')
		c -= 32;
	return (c);
}
