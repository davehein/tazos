//******************************************************************************
// Copyright (c) 2014-2019 Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>

int linenum = 1;
int bytenum = 1;
char buffer1[1000];
char buffer2[1000];

void usage(void)
{
    printf("usage: cmp file1 file2\n");
    exit(0);
}

void ErrorExit(char *fname)
{
    printf("%s: No such file or directory\n", fname);
    exit(2);
}

void DiffExit(char *fname1, char *fname2)
{
    printf("%s %s differ: byte %d, line %d\n", fname1, fname2, bytenum, linenum);
    exit(1);
}

int main(int argc, char **argv)
{
    FILE *infile1;
    FILE *infile2;
    int i, num1, num2;

#ifdef __P2GCC__
    sd_mount(58, 61, 59, 60);
    chdir(argv[argc]);
#endif

    if (argc != 3) usage();

    if (!(infile1 = fopen(argv[1], "r")))
        ErrorExit(argv[1]);
    
    if (!(infile2 = fopen(argv[2], "r")))
        ErrorExit(argv[2]);

    while (1)
    {
        num1 = fread(buffer1, 1, 1000, infile1);
        num2 = fread(buffer2, 1, 1000, infile2);
        if (!num1 && !num2) break;
        for (i = 0; i < num1 && i < num2; i++)
        {
            if (buffer1[i] != buffer2[i])
            {
                DiffExit(argv[1], argv[2]);
                exit(1);
            }
            bytenum++;
            if (buffer1[i] == 10) linenum++;
        }
        if (num1 != num2) DiffExit(argv[1], argv[2]);
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
