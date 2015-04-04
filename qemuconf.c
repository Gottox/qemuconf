/*
 * qemuconf.c
 * Copyright (C) 2015 tox <tox@rootkit>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <ctype.h>

#define DROP(x, y) { for(y = i; i < len && x; i++); }

static int start();
static int addoptarg(char *arg, int len);
static int addopt(char *opt, int len);
static int compact(char *text, int i, int len);
static int parseconfig(char *text, int len);
static int loadconfig(char *path);
static int argify(char **value, int len);

static char *binary = "/usr/bin/echo";
static int verbose = 0;
char **cargv;
char **curopt = NULL;
char *cwd = ".";
int cargc = 0;
int maxargc = 0;

int
start() {
	execv(binary, cargv);
	return 1;
}

int
addoptarg(char *arg, int len) {
	arg[len] = '\0';
	if(curopt != NULL) {
		*curopt = arg;
		curopt = NULL;
		return 0;
	}
	curopt = NULL;
	cargv[cargc] = arg;
	if(++cargc == maxargc) {
		fputs("Too many args", stderr);
		return 1;
	}
	cargv[cargc] = NULL;

	return 0;
}

int
addopt(char *opt, int len) {
	char *optdup;
	if(strncmp(opt, "cwd", 3) == 0) {
		curopt = &cwd;
		return 0;
	}
	if(!(optdup = calloc(sizeof(char), len + 2))) {
		perror("malloc");
		return 1;
	}
	optdup[0] = '-';
	memcpy(optdup+1, opt, len);
	return addoptarg(optdup, len + 1);
}

int
compact(char *text, int i, int len) {
	int _i, indent, curindent, w=i++, first;

	DROP(isspace(text[i]) && text[i] != '\n', i);
	//if(text[i] != '\n') {
	//	fputs("Error: expected newline after ':' ", stderr);
	//	return 1;
	//}

	for(indent = -1, first = 1; i < len;) {
		curindent = 0;
		while(text[i] == '\n') {
			i++;
			first = 0;
			DROP(isspace(text[i]) && text[i] != '\n', _i);
			curindent = i - _i;
		}
		if(i == len)
			break;
		else if(text[i] == '#') {
			DROP(text[i] != '\n', i);
			continue;
		}
		else if(indent == -1 && !first) {
			indent = curindent;
			if(indent == 0) {
				fputs("Error: expected indention after ':'. ", stderr);
				return 1;
			}
		}
		else if(indent > curindent)
			break;
		DROP(isspace(text[i]), i);
		DROP(isalnum(text[i]), _i);
		memmove(&text[w], &text[_i], i - _i);
		w += i - _i;
		DROP(text[i] != '\n' && isspace(text[i]), i);
		if(text[i] == '=' && !first) {
			text[w++] = '=';
			i++;
			DROP(text[i] != '\n' && isspace(text[i]), i);
		}
		DROP(text[i] != '\n', _i);
		memmove(&text[w], &text[_i], i - _i);
		w += i - _i;
		text[w++] = ',';
	}
	text[w-1] = '\n';
	memset(&text[w], ' ', i - w);
	return 0;
}

int
parseconfig(char *text, int len) {
	int i = 0, _i, line, linestart=0;

	for(linestart = i = line = 0; i < len; line++, linestart = ++i) {
		DROP(isspace(text[i]), i);
		if(i >= len)
			break;

		if(text[i] == '#') {
			DROP(text[i] != '\n', i);
			continue;
		}

		if(text[i] == '.') {
			i++;
			DROP(isspace(text[i]), i);
			DROP(text[i] != '\n', _i);
			text[i] = '\0';
			if(loadconfig(&text[_i])) {
				fprintf(stderr, "Error at line %i ", line);
				return 1;
			}
			continue;
		}

		DROP(isalnum(text[i]), _i);

		addopt(&text[_i], i - _i);

		DROP(isspace(text[i]) && text[i] != '\n', i);
		if(text[i] == '\n')
			continue;
		else if(text[i] == '=')
			i++;
		else if(text[i] == ':') {
			if(compact(text, i, len)) {
				fprintf(stderr, "At line %i character %i. ", line, i - linestart);
				return 1;
			}
		}
		else {
			fprintf(stderr, "Expected '=' or ':' at line %i character %i. ", line, i - linestart);
			return 1;
		}
		DROP(isspace(text[i]) && text[i] != '\n', i);

		if(text[i] == '\n') {
			fprintf(stderr, "Value expected at line %i character %i. ", line, i - linestart);
			return 1;
		}

		DROP(text[i] != '\n', _i);

		addoptarg(&text[_i], i - _i);
	}
	return 0;
}

int
loadconfig(char *path) {
	int r = 0, len = 0;
	char *text = NULL, *wd;
	char oldwd[PATH_MAX+1] = { 0 };
	FILE *file;
	char *pathdup = strdup(path);

	if(!(file = fopen(path, "r"))) {
		perror(path);
		return 1;
	}

	if(!getcwd(oldwd, sizeof(oldwd))) {
		perror("getcwd");
		return EXIT_FAILURE;
	}

	wd = dirname(pathdup);
	if(chdir(wd)) {
		perror(wd);
		return 1;
	}

	do {
		len += r;
		text = realloc(text, sizeof(char) * (len + BUFSIZ));
	} while((r = fread(text, sizeof(char), (len + BUFSIZ), file)) > 0);

	if(ferror(file)) {
		perror(path);
		return 1;
	}

	if(parseconfig(text, strlen(text))) {
		fprintf(stderr, "At file '%s'\n", path);
		return 1;
	}

	chdir(oldwd);
	free(pathdup);

	return 0;
}

int main(int argc, char *argv[]) {
	int opt;

	while ((opt = getopt(argc, argv, "q:c:nvV")) != -1) {
		switch(opt) {
			case 'q':
				binary = optarg;
				break;
			case 'V':
				puts("qemud-" VERSION);
				return EXIT_SUCCESS;
			case 'v':
				verbose = 1;
				break;
usage:
			default:
				printf("Usage: %s [-q exec] [-n] [-V] [-v] <name>\n", argv[0]);
				return EXIT_FAILURE;
		}
	}
	if(optind != argc - 1)
		goto usage;

	if((maxargc = sysconf(_SC_ARG_MAX)) <= 0) {
		perror("sysconf");
		return EXIT_FAILURE;
	}
	if(!(cargv = calloc(sizeof(char *), maxargc))) {
		perror("calloc");
		return EXIT_FAILURE;
	}
	cargv[cargc++] = binary;

	if(loadconfig(argv[optind]))
		return EXIT_FAILURE;
	if(start())
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
