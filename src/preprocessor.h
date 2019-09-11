#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

// #include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <pcre.h>
#include <string.h>
#include "helper.h"

int load_include_file(const char *path, char *buffer)
{
	FILE * fp;
	buffer[0] = '\0';
	char chr;

	fp = fopen(path, "r");
	if(fp == NULL)
		return -1;

	while((chr = getc(fp)) != EOF)
		strcat(buffer, &chr);
	fclose(fp);
	return 0;
}

void insert_mvim_configuration(char *source, char *dest, const char *conf_dir)
{
	const char temp[] = "let &rtp = substitute(&rtp, '\\.vim\\>', '%s', 'g')\n%s";
	sprintf(dest, temp, conf_dir, source);
}

#define CDIR "__cdir__"

struct constant_vars
{
	char *cdir;
};
typedef struct constant_vars constant_vars_t;

void handle_constant_vars(char *source, char *dest, const constant_vars_t *vars)
{
	char buffer[512];
	sprintf(buffer, "\"%s\"", vars->cdir);
	source = replace_word(source, CDIR, buffer);
	strcpy(dest, source);
}

void handle_at_includes(char *source, char *dest, const char * share_dir)
{
	pcre *reCompiled;
	pcre_extra *pcreExtra;
	int pcreExectRet;
	int subStrVec[30];
	const char *pcreErrorStr;
	int pcreErrorOffset;
	char *aStrRegex;
	char **aLineToMatch;
	const char *psubStrMatchStr;
	int j;

	aStrRegex = "^@.*$";
	reCompiled = pcre_compile(aStrRegex, PCRE_MULTILINE, &pcreErrorStr, &pcreErrorOffset, NULL);
	if(reCompiled == NULL)
	{
		printf("ERROR: Could not compile '%s': %s\n", aStrRegex, pcreErrorStr);
		exit(EXIT_FAILURE);
	}

	pcreExtra = pcre_study(reCompiled, 0, &pcreErrorStr);

	if(pcreErrorStr != NULL)
	{
		printf("ERROR: Could not study '%s': %s\n", aStrRegex, pcreErrorStr);
		exit(EXIT_FAILURE);
	}

	pcreExectRet = pcre_exec(
			reCompiled,
			pcreExtra,
			source,
			strlen(source),
			0,
			0,
			subStrVec,
			30);
	if(pcreExectRet < 0)
	{
		switch(pcreExectRet)
		{
			case PCRE_ERROR_NOMATCH      : printf("String did not match the pattern\n");        break;
      			case PCRE_ERROR_NULL         : printf("Something was null\n");                      break;
     			case PCRE_ERROR_BADOPTION    : printf("A bad option was passed\n");                 break;
     			case PCRE_ERROR_BADMAGIC     : printf("Magic number bad (compiled re corrupt?)\n"); break;
     			case PCRE_ERROR_UNKNOWN_NODE : printf("Something kooky in the compiled re\n");      break;
     			case PCRE_ERROR_NOMEMORY     : printf("Ran out of memory\n");                       break;
      			default                      : printf("Unknown error\n");
		}
	}
	else
	{
		printf("Result: We have a match!\n");

		if(pcreExectRet == 0)
		{
			printf("But too many substrings were found to fit in subStrVec!\n");
			pcreExectRet = 30 / 3;
		}

		char include_path_buf[1024];
		char include_content_buf[10240];
		int inc_res;
		const char * include_path_format = "%s/%s.vim";

		for(j = 0; j < pcreExectRet; ++j)
		{
			pcre_get_substring(source, subStrVec, pcreExectRet, j, &(psubStrMatchStr));
			printf("Processing include(%2d/%2d): (%2d,%2d): '%s'\n",
				 j,
				 pcreExectRet - 1,
				 subStrVec[j*2],
				 subStrVec[j*2+1],
				 psubStrMatchStr);
			include_content_buf[0] = '\0';
			sprintf(include_path_buf, include_path_format, share_dir, psubStrMatchStr + 1);
			inc_res = load_include_file(include_path_buf, include_content_buf);
			if(inc_res != 0)
			{
				printf("Could not load include file at '%s'\n", include_path_buf);
				exit(EXIT_FAILURE);
			}
			source = replace_word(source, psubStrMatchStr, include_content_buf);
		}
		pcre_free_substring(psubStrMatchStr);
	}
	printf("\n");
	strcpy(dest, source);
	pcre_free(reCompiled);

	if(pcreExtra != NULL)
	{
#ifdef PCRE_CONFIG_JIT
		pcre_free_study(pcreExtra);
#else
		pcre_free(pcreExtra);
#endif
	}
}

#endif
