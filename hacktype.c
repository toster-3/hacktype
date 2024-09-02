/* hacktype - hackertyper.com for the terminal */
/* see LICENSE file for copyright and license details. */

#include <curses.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

typedef struct {
	int automatic;
	int screensaver;
	double timestep;
} Config;

Config config = {
	.automatic = 0,
	.screensaver = 0,
	.timestep = 0.03,
};

/* from libsl git.suckless.org/libsl */
void die(const char* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}

	endwin();
	exit(1);
}


void printhelp(void) {
  die("Usage: hacktype [OPTION(S)] [FILE]\n\n"

         "hacktype is a fake coding simulator\n"
         "based on hackertyper.com\n\n"

         "Options:\n"
         "  -a, --automatic         automatic mode: types automatically\n"
         "  -s, --screensaver  screensaver mode: automatic and exits on any "
         "keypress\n"
         "  -t, --time         in automatic mode wait TIME secs between\n"
         "                       every keypress [default: 0.03]\n"
         "  -h, --help         show help and exit\n"
         "  -v, --version      show version and exit\n");
}

void parseargs(int argc, char *argv[])
{
	int c;
	int option_index = 0;
	static const struct option opts[] = {
		{"help",        no_argument,       0, 'h'},
		{"version",     no_argument,       0, 'v'},
		{"automatic",   no_argument,       0, 'a'},
		{"screensaver", no_argument,       0, 's'},
		{"time",        required_argument, 0, 't'},
		{0, 0, 0, 0}
	};

	while ((c = getopt_long(argc, argv, "hvast:", opts, &option_index)) != -1) {
		switch (c) {
		case 'h':
			printhelp();
			break;
		case 'v':
			fprintf(stderr, "hacktype-"VERSION"\n");
			exit(1);
		case 'a':
			config.automatic = 1;
			break;
		case 's':
			config.screensaver = 1;
			break;
		case 't':
			if (strtold(optarg, NULL) > 0)
				config.timestep = strtod(optarg, NULL);
			else {
				die("error: invalid step time: '%s'", optarg);
			}
			break;
		case '?':
			exit(1);
			break;
		default:
			break;
		}
	}
}

FILE *getdefaultfile()
{
	char *defaultfile;
	
	if ((defaultfile = getenv("HOME")) == NULL) {
		return NULL;
	}

	/* ~/.config/hacktype_default */
	strcat(defaultfile, CONFIG_DEFAULTFILE);

	return fopen(defaultfile, "r");
}

void initcurses(void)
{
	initscr();
	cbreak();
	noecho();
	scrollok(stdscr, TRUE);
	start_color();
}

void run(FILE *fp)
{
	char *s = (char *)malloc(sizeof(char) * 3);
	int y, maxy;

	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	attron(COLOR_PAIR(1));

	if (config.automatic == 1 || config.screensaver == 1) {
		nodelay(stdscr, TRUE);
		while (1) {
			fgets(s, 3, fp);
			printw("%s", s);

			y = getcury(stdscr);
			maxy = getmaxy(stdscr);
			if (y >= maxy) {
				scroll(stdscr);
			}

			if (feof(fp)) {
				clear();
				fseek(fp, 0, SEEK_SET);
			}
			refresh();
			usleep(config.timestep * 1000000);
			if (config.screensaver == 1 && getch() != ERR) {
				break;
			}
		}
	} else {
		while (getch()) {
			fgets(s, 3, fp);
			printw("%s", s);

			y = getcury(stdscr);
			maxy = getmaxy(stdscr);
			if (y >= maxy) {
				scroll(stdscr);
			}

			if (feof(fp)) {
				clear();
				fseek(fp, 0, SEEK_SET);
			}
			refresh();
		}
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;
	char *fname;

	parseargs(argc, argv);
	if (optind >= argc) {
		if ((fp = getdefaultfile()) == NULL) {
			die("Unable to open default file '%s' for reading:", getenv("HOME"));
		}
	}
	else if (!(fp = fopen(fname = argv[optind], "r"))) {
		die("Unable to open '%s' for reading:", fname);
		printf("%s\n", argv[optind]);
	}

	initcurses();
	run(fp);
	endwin();
	return 0;
}
