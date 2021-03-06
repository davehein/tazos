//******************************************************************************
// Copyright (c) 2011-2019, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void RemoveCRLF(char *str);

int main(int argc, char **argv)
{
  FILE *infile;
  int line, charval, maxlines;
  char buffer[100];

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

  if (argc != 2)
  {
    printf("usage: man application\n");
    exit(0);
  }
  else
  {
    strcpy(buffer, "/manpages/");
    strcat(buffer, argv[1]);
    strcat(buffer, ".txt");
    infile = fopen(buffer, "r");
    if (infile == 0)
    {
      printf("No manual entry for %s\n", argv[1]);
      exit(1);
    }
  }
  //if (!(maxlines = (getconfig >> 8) & 255))
  maxlines = 24;

  line = 0;
  while (fgets(buffer, 100, infile))
  {
    RemoveCRLF(buffer);
    printf("%s\n", buffer);
    if (++line >= maxlines - 1)
    {
      printf("<more>");
#ifdef __P2GCC__
      charval = getch();
#else
      charval = getchar();
#endif
      printf("\b\b\b\b\b\b      \b\b\b\b\b\b");
      if (charval == 'q' || charval == 3)
        break;
      else if (charval == 13)
        line--;
      else
        line = 0;
    }
  }
  fclose(infile);
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
