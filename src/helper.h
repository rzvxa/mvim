#ifndef HELPER_H
#define HELPER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void insert_substring(char*, char*, int);
char* substring(char*, int, int);
char* replace_word(const char*, const char*, const char*);

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

#endif
