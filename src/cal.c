//******************************************************************************
// Copyright (c) 2014-2019 Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

char *wday[] = {"Sunday", "Monday", "Tuesday",
    "Wednesday", "Thursday", "Friday", "Saturday"};

char *mname[] = {"January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"};

int numdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

char buffer[8][70];

int dayofweek(int y, int m, int d)
{
    y -= m < 3;
    return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

void RemoveExtraNulls(void)
{
    int i, j, n;
    for (i = 0; i < 8; i++)
    {
        // Locate last null
        for (n = 70-1; n > 0; n--)
        {
            if (buffer[i][n] == 0) break;
        }
        // Remove extra nulls
        for (j = 0; j < n; j++)
        {
            if (buffer[i][j] == 0) buffer[i][j] = ' ';
        }
    }
}

int PrintMonth(int year, int month, int col)
{
    char *ptr;
    int row = 0;
    int i, num, first;

    if (col >= 0)
    {
        num = (22 - strlen(mname[month-1]))/2;
        sprintf(buffer[0] + col + num, "%s", mname[month-1]);
    }
    else
    {
        num = (17 - strlen(mname[month-1]))/2;
        sprintf(buffer[0] + col + num, "%s %d", mname[month-1], year);
        col = 0;
    }
    sprintf(buffer[1] + col, "Su Mo Tu We Th Fr Sa");
    row = 2;
    ptr = buffer[2] + col;
    first = dayofweek(year, month, 1);
    ptr += 3 * first;
    num = numdays[month-1];
    if (month == 2 && year%4 == 0 && (year%100 != 0 || year%400 == 0)) num++;
    for (i = 1; i <= num; i++)
    {
        sprintf(ptr, "%2d ", i);
        ptr += 3;
        if (++first >= 7)
        {
            ptr = buffer[++row] + col;
            first = 0;
        }
    }
    if (first) row++;

    return row;
}

void usage(void)
{
    printf("usage: cal [[month] year]\n");
    exit(1);
}

int main(int argc, char **argv)
{
    int year, month, i, j, num;

    if (argc > 3) usage();

    if (argc == 2)
    {
        year = atol(argv[1]);
        printf("                               %d\n\n", year);
        for (j = 0; j < 12; j += 3)
        {
            memset(buffer[0], ' ', 8*70);
            num = PrintMonth(year, j+1, 0);
            i = PrintMonth(year, j+2, 23);
            num = (i > num) ? i : num;
            i = PrintMonth(year, j+3, 46);
            num = (i > num) ? i : num;
            RemoveExtraNulls();
            for (i = 0; i < num; i++) printf("%s\n", buffer[i]);
            printf("\n");
        }
    }
    else
    {
        if (argc == 3)
        {
            month = atol(argv[1]);
            year = atol(argv[2]);
            if (month < 1 || month > 12) usage();
        }
        else
        {
            month = 2;
            year = 2019;
        }
        memset(buffer[0], ' ', 8*70);
        num = PrintMonth(year, month, -1);
        for (i = 0; i < num; i++) printf("%s\n", buffer[i]);
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
