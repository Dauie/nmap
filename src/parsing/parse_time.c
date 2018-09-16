#include "../../incl/parser.h"
/*
** TODO:  Eventually we want to be able to parse ms, s, m, h, and d, but
** TODO:   for now, everything should be entered in ms
*/
int			parse_time(uint32_t *opt_time, char *input)
{
	long	time;

	if ((time = atoi(input)) < 0)
		return (FAILURE); /* TODO hermese_error() */
	/* TODO bounds check for ms */
	*opt_time = (uint32_t)time;
	return (SUCCESS);
}
