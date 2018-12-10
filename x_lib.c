#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>
#include "x_lib.h"

int x_dirorfile(char* s)
{
	struct stat st;

	lstat(s, &st);

	if(S_ISDIR(st.st_mode))
	{
		return 0;
	}
	else if(S_ISREG(st.st_mode))
	{
		return 1;
	}

	return -1;
}

int x_readdir(char* dir, callback lpfn, void* arg)
{
	DIR* dp;

	if((dp = opendir(dir)) == NULL)
	{
		return -1;
	}

	struct dirent* dt;
	while((dt = readdir(dp)) != NULL)
	{
		if(strncmp(dt->d_name, ".", 1) == 0)
			continue;

		if(strncmp(dt->d_name, "CVS", 3) == 0)
			continue;

		char s[1024];
		snprintf(s, 1024, "%s/%s", dir, dt->d_name);

		int result;
		if((result = x_dirorfile(s)) < 0)
		{
			continue;
		}

		if(lpfn(s, dt->d_name, result, arg) < 0)
		{
			continue;
		}
	}

	closedir(dp);

	return 0;
}

char* x_lastbyte(char* begin, int len, char ch, int* retlen)
{
	if(len <= 0)
		goto nofind;

	char* end = begin + len - 1;
	if(*end == ch)
		goto nofind;

	while(end >= begin)
	{
		if(*end == ch)
		{
			end++;
			*retlen = begin + len - end;

			return end;
		}
		end--;
	}

nofind:
	*retlen = 0;
	return NULL;
}

bool x_ispair(char* buffer, int len, char c)
{
	bool ispair = true;

	int i;
	//
	for(i = 0; i < len; i++)
	{
		if(buffer[i] == c)
		{
			ispair = !ispair;
		}
	}

	return ispair;
}




