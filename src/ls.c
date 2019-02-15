//******************************************************************************
// Copyright (c) 2011-2019, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include "globit.h"

void usage(void);
char *filetype(int attrib, char *path, char *fname);
void listdirectory(char *path);
void printfile(char *path, char *fname, int filesize, int attrib, int time, int date);
void printcompact(char *fname);
static void ConvertToLower(char *ptr);
static void listfile(char *path, int attrib, int filesize, int time, int date);
void resolve_path(char *path, char *str);

static int longlist;
static int argi;
static int total;
static int numfiles;
static int column = 1;
static int lastlen = 14;
static int hide = 1;
static GlobT globdata;
static char str[256];
static int columns;
static struct dirent *pdirent;

int getmod(char *path, int *filesize, int *time, int *date)
{
  struct stat path_stat;
  if (stat(path, &path_stat))
    return -1;
  *filesize = path_stat.st_size;
  *date = (path_stat.st_mtime >> 16) & 0xffff;
  *time = path_stat.st_mtime & 0xffff;
  return path_stat.st_mode;
}

int main(int argc, char **argv)
{
  int attrib, argj, filesize, time, date;
  char *path;

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

  total = 0;
  numfiles = 0;
  argi = 1;
  longlist = 0;
  columns = 5;

  // Parse the option flags
  for (argi = 1; argi < argc; argi++)
  {
    if (argv[argi][0] != '-')
      break;
    if (!strcmp(argv[argi], "-l"))
      longlist = 1;
    else if (!strcmp(argv[argi], "-e"))
      longlist = 2;
    else if (!strcmp(argv[argi], "-a"))
      hide = 0;
    else
      usage();
  }

  // List current directory if nothing specified
  if (argi >= argc)
  {
    listdirectory(".");
    exit(0);
  }
 
  // List a single item
  else if (argi == argc - 1 && !globit_iswild(argv[argi]))
  {
    path = argv[argi];
    resolve_path(path, str);
    if (!strcmp(str, "/"))
      attrib = S_IFDIR | S_IREAD | S_IWRITE;
    else
      attrib = getmod(path, &filesize, &time, &date);
    if (attrib == -1)
    {
      printf("%s does not exist\n", path);
      exit(1);
    }
    else if (!S_ISDIR(attrib))
      listfile(path, attrib, filesize, time, date);
    else
      listdirectory(path);
    exit(0);
  }
 
  // List multiple items
  else
  {
    // list files
    for (argj = argi; argi < argc; argi++)
    {
      memset(&globdata, 0, sizeof(globdata));
      while ((path = globit(argv[argi], &globdata, str)))
      {
        //printf1(string("<%s>"), path)
        attrib = getmod(path, &filesize, &time, &date);
        if (attrib == -1)
          printf("%s does not exist\n", path);
        else if (!S_ISDIR(attrib))
          listfile(path, attrib, filesize, time, date);
      }
    }
    if (column != 1)
    {
      printf("\n");
      column = 1;
      lastlen = 14;
    }
    printf("\n");

    // list directories
    for (argi = argj; argi < argc; argi++)
    {
      memset(&globdata, 0, sizeof(globdata));
      while ((path = globit(argv[argi], &globdata, str)))
      {
        //printf1(string("<%s>"), path)
        attrib = getmod(path, &filesize, &time, &date);
        if (attrib != -1 && S_ISDIR(attrib))
        {
          printf("%s:\n", path);
          listdirectory(path);
          printf("\n");
        }
      }
    }
    if (column != 1)
      printf("\n");
  }
  return 0;
}

static void ConvertToLower(char *ptr)
{
  int val;
  while ((val = *ptr))
  {
    if ((val >= 'A') && (val <= 'Z'))
      *ptr = val + 32;
    ptr++;
  }
}

static void listfile(char *path, int attrib, int filesize, int time, int date)
{
  int result = 0;
#if 0
  FILE *infile;
  
  infile = fopen(path, "r");
  if (!infile)
  {
    printf("Could not open file %s\n", path);
    return;
  }
#endif
  printfile((char *)&result, path, filesize, attrib, time, date);
}

void listdirectory(char *path)
{
  DIR *handle = opendir(path); 
  int filesize, attrib, time, date;
  char path1[100];
  int len;

  if (!handle)
  {
    printf("Could not open directory %s\n", path);
    return;
  }

  while ((pdirent = readdir(handle)))
  {
    if (hide && (pdirent->d_name[0] == '.' || pdirent->d_name[0] == '_'))
      continue;
    if (!longlist)
    {
        ConvertToLower(pdirent->d_name);
        printcompact(pdirent->d_name);
        continue;
    }
    strcpy(path1, path);
    len = strlen(path1);
    if (len && path1[len-1] != '/')
      strcat(path1, "/");
    strcat(path1, pdirent->d_name);
    attrib = getmod(path1, &filesize, &time, &date);
    total += filesize;
    printfile(path, pdirent->d_name, filesize, attrib, time, date);
  }
  if (longlist)
    printf("%6d files, %d bytes\n", numfiles, total);
  else if (column != 1)
  {
    printf("\n");
    column = 1;
    lastlen = 14;
  }
  closedir(handle);
}

