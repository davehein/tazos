//******************************************************************************
// Copyright (c) 2011-2019 Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int   main(int argc, char **argv);
void  RemoveCRLF(char *str);
void  InsertLine(char *str, int linenum);
void  DeleteLine(int linenum);
char *GetLineNumber(char *str, int linenum, int *num);
int   ParseCommand(char *str, int linenum, int *first, int *last, int *cmd);
void  help(void);

static int numline = 0;
static char *bigbuf;
static char *lineptr[300];

int main(int argc, char **argv)
{
    FILE *infile;
    char *bigptr;
    int numchar, linenum;
    char instr[100];
    int i, first, last, cmd, changed;

#ifdef __P2GCC__
    sd_mount(58, 61, 59, 60);
    chdir(argv[argc]);
#endif

    if (argc != 2)
    {
        printf("usage: ted file\n");
        exit(1);
    }
    bigbuf = malloc(12000);
    numchar = 0;
    changed = 0;
    infile = fopen(argv[1], "r");
    bigptr = bigbuf;
    if (infile)
    {
        while (fgets(bigptr, 100, infile))
        {
            RemoveCRLF(bigptr);
            lineptr[numline++] = bigptr;
            numchar += strlen(bigptr);
            bigptr += strlen(bigptr) + 1;
        }
        fclose(infile);
    }
    else
    {
        numchar = 0;
    }
    printf("%d\n", numchar);
    linenum = numline - 1;
    while (1)
    {
        gets(instr);
        if (!ParseCommand(instr, linenum, &first, &last, &cmd))
        {
            printf("?\n");
            continue;
        }
        if (cmd == 0)
        {
            if (last >= 0)
            {
                linenum = last;
            }
            else if (first >= 0)
            {
                linenum = first;
            }
            printf("%s\n", lineptr[linenum]);
        }
        else if (cmd == 'Q')
        {
            break;
        }
        else if (cmd == 'q')
        {
            if (changed == 0)
            {
                break;
            }
            changed = 0;
            printf("?\n");
        }
        else if (cmd == 'w')
        {
            numchar = 0;
            infile = fopen(argv[1], "w");
            i = 0;
            while (i < numline)
            {
                numchar += strlen(lineptr[i]);
                fprintf(infile, "%s\n", lineptr[i]);
                i++;
            }
            fclose(infile);
            printf("%d\n", numchar);
            changed = 0;
        }
        else if (cmd == 'd')
        {
            DeleteLine(linenum);
            changed = 1;
        }
        else if (cmd == 'i' || *instr == 'a')
        {
            if (cmd == 'a' || linenum < 0)
            {
                linenum++;
            }
            while (1)
            {
                gets(bigptr);
                RemoveCRLF(bigptr);
                if (*bigptr == '.' && bigptr[1] == 0)
                {
                    break;
                }
                InsertLine(bigptr, linenum ++);
                bigptr += strlen(bigptr) + 1;
            }
            linenum--;
            changed = 1;
        }
        else if (cmd == 'j')
        {
            strcpy(bigptr, lineptr[linenum]);
            strcat(bigptr, lineptr[linenum + 1]);
            lineptr[linenum] = bigptr;
            bigptr += strlen(bigptr) + 1;
            DeleteLine(linenum + 1);
            changed = 1;
        }
        else if (cmd == 'p')
        {
            if (last < 0)
            {
                last = first;
            }
            if (first < 0)
            {
                last = first = linenum;
            }
            linenum = first;
            while (linenum <= last)
            {
                printf("%s\n", lineptr[linenum]);
                linenum++;
            }
            linenum = last;
        }
        else if (cmd == 'l')
        {
            if (last < 0)
            {
                last = first;
            }
            if (first < 0)
            {
                last = first = linenum;
            }
            linenum = first;
            while (linenum <= last)
            {
                printf("%s$\n", lineptr[linenum]);
                linenum++;
            }
            linenum = last;
        }
        else if (cmd == 'n')
        {
            if (last < 0)
            {
                last = first;
            }
            if (first < 0)
            {
                last = first = linenum;
            }
            linenum = first;
            while (linenum <= last)
            {
                printf("%4d  %s\n", linenum + 1, lineptr[linenum]);
                linenum++;
            }
            linenum = last;
        }
        else if (cmd == 'h')
        {
            help();
        }
        else
        {
            printf("?\n");
        }
    }
    exit(0);
}

void RemoveCRLF(char *str)
{
    int len;
    len = strlen(str);
    str += len - 1;
    while (len > 0)
    {
        if (*str != 10 && *str != 13) break;
        *str-- = 0;
        len--;
    }
}

void InsertLine(char *str, int linenum)
{
    int i;
    i = numline;
    while (i > linenum)
    {
      lineptr[i] = lineptr[i - 1];
      i--;
    }
    lineptr[linenum] = str;
    numline++;
}

void DeleteLine( int linenum)
{
    int i;
    i = linenum;
    while (i < numline)
    {
        lineptr[i] = lineptr[i + 1];
        i++;
    }
    numline--;
}

char *GetLineNumber(char *str, int linenum, int *num)
{
    if (*str == '.')
    {
        *num = linenum;
        str++;
    }
    else if (*str == '-')
    {
        *num = linenum - 1;
        str++;
    }
    else if (*str == '+')
    {
        *num = linenum + 1;
        str++;
    }
    else if (*str == '$')
    {
        *num = numline - 1;
        str++;
    }
    else if (*str >= '0' && *str <= '9')
    {
        *num = 0;
        while (*str >= '0' && *str <= '9')
        {
            *num = ((*num) * 10) + *str - '0';
            str++;
        }
        num[0]--;
    }
    else
    {
        *num = -2;
    }
    if (*num != - 2 && (*num < 0 || *num >= numline))
    {
        *num = -3;
    }
    return str;
}

int ParseCommand(char *str, int linenum, int *first, int *last, int *cmd)
{
    str = GetLineNumber(str, linenum, first);
    if (*first == -3) return 0;
    if (*str == ',')
    {
        str = GetLineNumber(str + 1, linenum, last);
        if (*last == -3) return 0;
    }
    else
    {
        *last = -2;
    }
    *cmd = *str;
    return 1;
}

void help(void)
{
    printf("TED Commands\n");
    printf("q - quit\n");
    printf("w - write\n");
    printf("d - delete line\n");
    printf("i - insert line\n");
    printf("a - append line\n");
    printf("p - print lines\n");
    printf("j - join line\n");
    printf("# - move to line #\n");
    printf(". - current line\n");
    printf("$ - move to last line\n");
    printf("- - move down one line\n");
    printf("+ - move up one line\n");
    printf("h - print help information\n");
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
