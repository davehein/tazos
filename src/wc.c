//******************************************************************************
// Copyright (c) 2013-2019, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "globit.h"

int whitespace(int val)
{
  return (val <= 32);
}

int getmod(char *path)
{
  struct stat path_stat;
  if (stat(path, &path_stat))
    return -1;
  return path_stat.st_mode;
}

int main(int argc, char **argv)
{
  int attrib;
  int i, j, num, numchar, numline, numword, val;
  int wordflag, totchar, totline, totword, totfile;
  GlobT globdata;
  FILE *infile;
  char buffer[20];
  char str[100];
  char *fname;

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

  if (argc < 2)
  {
    printf("usage: wc [FILE]...\n");
    exit(1);
  }

  totfile = totchar = totline = totword = 0;
  for (i = 1; i < argc; i++)
  {
    memset(&globdata, 0, sizeof(globdata));
    while ((fname = globit(argv[i], &globdata, str)))
    {
      infile = fopen(fname, "r");
      if (infile == 0)
      {
        printf("Error opening %s\n", fname);
        exit(1);
      }
      attrib = getmod(fname);
      if (attrib < 0 || S_ISDIR(attrib)) // Check if it's a directory
      {
        printf("%s is a directory\n", fname);
        fclose(infile);
        continue;
      }

      // Read 20 characters at a time
      wordflag = numchar = numline = numword = 0;
      while ((num = fread(buffer, 1, 20, infile)) > 0)
      {
        numchar += num;
        for (j = 0; j < num; j++)
        {
          val = buffer[j];
          if (val == 10)
          {
            numline++;
            wordflag = 0;
          }
          else if (whitespace(val))
            wordflag = 0;
          else if (!wordflag)
          {
            wordflag = 1;
            numword++;
          }
        }
      }

      fclose(infile);
      printf("%6d %6d %7d %s\n", numline, numword, numchar, fname);
      totline += numline;
      totword += numword;
      totchar += numchar;
      totfile++;
    }
  }

  if (totfile > 1)
    printf("%6d %6d %7d total\n", totline, totword, totchar);

  return 0;
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
