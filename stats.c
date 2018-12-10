#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "stats.h"
#include "note.h"

extern struct note_choice choices[];

bool stats_initialize(struct statistics* stats, char* rdfile, char* wrfile)
{
	//filename
	snprintf(stats->filename, sizeof(stats->filename), "%s", rdfile);

	//open file waiting for reading
	if((stats->rdfp = fopen(rdfile, "r")) == NULL)
	{
		printf("open file[%s] error: %s\n", stats->filename, strerror(errno));

		return false;
	}

	//
	if((stats->wrfp = fopen(wrfile, "w")) == NULL)
	{
		printf("open file[%s] error: %s\n", stats->filename, strerror(errno));

		return false;
	}

	//
	stats->count   = 0;
	stats->blank   = 0;
	stats->note    = 0;
	stats->code	   = 0;
	stats->combine = 0;

	return true;
}

int stats_note(struct statistics* stats)
{
	FILE* fp = stats->rdfp;

	while(!feof(fp))
	{
		char buffer[1024];
		fgets(buffer, sizeof(buffer), fp);

		if(feof(fp))
		{
			break;
		}

		//
		uint8_t flag = 0;
		int		offset = check_note(buffer, strlen(buffer), &flag);

		int		type = flag&0x03;
		bool	arg  = (flag&0x04) >> 2;

		if(choices[type].handle(stats, buffer, offset, arg, 0) == -1)
		{
			break;
		}
	}

	return 0;
}

int stats_copy(struct statistics* stats)
{
	char buf[1024];
	int  num;

	while(1)
	{
		if((num = fread(buf, 1, 1024, stats->rdfp)) != 1024)
		{
			if(!feof(stats->rdfp))
			{
				printf("copy file[%s] error.\n", stats->filename);
				return -1;
			}
			else
			{
				fwrite(buf, 1, num, stats->wrfp);
				break;
			}
		}
		fwrite(buf, 1, 1024, stats->wrfp);
	}

	return 0;
}

void stats_print(struct statistics* stats, FILE* fp)
{
	fprintf(fp, "file :     %s\n"
				"count:		%d\n"
			    "blank:		%d\n"
				"note :		%d\n"
				"code :		%d\n"
				"combine:	%d\n",stats->filename, stats->count, stats->blank, stats->note, stats->code, stats->combine);
}

void stats_clean(struct statistics* stats)
{
	fclose(stats->rdfp);
	fclose(stats->wrfp);
}

