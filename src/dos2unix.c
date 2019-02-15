//******************************************************************************
// Copyright (c) 2011-2019, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buffer[200];

int main(int argc, char **argv)
{
  FILE *infile;
  FILE *outfile;
  int len;
  char *ptr;

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

  if (argc != 2)
  {
    printf("usage: dos2unix file\n");
    exit(1);
  }

  // Copy file to temporary file and remove CR
  infile = fopen(argv[1], "r");
  if (!infile)
  {
    printf("Could not open %s\n", argv[1]);
    exit(1);
  }

  outfile = fopen("_temp123.tmp", "w");
  if (!outfile)
  {
    fclose(infile);
    printf("Could not open temporary file\n");
    exit(1);
  }

  while (fgets(buffer, 197, infile))
  {
    len = strlen(buffer);
    ptr = buffer + len - 1;
    if (len && *ptr == 10)
    {
      len--;
      ptr--;
    }
    if (len && *ptr == 13)
    {
      len--;
      ptr--;
    }
    ptr[1] = 10;
    ptr[2] = 0;
    fputs(buffer, outfile);
  }

  fclose(infile);
  fclose(outfile);

  // Copy temporary file to file
  infile = fopen("_temp123.tmp", "r");
  if (!infile)
  {
    printf("Could not open temporary file\n");
    exit(1);
  }

  outfile = fopen(argv[1], "w");
  if (!outfile)
  {
    fclose(infile);
    printf("Could not open %s\n", argv[1]);
    exit(1);
  }

  while ((len = fread(buffer, 1, 200, infile)) > 0)
    fwrite(buffer, 1, len, outfile);

  fclose(infile);
  fclose(outfile);

  remove("_temp123.tmp");
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
