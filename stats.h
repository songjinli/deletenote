#ifndef __STATS_H_
#define __STATS_H_

#include <stdio.h>
#include <stdbool.h>

#include "note.h"

bool stats_initialize(struct statistics* stats, char* rdfile, char* wrfile);
int  stats_note(struct statistics* stats);
int  stats_copy(struct statistics* stats);
void stats_print(struct statistics* stats, FILE* fp);
void stats_clean(struct statistics* stats);

#endif