void printfile(char *path, char *fname, int filesize, int attrib, int time, int date)
{
  char astr[8];
  char *aptr;
  char *ptr;

  aptr = astr;
  strcpy(aptr, "drwx");
  if (!S_ISDIR(attrib))
    aptr[0] = '-';
  if (!(attrib & S_IREAD))
    aptr[1] = '-';
  if (!(attrib & S_IWRITE))
    aptr[2] = '-';
  if (!(attrib & S_IEXEC))
    aptr[3] = '-';
  ConvertToLower(fname);
  if (longlist == 1 || longlist == 2)
  {
    //printf("%s %6d %s\n", astr, filesize, fname);
    printf("%s %6d ", astr, filesize);
    printf("%2.2d/%2.2d/%2.2d ", (date>>9)+1980, (date >> 5)&15, date&31);
    printf("%2.2d:%2.2d:%2.2d ", time >> 11, (time >> 5)&63, (time&31)<<1);
    if (longlist == 2)
    {
      ptr = filetype(attrib, path, fname);
      printf("%s ", ptr);
    }
    printf("%s\n", fname);
  }
  else
    printcompact(fname);
    //printf("%s\n", fname);
  numfiles++;
}

char *filetype(int attrib, char *path, char *fname)
{
  //int temp;
  int len;
  FILE *infile;
  int ibuf[20];
  char *typestr = (char *)ibuf;
  //char typestr[80];

  if (S_ISDIR(attrib))
    return "Directory";

  if ((len = strlen(path)))
  {
    strcpy(typestr, path);
    if (typestr[len-1] != '/')
    {
      typestr[len++] = '/';
      typestr[len] = 0;
    }
    strcat(typestr, fname);
  }
  else
    strcpy(typestr, fname);

  infile = fopen(typestr, "r");

  if (!infile)
    return "         ";

  memset(typestr, 0, 32);
  fread(typestr, 1, 32, infile);
  fclose(infile);

#if 0
  temp = ((byte[@typestr][4] & $bf) == $22)

  if temp and (not strncmp(@typestr, string("SPIN"), 4))
    return string("Spin App ")

  if temp and (not strncmp(@typestr, string("CAPP"), 4))
    return string("C App    ")

  if temp and (not strncmp(@typestr, string("SPRO"), 4))
    return string("Spin Prog")

  if temp and (not strncmp(@typestr, string("CPRO"), 4))
    return string("C Program")

  if typestr => 80_000_000 and typestr =< 120_000_000
    return string("SAProgram")

  ifnot strncmp(@typestr, string("#shell"), 6)
    return string("Script   ")
#endif
  if (!strncmp(typestr, "#shell", 6))
    return "Script   ";
  else if (ibuf[5] == 80000000 && ibuf[6] == 19146744 && ibuf[7] == 115200)
    return "C Program";

  while (*fname)
  {
    if (*fname == '.')
      break;
    fname++;
  }

  if (!strcmp(fname, ".txt"))
    return "Text file";

  if (!strcmp(fname, ".spn") || !strcmp(fname, ".spi"))
    return "Spin src ";

  if (!strcmp(fname, ".spa"))
    return "Spasm src";

  if (!strcmp(fname, ".c"))
    return "C source ";

  if (!strcmp(fname, ".fth"))
    return "Forth src";

  return "         ";
}

void printcompact(char *fname)
{
  int i;
  //int len;
  //char buffer[16];

  if (14 > lastlen)
  {
    i = 14 - lastlen;
    while (i-- > 0)
      putchar(' ');
  }
  else if (column != 1)
    putchar(' ');

  if (column >= columns)
  {
    printf("%s\n", fname);
    column = 1;
    lastlen = 14;
  }

  else
  {
    printf("%s", fname);
    column++;
    lastlen = strlen(fname);
  }
}

void usage(void)
{
  printf("ls [-l] [-a] [-e] [file list]\n");
  exit(2);
}

/*
+-----------------------------------------------------------------------------+
|                       TERMS OF USE: MIT License                             |
+-----------------------------------------------------------------------------+
|Permission is hereby granted, free of charge, to any person obtaining a copy |
|of this software and associated documentation files (the "Software"), to deal|
|in the Software without restriction, including without limitation the rights |
|to use, copy, modify, merge, publish, distribute, sublicense, and/or sell    |
|copies of the Software, and to permit persons to whom the Software is        |
|furnished to do so, subject to the following conditions:                     |
|                                                                             |
|The above copyright notice and this permission notice shall be included in   |
|all copies or substantial portions of the Software.                          |
|                                                                             |
|THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   |
|IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     |
|FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  |
|AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       |
|LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,|
|OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE|
|SOFTWARE.                                                                    |
+-----------------------------------------------------------------------------+
*/
