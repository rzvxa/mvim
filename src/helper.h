#ifndef HELPER_H
#define HELPER_H

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

void insert_substring(char*, char*, int);
char* substring(char*, int, int);
char* replace_word(const char*, const char*, const char*);
int remove_directory(const char*);

void insert_substring(char *src, char *str, int pos)
{
	char *f, *e;
	int len;

	len = strlen(src);

	f = substring(src, 1, pos - 1);
	e = substring(src, pos, len - pos + 1);

	strcpy(src, "");
	strcat(src, f);
	free(f);
	strcat(src, str);
	strcat(src, e);
	free(e);
}

char *substring(char *string, int pos, int len)
{
	char *pointer;
	int c;

	pointer = (char *) malloc(len + 1);

	if(pointer == NULL)
		exit(EXIT_FAILURE);

	for(c = 0; c < len; ++c)
		*(pointer+c) = *((string + pos - 1) + c);

	*(pointer+c) = '\0';
	return pointer;
}

char *replace_word(const char* str, const char *search, const char *word)
{
	char *result;
	int i, cnt = 0;
	int slen = strlen(search);
	int wlen = strlen(word);

	// Counting the number of times old word
	// occur in the string
	for(i = 0; str[i] != '\0'; ++i)
	{
		if(strstr(&str[i], search) == &str[i])
		{
			cnt++;
			i += slen - 1;
		}
	}

	result = (char *)malloc(i + cnt * (wlen - slen) + 1);

	i = 0;
	while (*str)
	{
		if(strstr(str, search) ==str)
		{
			strcpy(&result[i], word);
			i += wlen;
			str += slen;
		}
		else
			result[i++] = *str++;
	}

	result[i] = '\0';
	return result;
}

int remove_directory(const char* path)
{
	DIR *d = opendir(path);
	size_t path_len = strlen(path);
	int r = -1;

	if (d)
	{
		struct dirent *p;

		r = 0;

		while(!r && (p = readdir(d)))
		{
			int r2 = -1;
			char *buf;
			size_t len;

			// Skip thee names "." and ".." as we don't want to
			// recurse on them.
			if(!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
				continue;

			len = path_len + strlen(p->d_name) + 2;
			buf = (char*)malloc(len);

			if(buf)
			{
				struct stat statbuf;

				snprintf(buf, len, "%s/%s", path, p->d_name);

				if(!stat(buf, &statbuf))
				{
					if(S_ISDIR(statbuf.st_mode))
						r2 = remove_directory(buf);
					else
						r2 = unlink(buf);
				}
				free(buf);
			}
			r = r2;
		}
		closedir(d);
	}
	if(!r)
		r =rmdir(path);
	return r;
}

#endif
