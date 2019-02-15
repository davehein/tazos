//******************************************************************************
// Copyright (c) 2011-2019, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>

void usage(void);
void dumphex(FILE *infile);
void dumpoctal(FILE *infile);
void dumpascii(FILE *infile);

int main(int argc, char **argv)
{
  char *fname;
  FILE *infile;
  int mode;

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

  mode = 'o';
  if (argc < 2 || argc > 3)
    usage();
  if (argc == 3)
  {
    fname = argv[2];
    if (argv[1][0] != '-')
      usage();
    mode = argv[1][1];
    if (mode != 'o' && mode != 'x' && mode != 'a')
      usage();
  }
  else
    fname = argv[1];
  infile = fopen(fname, "r");
  if (infile == 0)
  {
    printf("File %s not found\n", fname);
    exit(1);
  }
  if (mode == 'x')
    dumphex(infile);
  else if (mode == 'o')
    dumpoctal(infile);
  else
    dumpascii(infile);
  fclose(infile);
  return 0;
}

void dumphex(FILE *infile)
{
  int num, i, j;
  unsigned char buf[16];

  j = 0;
  while ((num = fread(buf, 1, 16, infile)) > 0)
  {
    printf("%04x", j);
    j += 16;
    i = 0;
    while (i < num)
    {
      printf(" %02x", buf[i]);
      i++;
    }
    printf("\n");
  }
}

void dumpoctal(FILE *infile)
{
  int num, i, j;
  unsigned char buf[16];

  j = 0;
  while ((num = fread(buf, 1, 16, infile)) > 0)
  {
    printf("%06o", j);
    j += 16;
    i = 0;
    while (i < num)
    {
      printf(" %03o", buf[i]);
      i++;
    }
    printf("\n");
  }
}

void dumpascii(FILE *infile)
{
  int charval;
  int num, i, j;
  unsigned char buf[16];

  j = 0;
  while ((num = fread(buf, 1, 16, infile)) > 0)
  {
    printf("%04x", j);
    j += 16;
    i = 0;
    while (i < num)
    {
      charval = buf[i];
      if (charval >= 32 && charval <= 126)
        printf("  %c", charval);
      else if (charval == 9)
        printf(" \\t");
      else if (charval == 10)
        printf(" \\n");
      else if (charval == 13)
        printf(" \\r");
      else
        printf(" %02x", charval);
      i++;
    }
    printf("\n");
  }
}

void usage(void)
{
  printf("usage: od [flags] filename\n");
  printf("  -o Dump in octal (default)\n");
  printf("  -x Dump in hex\n");
  printf("  -a Dump in ASCII\n");
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
