//******************************************************************************
// Copyright (c) 2011, 2012, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void usage(void);
void nofile(char *fname);

int getmod(char *path)
{
  struct stat path_stat;
  if (stat(path, &path_stat))
    return -1;
  return path_stat.st_mode;
}

int main(int argc, char **argv)
{
  char *ptr;
  char *fname;
  int value, setmask, setflag, bitpos;

  if (argc != 3)
    usage();

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

  ptr = argv[1];
  fname = argv[2];

  setmask = getmod(fname);

  if (setmask == -1)
    nofile(fname);

  if (*ptr != '+' && *ptr != '-')
    usage();

  setflag = 1;
  while (*ptr)
  {
    value = *ptr++;
    if (value == '+')
    {
      setflag = 1;
      continue;
    }
    else if (value == '-')
    {
      setflag = 0;
      continue;
    }
    else if (value == 'x')
      bitpos = S_IEXEC;
    else if (value == 'w')
      bitpos = S_IWRITE;
    else if (value == 'r')
      bitpos = S_IREAD;
    else
      usage();

    if (setflag)
      setmask |= bitpos;
    else
      setmask &= ~bitpos;
  }
  chmod(fname, setmask);

  return 0;
}

void nofile(char *fname)
{
  printf("Could not find %s\n", fname);
  exit(1);
}

void usage(void)
{
  printf("usage: chmod mode file\n");
  printf("  mode contains the characters +-rwx\n");
  exit(0);
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
