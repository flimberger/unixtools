/* p:	print input in chunks */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE		4096
#define PAGESIZE	22

static const char	*progname;

static FILE	*efopen(const char * restrict, const char * restrict);
static void	 print(FILE *fp, int pagesize);
static char	 ttyin();

int
main(int argc, char *argv[])
{
	int	 i, pagesize = PAGESIZE;
	FILE	*fp;

	progname = argv[0];
	if (argc > 1 && argv[1][0] == '-') {
		pagesize = atoi(&argv[1][1]);
		argc--;
		argv++;
	}
	if (argc == 1)
		print(stdin, pagesize);
	else
		for (i = 1; i < argc; i++) {
			fp = efopen(argv[i], "r");
			print(fp, pagesize);
			fclose(fp);
		}
	exit(0);
}

/* fopen file, die if can't */
FILE	*
efopen(const char * restrict file, const char * restrict mode)
{
	FILE	*fp;

	if ((fp = fopen(file, mode)) != NULL)
		return fp;
	fprintf(stderr, "%s: can't open file %s mode %s\n", progname, file,
	    mode);
	exit(1);
}

/* print fp in pagesize chunks */
void
print(FILE *fp, int pagesize)
{
	static int	lines = 0;
	char		buf[BUFSIZE];

	while (fgets(buf, sizeof(buf), fp) != NULL)
		if (++lines < pagesize)
			fputs(buf, stdout);
		else {
			buf[strlen(buf) - 1] = '\0';
			fputs(buf, stdout);
			fflush(stdout);
			ttyin();
			lines = 0;
		}
}

/* process response from /dev/tty */
char
ttyin()
{
	static FILE	*tty = NULL;
	char	buf[BUFSIZE];

	if (tty == NULL)
		tty = efopen("/dev/tty", "r");
	if (fgets(buf, BUFSIZE, tty) == NULL || buf[0] == 'q')
		exit(0);
	else
		return buf[0];
}
