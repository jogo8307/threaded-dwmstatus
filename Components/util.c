#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
char* smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	ret = malloc(++len);
	if (ret == NULL) {
		perror("malloc");
		exit(1);
	}

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}

char * readfile(char *base, char *file)
{
	char *path, line[513];
	FILE *fd;

	memset(line, 0, sizeof(line));

	path = smprintf("%s/%s", base, file);
	//fprintf(stderr,"Path: %s", path);
	fd = fopen(path, "r");
	if (fd == NULL){
		fprintf(stderr,"Couldn't read file: %s \n",path);
		free(path);
		return NULL;
	}
	if (fgets(line, sizeof(line)-1, fd) == NULL){
		free(path);
		return NULL;
	}
	fclose(fd);
	free(path);

	return smprintf("%s", line);
}
