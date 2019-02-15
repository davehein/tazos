//******************************************************************************
// Copyright (c) 2011, 2012, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "globit.h"

int getmod(char *path)
{
  struct stat path_stat;
  if (stat(path, &path_stat))
    return -1;
  return path_stat.st_mode;
}

int main(int argc, char **argv)
{
  int attrib, argi;
  GlobT globdata;
  char str[100];
  char *fname;

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

  if (argc < 2)
  {
    printf("usage: rm file1 [file2 ...]\n");
    exit(1);
  }

  for (argi = 1; argi < argc; argi++)
  {
    memset(&globdata, 0, sizeof(globdata));
    while ((fname = globit(argv[argi], &globdata, str)))
    {
      attrib = getmod(fname);
      if (attrib == -1)
        printf("%s not found\n", fname);
      else if (S_ISDIR(attrib))
        printf("%s is a directory\n", fname);
      else if (!(attrib & S_IWRITE))
        printf("%s is write_protected", fname);
      else if (remove(fname) < 0)
        printf("Could not delete %s", fname);
    }
  }
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
