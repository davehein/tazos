//******************************************************************************
// Copyright (c) 2011-2019, Dave Hein
//******************************************************************************
#include <stdint.h>
#include <dirent.h>

#define GLOB_START  0
#define GLOB_ACTIVE 1
#define GLOB_DONE   2

typedef struct GlobS {
  char state;
  char num;
  DIR *handle;
} GlobT;

char *globit(char *pat, GlobT *globptr, char *str);
int   globit_iswild(char *ptr);
void  globit_exit(GlobT *globptr);
