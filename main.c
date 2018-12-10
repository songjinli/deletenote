#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>

#include "x_lib.h"
#include "stats.h"

int lpfn_work(char* path, char* name, int type, void* arg)
{
	assert((type==0)||(type==1));

	//
	char* s = (char* )arg;
	char buff[1024];
	snprintf(buff, 1024, "%s/%s", s, name);

	//
	if(type == 0)
	{
		if(mkdir(buff, 0755) == -1)
		{
			printf("create directory[%s] error.\n", buff);
			return -1;
		}
		
		if(x_readdir(path, lpfn_work, buff) < 0)
		{
			printf("open directory[%s] error.\n", path);
		}
	}
	else if(type == 1)
	{
		struct statistics stats;
		if(!stats_initialize(&stats, path, buff))
		{
			printf("statstic initialize error.\n");
			return -1;
		}

		//
		int   len;
		char* str = x_lastbyte(name, strlen(name), '.', &len);

		if((len == 1) && ((strncmp(str, "c", 1) != 0) || (strncmp(str, "h", 1) != 0)))
		{
			stats_note(&stats);
			stats_print(&stats, stdout);
		}
		else
		{
			stats_copy(&stats);
		}
			
		//
		stats_clean(&stats);
	}
	else
	{
		assert(0);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("Usage: ./note arg [dir or path]\n");
		return -1;
	}

	//
	char* s	   = strdup(argv[1]);
	int result;

	if((result = x_dirorfile(s)) < 0)
	{
		printf("[%s] is not directory or file.\n", s);
		return -1;
	}

	//
	int   len;
	char* str = x_lastbyte(s, strlen(s), '/', &len);

	if(len <= 0)
	{
		str = s;
		len = strlen(s);
	}
	
	time_t now = time(NULL);	
	char   path[1024];
	
	snprintf(path, 1024, "./%lld_%.*s", now, len, str);

	//start 
	if(result == 0/*directory*/)
	{		
		if(mkdir(path, 0755) == -1)
		{
			printf("create folder[%s] error.\n", path);
			return -2;
		}

		//
		if(x_readdir(s, lpfn_work, path) < 0)
		{
			printf("open directory[%s] error.\n", s);
			return -3;
		}

	}
	else if(result == 1/*file*/)
	{
		struct statistics stats;

		if(!stats_initialize(&stats, s, path))
		{
			return -1;
		}

		stats_note(&stats);

		stats_print(&stats, stdout);
	}
	
	return 0;
}




