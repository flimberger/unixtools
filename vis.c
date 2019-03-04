/* vis:	make funny characters visible */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static void	vis(FILE *fp, int strip);

int
main(int argc, char *argv[])
{
	FILE	*fp;
	int	 i, strip = 0;

	while (argc > 1 && argv[1][0] == '-') {
		switch (argv[1][1]) {
		case 's':	/* -s: strip funny chars */
			strip = 1;
			break;
		default:
			fprintf(stderr, "%s: unknown arg %s\n", argv[0],
			    argv[1]);
			exit(EXIT_FAILURE);
		}
		argc--;
		argv++;
	}
	if (argc == 1)
		vis(stdin, strip);
	else
		for (i = 1; i < argc; i++)
			if ((fp = fopen(argv[1], "r")) == NULL) {
				fprintf(stderr, "%s: can't open %s\n", argv[0],
				    argv[i]);
				exit(1);
			} else {
				vis(fp, strip);
				fclose(fp);
			}
	exit(EXIT_SUCCESS);
}

/* make chars visible in FILE *fp */
void
vis(FILE *fp, int strip)
{
	int	c;

	while ((c = getc(fp)) != EOF)
		if (isascii(c) && (isprint(c) || c == '\n' || c == '\t' ||
		    c == ' '))
			putchar(c);
		else if (!strip)
			printf("\\%03o", c);
}
