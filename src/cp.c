//******************************************************************************
// Copyright (c) 2011-2019, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "globit.h"

char *GetFileName(char *path);
void  resolve_path(char *path, char *str);

static GlobT globdata;
static int count;
static char str[100];

int getmod(char *path)
{
  struct stat path_stat;
  if (stat(path, &path_stat))
    return -1;
  return path_stat.st_mode;
}

int main(int argc, char **argv)
{
  int argi, num;
  FILE *infile;
  FILE *outfile;
  char buf[100];
  char *path;
  int attrib;
  char *target;
  int attrib1;
  char *fname;

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

  if (argc < 3)
  {
    printf("usage: cp file1 [file2 ...] path\n");
    exit(6);
  }
  path = argv[argc-1];

  // Get the destination file attribute
  if (globit_iswild(path))
  {
    printf("%s contains wildcard characters\n", path);
    exit(1);
  }
  resolve_path(path, str);
  if (!strcmp(str, "/"))
    attrib = S_IFDIR | S_IWRITE | S_IREAD;
  else
    attrib = getmod(path);
  if (attrib == -1)
    attrib = S_IWRITE | S_IREAD;
  else if (!(attrib & S_IWRITE))
  {
    printf("%s is read only\n", path);
    exit(1);
  }
  if (!S_ISDIR(attrib) && (argc > 3 || globit_iswild(argv[1])))
  {
    printf("%s is not a directory\n", path);
    exit(2);
  }

  target = buf;
  for (argi = 1; argi < argc - 1; argi++)
  {
    memset(&globdata, 0, sizeof(globdata));
    while ((fname = globit(argv[argi], &globdata, str)))
    {
      if (count++ && !S_ISDIR(attrib))
      {
        printf("%s is not a directory\n", path);
        globit_exit(&globdata);
      }
      infile = fopen(fname, "r");
      if (!infile)
      {
        printf("Could not open input file %s\n", fname);
        globit_exit(&globdata);
      }
      attrib1 = getmod(fname);
      if (attrib1 == -1)
      {
        printf("Could not get attributes of input file %s\n", fname);
        globit_exit(&globdata);
      }
      if (S_ISDIR(attrib1))
      {
        printf("%s is a directory\n", fname);
        fclose(infile);
        continue;
      }
      strcpy(target, path);
      if (S_ISDIR(attrib))
      {
        strcat(target, "/");
        strcat(target, GetFileName(fname));
      }
      outfile = fopen(target, "w");
      if (!outfile)
      {
        printf("Could not open output file %s\n", target);
        fclose(infile);
        exit(5);
      }
      while ((num = fread(buf, 1, 100, infile)) > 0)
        fwrite(buf, 1, num, outfile);
      fclose(infile);
      fclose(outfile);
      chmod(target, attrib1);
    }
  }

  return 0;
}

char *GetFileName(char *path)
{
  int len = strlen(path);
  if (len <= 0)
    return path;
  path += len;

  while (len--)
  {
    if (*--path == '/')
      return path + 1;
  }

  return path;
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
