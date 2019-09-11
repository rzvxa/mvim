#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bsd/string.h>

#ifdef linux
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "preprocessor.h"

#define MVIM_DIR_NAME ".mvim"
#define MVIM_DB_NAME ".database"
#define MVIM_TEMP_NAME ".tmp"
#define MVIM_SHARE_NAME "share"

#define DB_VERSION 1

static char* HOME_DIR;
static char* MVIM_DIR;
static char* MVIM_TEMP;
static char* MVIM_DB;
static char* MVIM_SHARE_DIR;

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
	HOME_DIR = malloc(strlen(home) + 1);
	strcpy(HOME_DIR, home);
	if(home == NULL)
		exit(EXIT_FAILURE);
	const char mvim_dir[] = "/" MVIM_DIR_NAME;
	MVIM_DIR = malloc(strlen(home) + strlen(mvim_dir) + 1);
	strcpy(MVIM_DIR, home);
	strcat(MVIM_DIR, mvim_dir);
	const char mvim_data_base[] = "/" MVIM_DB_NAME;
	MVIM_DB = malloc(strlen(MVIM_DIR) + strlen(mvim_data_base) + 1);
	strcpy(MVIM_DB, MVIM_DIR);
	strcat(MVIM_DB, mvim_data_base);
	const char mvim_temp[] = "/" MVIM_TEMP_NAME;
	MVIM_TEMP = malloc(strlen(MVIM_DIR) + strlen(mvim_temp) + 1);
	strcpy(MVIM_TEMP, MVIM_DIR);
	strcat(MVIM_TEMP, mvim_temp);
	const char mvim_share[] = "/" MVIM_SHARE_NAME;
	MVIM_SHARE_DIR = malloc(strlen(MVIM_DIR) + strlen(mvim_share) + 1);
	strcpy(MVIM_SHARE_DIR, MVIM_DIR);
	strcat(MVIM_SHARE_DIR, mvim_share);
#ifdef linux
	struct stat st = {0};
	if(stat(MVIM_DIR, &st) == -1)
		mkdir(MVIM_DIR, 0777);
	if(stat(MVIM_TEMP, &st) == -1)
		mkdir(MVIM_TEMP, 0777);
#endif
}

static inline void add_to_database(const char *path, char const *name)
{
	// TODO improve is_new mechanic
	// TODO so it check existance of file without opening it again
	ushort is_new = 0;
	FILE *file;

	file = fopen(path, "r");
	if(file == NULL) // it don't exists
		is_new = 1;
	else
		fclose(file);

	file = fopen(path, "a");
	if(file == NULL)
	{
		printf("Could not create or open database file\n");
		exit(EXIT_FAILURE);
	}

	char db_buf[512];
	db_buf[0] = '\0';
	if(is_new)
		sprintf(db_buf, "%d\n", DB_VERSION);
	strcat(db_buf, name);

	fprintf(file, "%s\n", db_buf);

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

static inline void process_file(const char * conf, const char * relative_dir, char * save_path)
{
	FILE * fp;
	char buffer1[102400];
	char buffer2[102400];
	buffer1[0] = buffer2[0] = '\0';
	char chr;

	fp = fopen(conf, "r");
	if(fp == NULL)
	{
		printf("Could not read config file!\n");
		exit(EXIT_FAILURE);
	}

	while((chr = getc(fp)) != EOF)
		strcat(buffer1, &chr);
	fclose(fp);

	char *dir;

	dir = malloc(strlen(HOME_DIR) + strlen(relative_dir) + 2);
	sprintf(dir, "%s/%s", HOME_DIR, relative_dir);

	insert_mvim_configuration(buffer1, buffer2, relative_dir);
	constant_vars_t cvs;
	cvs.cdir = malloc(strlen(dir) + 1);
	strcpy(cvs.cdir, dir);
	handle_constant_vars(buffer2, buffer1, &cvs);
	handle_at_includes(buffer1, buffer2, MVIM_SHARE_DIR);

	fp = fopen(save_path, "w");
	if(fp == NULL)
	{
		printf("Could not read config file!");
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "%s", buffer2);
	fclose(fp);
}

int main(int argc, char *argv[])
{
	Initialize();
	char conf_name[1024];
	conf_name[0] = '\0';
	size_t optind = 1;
	enum { NONE, NEW } command = NONE;
	while(optind < argc)
	{
		char * current = argv[optind];
		++optind;
		if(command == NEW)
		{
			if(contains_in_db(MVIM_DB, current) == 0)
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
			char new_gconfig_path[1024];
			sprintf(new_gconfig_path, "%s/g%svimrc", MVIM_DIR, current);
			printf("%s\n", new_gconfig_path);
			fp = fopen(new_gconfig_path, "w");
			fclose(fp);
			printf(" gvim config file created at %s\n", new_gconfig_path);
			free(new_config_path);
			add_to_database(MVIM_DB, current);
			printf("added new config to the database\n");
			printf("For launching vim using this config call mvim %s\n", current);
			exit(EXIT_SUCCESS);
		}
		strcpy(conf_name, current);
		if(strncmp(current, "--new", 6) == 0)
			command = NEW;
		else if(strncmp(current, "ls", 2) == 0)
		{
			list_configs(MVIM_DB);
			exit(EXIT_SUCCESS);
		}
		else // we assume it is a config name
		{
			if(contains_in_db(MVIM_DB, conf_name) == 0)
				break;
			else
			{
				printf("Config called %s not exits!\n", conf_name);
				exit(EXIT_FAILURE);
			}
		}
	}

	int remaining_args = argc - optind;
	char cmd[1024];

	char conf_path[512];
	char pro_conf_path[512];
	char gconf_path[512];
	char pro_gconf_path[512];
	char dir_path[512];

	sprintf(conf_path, "%s/%svimrc", MVIM_DIR, conf_name);
	sprintf(gconf_path, "%s/g%svimrc", MVIM_DIR, conf_name);

	sprintf(pro_conf_path, "%s/%scompiled", MVIM_TEMP, conf_name);
	sprintf(pro_gconf_path, "%s/g%scompiled", MVIM_TEMP, conf_name);

	sprintf(dir_path, "%s/%svim", MVIM_DIR_NAME, conf_name);

	process_file(conf_path, dir_path, pro_conf_path);
	process_file(conf_path, dir_path, pro_gconf_path);


	if(conf_name[0] != '\0')
		sprintf(cmd, "vim -u %s -U %s", pro_conf_path, pro_gconf_path);
	else
		sprintf(cmd, "vim");

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
