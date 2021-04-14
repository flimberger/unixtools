/* p:	print input in chunks */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE		4096
#define PAGESIZE	22

typedef void	(*Sighandler)(int);

static const char	*progname;

static FILE	*efopen(const char * restrict, const char * restrict);
static void	 print(FILE *fp, int pagesize);
static char	 ttyin();

int
main(int argc, char *argv[])
{
	int	 i, pagesize = PAGESIZE;
	FILE	*fp;
	char	*p;

	progname = argv[0];
	if ((p = getenv("PAGESIZE")) != NULL)
		pagesize = atoi(p);
	if (argc > 1 && argv[1][0] == '-') {
		pagesize = atoi(&argv[1][1]);
		argc--;
		argv++;
	}
	if (pagesize == 0)
		pagesize = PAGESIZE;
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

static void
xdup(int fd)
{
	int	s;

	s = dup(fd);
	if (s < 0) {
		fprintf(stderr, "dup: %s\n", strerror(errno));
		exit(1);
	}
}

static int
xsystem(const char *s)
{
	Sighandler	ihf, qhf;
	int		status, pid, w, tty;

	fflush(stdout);
	tty = open("/dev/tty", O_RDWR);
	if (tty == -1) {
		fprintf(stderr, "%s: cannot open /dev/tty: %s\n", progname, strerror(errno));
		return -1;
	}
	pid = fork();
	if ((pid = fork()) == 0) {
		close(0);	xdup(tty);
		close(1);	xdup(tty);
		close(2);	xdup(tty);
		close(tty);
		execlp("sh", "sh", "-c", s, NULL);
		exit(127);
	}
	close(tty);
	if (pid == -1)
		return -1;
	ihf = signal(SIGINT, SIG_IGN);
	qhf = signal(SIGQUIT, SIG_IGN);
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;
	signal(SIGQUIT, qhf);
	signal(SIGINT, ihf);

	return status;
}

/* process response from /dev/tty */
char
ttyin()
{
	static FILE	*tty = NULL;
	int		 res;
	char		 buf[BUFSIZE];

	if (tty == NULL)
		tty = efopen("/dev/tty", "r");
	for (;;) {
		if (fgets(buf, BUFSIZE, tty) == NULL || buf[0] == 'q')
			exit(0);
		else if (buf[0] == '!') {
			res = xsystem(buf + 1);
			if (res != 0)
				printf("(%d)!\n", res);
			else
				printf("!\n");
		} else
			return buf[0];
	}
}
