#include	<stdio.h>
#include	<stdlib.h>

int	open_log(FILE **out, char *lognm)
{
	if((*out = fopen(lognm, "a")) != NULL) 
		return(0);
	else
		return(1);
}

int	log_output(FILE *out, char *s)
{
	int		status;
	int		count;
	char	*entry;

	count = sizeof(s) + 10;
	entry = (char *)malloc(count);
	status = fprintf(out,"%s", s);
	fflush(out);
	free(entry);
	if(status == count)
		return(0);
	else
		return(1);
}
