//******************************************************************************
// Copyright (c) 2012-2019 Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void usage(void)
{
  printf("usage: head [-n] file\n");
  exit(0);
}

int main(int argc, char **argv)
{
  int i;
  FILE *infile = 0;
  FILE *outfile;
  int numlines = 10;
  char *ptr = argv[1];
  char buffer[256];

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

  // Parse the command-line arguments
  if (argc < 2)
    infile = stdin;
  else if (argc > 3)
    usage();
  else if (*ptr == '-')
  {
    if (!isdigit(*++ptr))
      usage();
    sscanf(ptr, "%d", &numlines);
    if (numlines <= 0)
      usage();
    if (argc == 2)
      infile = stdin;
    else
      ptr = argv[2];
  }
  else if (argc == 2)
    ptr = argv[1];
  else
    usage();

  // Open the input file if not stdin
  if (!infile)
    infile = fopen(ptr, "r");
  if (infile == 0)
  {
    printf("Error opening %s\n", ptr);
    exit(0);
  }

  outfile = stdout;

  for (i = 0; i < numlines; i++)
  {
    if (fgets(buffer, 256, infile) <= 0)
      break;
    fputs(buffer, outfile);
  }

  if (infile != stdin)
    fclose(infile);

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
