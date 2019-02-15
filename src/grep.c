//******************************************************************************
// Copyright (c) 2011-2019, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globit.h"
#include <sys/stat.h>

int FindString(char *str1, char *str2);
void RemoveCRLF(char *str);
int IsRegularFile(char *path);

static char buffer[256];

int main(int argc, char **argv)
{
  int i;
  GlobT globdata;
  FILE *infile;
  char str[256];
  char *ptr;

  if (argc < 3)
  {
    printf("usage: grep string files...\n");
    exit(1);
  }

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

  for (i = 2; i < argc; i++)
  {
    memset(&globdata, 0, sizeof(globdata));
    while ((ptr = globit(argv[i], &globdata, str)))
    {
      if (!IsRegularFile(ptr))
        continue;
      infile = fopen(ptr, "r");
      if (!infile)
        continue;
      while (fgets(buffer, 199, infile))
      {
        RemoveCRLF(buffer);
        if (FindString(buffer, argv[1]))
        {
          if (argc == 3 && !globit_iswild(argv[i]))
            printf("%s\n", buffer);
          else
          {
            printf("%s:", ptr);
            puts(buffer);
          }
        }
      }
      fclose(infile);
    }
  }
  return 0;
}

int FindString(char *str1, char *str2)
{
  int len = strlen(str2);
  while (*str1)
  {
    if (!strncmp(str1, str2, len))
      return 1;
    str1++;
  }
  return 0;
}

void RemoveCRLF(char *str)
{
  int len = strlen(str);
  str += len - 1;
  while (len)
  {
    if (*str != 10 && *str != 13)
      break;
    str--;
    len--;
  }
  str[1] = 0;
}

int IsRegularFile(char *path)
{
  struct stat path_stat;
  if (stat(path, &path_stat))
    return 0;
  return S_ISREG(path_stat.st_mode);
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
