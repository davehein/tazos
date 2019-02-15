//******************************************************************************
// Copyright (c) 2014 Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 100000
#define MAX_LINES 30000

char *str1;
char *str2;
char buffer1[1000];
char buffer2[1000];
int global_flag = 0;

void RemoveCRLF(char *str)
{
    int len = strlen(str);

    while (len--)
    {
        if (str[len] != 10 && str[len] != 13) break;
        str[len] = 0;
    }
}

void Convert(char *buf1, char *buf2)
{
    int check = 1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    while (*buf1)
    {
        if (check)
        {
            if (!strncmp(buf1, str1, len1))
            {
                memcpy(buf2, str2, len2);
                buf2 += len2;
                buf1 += len1;
                check = global_flag;
                continue;
            }
        }
        *buf2++ = *buf1++;
    }
    *buf2 = 0;
}

char *FindChar(char *str, int val)
{
    while (*str)
    {
        if (*str == val) break;
        str++;
    }
    return str;
}

void usage(void)
{
    printf("usage: sed -h , or\n");
    printf("usage: sed s/expr/replace/[g] [FILE]...\n");
    exit(0);
}

int main(int argc, char **argv)
{
    int i;
    FILE *infile;
    int delim;
    char *cmdptr;

    // Check for help option or no parameters
    if (argc < 2 || !strcmp(argv[1], "-h"))
        usage();

#ifdef __P2GCC__
    sd_mount(58, 61, 59, 60);
    chdir(argv[argc]);
#endif

    // Check for s command
    if (argv[1][0] != 's')
    {
        printf("Must specify 's' command\n");
        exit(1);
    }

    // Check the delimiter
    delim = argv[1][1];
    if (!delim)
    {
        printf("Must specify a delimiter\n");
        exit(1);
    }

    // Extract str1 and str2
    str1 = &argv[1][2];
    str2 = FindChar(str1, delim);
    if (*str2) *str2++ = 0;
    cmdptr = FindChar(str2, delim);
    if (*str2) *cmdptr++ = 0;
    if (*cmdptr == 'g')
        global_flag = 1;
    else if (*cmdptr)
    {
        printf("Invalid command\n");
        exit(1);
    }

    if (argc == 2)
        i = 1;
    else
        i = 2;

    for (; i < argc; i++)
    {
        if (i == 1)
        {
            infile = stdin;
        }
        else
        {
            infile = fopen(argv[i], "r");
            if (!infile)
            {
                printf("%s: No such file or directory\n", argv[i]);
                exit(1);
            }
        }
        while (fgets(buffer1, 200, infile))
        {
            RemoveCRLF(buffer1);
            Convert(buffer1, buffer2);
            printf("%s\n", buffer2);
        }
        if (i > 1)
            fclose(infile);
    }

    //printf("global_flag = %d\n", global_flag);

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
