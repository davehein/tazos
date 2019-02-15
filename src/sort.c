//******************************************************************************
// Copyright (c) 2014-2019 Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE  20000
#define MAX_LINES 5000

static char *bufptr;
static int reverse = 0;
static int numlines = 0;
static char *lineptr[MAX_LINES];

void RemoveCRLF(char *str)
{
    int len = strlen(str);

    while (len--)
    {
        if (str[len] != 10 && str[len] != 13) break;
        str[len] = 0;
    }
}

void sort(char **str, int num)
{
    int i, j;
    char *temp;
    int val;

    for (i = 0; i < num - 1; i++)
    {
        for (j = i+1; j < num; j++)
        {
            val = strcmp(str[i], str[j]);
            if (reverse) val = -val;
            if (val > 0)
            {
                temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

void ReadFile(FILE *infile)
{
    while (fgets(bufptr, 200, infile))
    {
        RemoveCRLF(bufptr);
        lineptr[numlines++] = bufptr;
        bufptr += strlen(bufptr) + 1;
    }
}

void usage(void)
{
    printf("usage: sort [-r] [-h] [FILE]...\n");
    exit(0);
}

int main(int argc, char **argv)
{
    int i;
    FILE *infile;
    int nfiles = 0;

#ifdef __P2GCC__
    sd_mount(58, 61, 59, 60);
    chdir(argv[argc]);
#endif

    bufptr = malloc(BUF_SIZE);

    if (!bufptr)
    {
        printf("Could not allocate file buffer\n");
        exit(1);
    }

    // Process command line parameters
    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (!strcmp(argv[i], "-r"))
                reverse = 1;
            else if (!strcmp(argv[i], "-h"))
                usage();
            else
            {
                printf("Invalid option - %s\n", &argv[i][1]);
                exit(1);
            }
        }
        else
            nfiles++;
    }

    if (nfiles == 0)
    {
      ReadFile(stdin);
    }
    else
    {
        for (i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-') continue;
            infile = fopen(argv[i], "r");
            if (!infile)
            {
                printf("%s: No such file or directory\n", argv[i]);
                exit(1);
            }
            ReadFile(infile);
            fclose(infile);
        }

    }

    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-r"))
            reverse = 1;
        else
            nfiles++;
    }

    sort(lineptr, numlines);

    for (i = 0; i < numlines; i++)
        printf("%s\n", lineptr[i]);

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
