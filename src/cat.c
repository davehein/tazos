//******************************************************************************
// Copyright (c) 2011-2019, Dave Hein
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
  int i, value, redirout = 0, attrib;
  FILE *infile;
  char buffer[20];
  char *fname;
  GlobT globdata;
  char str[256];

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

  //redirout = (long[stdout][c#stream_type] == c#stream_fileio)
  for (i = 1; i < argc; i++)
  {
    // Check for stdin
    if (argc < 2 || !strcmp(argv[i], "-"))
    {
      infile = stdin;
      // Read one character at a time if stdin
      while ((value = fgetc(infile)) >= 0)
      {
        if (value == 0)
        {
          printf("\n");
          if (redirout)
            fprintf(stderr, "\n");
        }
        else if (value == 4)
          break;
        else
        {
          printf("%c", value);
          if (redirout)
            fprintf(stderr, "%c", value);
        }
      }
    }

    // Open a file
    else
    {
      memset(&globdata, 0, sizeof(globdata));
      while ((fname = globit(argv[i], &globdata, str)))
      {
        attrib = getmod(fname);
        if (attrib == -1)
        {
            printf("%s doesn't exist\n", fname);
            continue;
        }
        if (S_ISDIR(attrib))
        {
          printf("%s is a directory\n", fname);
          continue;
        }
        infile = fopen(fname, "r");
        if (infile == 0)
        {
          printf("Error opening %s\n", fname);
          continue;
        }

        // Read 20 characters at a time if it's a file
        while ((value = fread(buffer, 1, 20, infile)) > 0)
          fwrite(buffer, 1, value, stdout);

        // Close the file if it's not stdin
        fclose(infile);
      }
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
