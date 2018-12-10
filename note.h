#ifndef __NOTE_H_
#define __NOTE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>


struct statistics
{
	char	filename[1024];

	FILE*	rdfp;
	FILE*	wrfp;
	
	int		count;
	int		blank;
	int		note;
	int		code;

	int		combine;
};

typedef int (*lpfn)(struct statistics*, char*, int, bool, int);
struct note_choice
{
	int		choice;
	lpfn	handle;
};

int check_note(char* buffer, int len, uint8_t* flag);

#endif
