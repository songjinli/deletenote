#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "note.h"
#include "x_lib.h"

int  blank_note(struct statistics* stats, char* buffer, int offset, bool arg, int mask);
int single_note(struct statistics* stats, char* buffer, int offset, bool arg, int mask);
int  multi_note(struct statistics* stats, char* buffer, int offset, bool arg, int mask);
int   code_note(struct statistics* stats, char* buffer, int offset, bool arg, int mask);

struct note_choice choices[] = 
{
	{0,		blank_note},
	{1,		single_note},
	{2,		multi_note},
	{3,		code_note},
};

int blank_note(struct statistics* stats, char* buffer, int offset, bool arg, int mask)
{
	fputs(buffer, stats->wrfp);

	if(mask&0x3)
	{
		stats->note	  +=mask&0x1;
		stats->combine+=(mask&0x2)>>1;
	}
	else
	{
		stats->count++;
		stats->blank++;
	}
}

int code_note(struct statistics* stats, char* buffer, int offset, bool arg, int mask)
{
	fputs(buffer, stats->wrfp);

	if(mask&0x3)
	{
		stats->combine++;
	}
	else
	{
		stats->count++;
		stats->code ++;
	}
}

int single_note(struct statistics* stats, char* buffer, int offset, bool arg, int mask)
{
	bool front  = x_ispair(buffer, offset, '\"');
	bool behind = x_ispair(buffer+offset, strlen(buffer)-offset, '\"');
	if((!front)&&(!behind))
	{
		fputs(buffer, stats->wrfp);
		return 0;
	}

	if(!(mask&0x3)/*mask == 0*/)
	{
		stats->count++;
		
		if(arg)
		{
			buffer[offset]   = '\n';
			buffer[offset+1] = '\0'; 
			fputs(buffer, stats->wrfp);
	
			stats->combine++;
		}
		else
		{
			stats->note++;
		}
		return 0;
	}

	//mask != 0
	if(arg)
	{
		buffer[offset]   = '\n';
		buffer[offset+1] = '\0'; 
		fputs(buffer, stats->wrfp);
	
		stats->combine++;
	}
	else
	{
		stats->note	  +=mask&0x1;
		stats->combine+=(mask&0x2)>>1;
	}

	return 0;
}

int multi_note_tail(char* buffer)
{
	int offset;

	for(offset = 0; offset < strlen(buffer); offset++)
	{
		if((buffer[offset] == '*') && (buffer[offset+1] == '/'))
		{
			break;
		}
	}

	return offset;
}

int multi_note(struct statistics* stats, char* buffer, int offset, bool arg, int mask)
{
	if(!(mask&0x3))
	{
		stats->count++;
	}

	if(arg)
	{
		fwrite(buffer, offset, 1, stats->wrfp);
		mask=(0x1<<1);
	}
	else
	{
		if(mask == 0)
			mask = (0x1<<0);
	}

	//
	char* s   = buffer + offset + 2;

	do{
		s += multi_note_tail(s);
		if(s[0] != '\0')
		{
			goto find;
		}
		
		s = buffer;
		fgets(s, 1024, stats->rdfp);

		if(feof(stats->rdfp))
		{
			break;
		}

		mask=(0x1<<0);
		stats->note ++;
		stats->count++;

	}while(!feof(stats->rdfp));	

	return -1;

find:
	s += 2;
	
	uint8_t flag = 0;
	int		offset2 = check_note(s, strlen(s), &flag);

	int		type  =  flag&0x03;
	bool	arg2  = (flag&0x04) >> 2;

	if(choices[type].handle(stats, s, offset2, arg2, mask) == -1)
	{
		return -1;
	}

	return 0;
}

int trim_buf(char* buffer)
{
	char* p = buffer;

	while(*p == ' ' || *p == '\t')
		p++;

	return p - buffer;
}

int check_note(char* buffer, int len, uint8_t* flag)
{
	//
	int i = trim_buf(buffer); 

	//
	if(buffer[i] == '\n')
	{
		//blank line.
		*flag |= 0x00;

		return i;
	}

	//
	if(buffer[i] != '/')
	{
		*flag |= 0x04;
	}

	//
	for(; i < len; i++)
	{
		if(buffer[i] == '/')
		{
			if(buffer[i+1] == '/')
			{
				//single line note.
				*flag |= 0x01;
				return i;
			}
			else if(buffer[i+1] == '*')
			{
				//multi line note.
				*flag |= 0x02;
				return i;
			}
		}
	}

	//code line.
	*flag |= 0x03;
	return i;
}
