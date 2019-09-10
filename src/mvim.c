#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef linux
#include <sys/types.h>
#include <sys/stat.h>
#endif

static char* MVIM_DIR;
static char* MVIM_DATA_BASE;

static inline int remove_char(const char *src,char *dest,char c)
{
    int removed=0;
    while (*src)
    {
        if (*src!=c)
	{
            *dest++=*src;
        }
	else
	{
            ++removed;
        }
        ++src;
    }
    *dest=0;
    return removed;
}

static inline void Initialize()
{
	char *home = getenv("HOME");
	if(home == NULL)
		exit(EXIT_FAILURE);
	const char mvim_dir[] = "/.mvim";
	MVIM_DIR = malloc(strlen(home) + strlen(mvim_dir) + 1);
	strcpy(MVIM_DIR, home);
	strcat(MVIM_DIR, mvim_dir);
	const char mvim_data_base[] = "/.database";
	MVIM_DATA_BASE = malloc(strlen(MVIM_DIR) + strlen(mvim_data_base) + 1);
	strcpy(MVIM_DATA_BASE, MVIM_DIR);
	strcat(MVIM_DATA_BASE, mvim_data_base);
#ifdef linux
	struct stat st = {0};
	if(stat(MVIM_DIR, &st) == -1)
		mkdir(MVIM_DIR, 0777);
#endif
}

static inline void add_to_database(const char *path, char const *name)
{
	FILE *file;

	file = fopen(path, "a");
	if(file == NULL)
	{
		printf("Could not create or open database file\n");
		exit(EXIT_FAILURE);
	}

	fprintf(file, "%s\n", name);

	fclose(file);
}

static inline int contains_in_db(const char* path, const char* entity)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(path, "r");
	if(fp == NULL)
		return -1;

	int ret = -2;

	while((read = getline(&line, &len, fp)) != -1)
	{
		remove_char(line, line, '\n');
		int cmp_res = strcmp(line, entity);
		printf("strcmp(%s, %s) = %d \n", line, entity, cmp_res);
		if(cmp_res == 0)
		{
			ret = 0;
			break;
		}
	}

	fclose(fp);
	if(line)
		free(line);
	return ret;
}

void list_configs(const char* path)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(path, "r");

	const char empty_or_not_found[] = "Empty\n";

	if(fp == NULL)
	{
		printf(empty_or_not_found);
		exit(EXIT_FAILURE);
	}
	ushort flag = 0;
	while((read = getline(&line, &len, fp)) != -1)
	{
		remove_char(line, line, '\n');
		printf("%s => %svimrc config & %svim directory\n", line, line, line);
		flag = 1;
	}

	fclose(fp);
	if(line)
		free(line);
	if(flag == 0) printf(empty_or_not_found);
}

int main(int argc, char *argv[])
{
	Initialize();
	char conf_name[1024];
	size_t optind = 1;
	enum { NONE, NEW } command = NONE;
	while(optind < argc)
	{
		char * current = argv[optind];
		++optind;
		if(command == NEW)
		{
			if(contains_in_db(MVIM_DATA_BASE, current) == 0)
			{
				printf("config called %s already exists\n", current);
				exit(EXIT_FAILURE);
			}

			char * new_dir_path;
			new_dir_path = malloc(strlen(MVIM_DIR) + strlen(current) + strlen("vim") + 1 );
			sprintf(new_dir_path, "%s/%svim", MVIM_DIR, current);
			mkdir(new_dir_path, 0700);
			printf("config directory created at %s\n", new_dir_path);
			free(new_dir_path);
			char * new_config_path;
			new_config_path = malloc(strlen(MVIM_DIR) + strlen(current) + strlen("vimrc") + 1);
			sprintf(new_config_path, "%s/%svimrc", MVIM_DIR, current);
			FILE * fp;
			fp = fopen(new_config_path, "w");
			fclose(fp);
			printf("config file created at %s\n", new_config_path);
			free(new_config_path);
			add_to_database(MVIM_DATA_BASE, current);
			printf("added new config to the database\n");
			printf("For launching vim using this config call mvim %s\n", current);
			exit(EXIT_SUCCESS);
		}
		strcpy(conf_name, current);
		if(strncmp(current, "--new", 6) == 0)
			command = NEW;
		else if(strncmp(current, "ls", 2) == 0)
		{
			list_configs(MVIM_DATA_BASE);
			exit(EXIT_SUCCESS);
		}
		else
			break;
	}

	int remaining_args = argc - optind;
	char cmd[1024];
	sprintf(cmd, "vim -u %s/%svimrc", MVIM_DIR, conf_name);
	if(remaining_args > 0)
	{
		char *vim_args[remaining_args];
		for(; optind < argc; ++optind)
		{
			strcat(cmd, " ");
			strcat(cmd, argv[optind]);
		}
	}

	printf("%s\nShould i run?(Y/n)\n", cmd);
	char do_run;
	scanf("%c", &do_run);

	if(do_run == '\n')
		do_run = 'y';

	if(do_run >= 'A' && do_run <= 'Z')
		do_run += 32;

	if(do_run == 'y')
		system(cmd);

	return 0;
}
