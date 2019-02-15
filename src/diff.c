//******************************************************************************
// Copyright (c) 2014-2019 Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smalloc.h"

#define NUMLINES    1000000 // Set NUMLINES to a large value so it doesn't limit
#define BUFFER_SIZE 2000    // Define a buffer size of 2000 bytes

void usage(void);
void FillBuffers(FILE *infile1, FILE *infile2);
void FindMatch(int *pnum1, int *pnum2);

static int buffer[BUFFER_SIZE/4]; // Ensure that the buffer is long-aligned
static char linebuf[200];         // This is the input line buffer

// This flag indicates if there is a line in linebuf -- 1 for file1 and 2 for file2
static int pending = 0;

// List 1
StringT *head1 = 0;
StringT *tail1 = 0;
int level1 = 0;
int eofflag1 = 0;

// List 2
StringT *head2 = 0;
StringT *tail2 = 0;
int level2 = 0;
int eofflag2 = 0;

// This program implements the standard UNIX diff function between two files
int main(int argc, char **argv)
{
    FILE *infile1;
    FILE *infile2;
    int line1 = 0;
    int line2 = 0;
    int num1, num2;
    int dashflag;
    StringT *next;

    if (argc != 3) usage();

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
#endif

    // Open file 1
    if (!(infile1 = fopen(argv[1], "r")))
    {
	printf("Could not open %s\n", argv[1]);
	exit(1);
    }

    // Open file 2
    if (!(infile2 = fopen(argv[2], "r")))
    {
	printf("Could not open %s\n", argv[2]);
        fclose(infile1);
	exit(2);
    }

    // Initialize the line buffer
    smallocinit((char *)buffer, BUFFER_SIZE);

    // Begin main loop
    while (1)
    {
        // Read lines into list1 and list2
        FillBuffers(infile1, infile2);

        // Check if list1 and list2 are empty
	if (!head1 && !head2) break;

        // Compare the top line on the two lists
	if (!head1 || !head2 || strcmp(head1->str, head2->str))
	{
            // Find matching lines if the top lines do not match
	    FindMatch(&num1, &num2);

            // Handle the case where the top line on list2 matches a later line on list1
	    if (num1 > 0 && num2 == 0)
	    {
		dashflag = 0;
		printf("%d", line1 + 1);
		if (num1 > 1) printf(",%d", line1 + num1);
	        printf("d%d\n", line2);
	    }

            // Handle the case where the top line on list1 matches a later line on list2
	    else if (num2 > 0 && num1 == 0)
	    {
		dashflag = 0;
		printf("%dd%d", line1, line2 + 1);
		if (num2 > 1) printf(",%d", line2 + num2);
	        printf("\n");
	    }

            // Otherwise, handle the case where later lines on list1 and list2 match
	    else
	    {
		dashflag = 1;
		printf("%d", line1 + 1);
		if (num1 > 1) printf(",%d", line1 + num1);
	        printf("c%d", line2 + 1);
		if (num2 > 1) printf(",%d", line2 + num2);
	        printf("\n");
	    }

            // Print lines from list1 and free them
	    if (num1 > 0)
	    {
		line1 += num1;
		while (num1-- > 0)
		{
	            printf("< %s", head1->str);
		    next = head1->next;
		    sfree(head1);
		    head1 = next;
		    level1--;
		}
	    }

            // Print dashes if we are printing lines from both lists
	    if (dashflag) printf("---\n");

            // Print lines from list2 and free them
	    if (num2 > 0)
	    {
		line2 += num2;
		while (num2-- > 0)
		{
	            printf("> %s", head2->str);
		    next = head2->next;
		    sfree(head2);
		    head2 = next;
		    level2--;
		}
	    }
	}

        // Otherwise, don't print the top lines
	else
	{
            // Free the top line from list1
	    if (head1)
	    {
		line1++;
		next = head1->next;
		sfree(head1);
		head1 = next;
		level1--;
	    }
            // Free the top line from list2
	    if (head2)
	    {
		line2++;
		next = head2->next;
		sfree(head2);
		head2 = next;
		level2--;
	    }
	}

        // Clear the tails if lists are empty
	if (!head1) tail1 = 0;
	if (!head2) tail2 = 0;
    }

    // Close the files before exiting
    fclose(infile1);
    fclose(infile2);
    return 0;
}

// Print out the usage statement and exit      
void usage(void)
{
    printf("usage: diff file1 file2\n");
    exit(1);
}

// This routine reads lines from the input files and adds them
// to list1 and list2 until one of the following occurs:
// - The end of file is reached on both files
// - There are NUMLINES in the buffer for both files
// - There is no more room left in the buffer to add a line
void FillBuffers(FILE *infile1, FILE *infile2)
{
    StringT *next;

    while (1)
    {
        // Quit if EOF or there are NUMLINES on the list for both files
	if ((eofflag1 || level1 >= NUMLINES) &&
	    (eofflag2 || level2 >= NUMLINES)) break;

        // Read a line from an input file if one is not pending
	if (!pending)
	{
            // Read from file 1 if list1 has fewer lines than list2
	    if (!eofflag1 && (level1 <= level2 || eofflag2))
	    {
	        if (!fgets(linebuf, 200, infile1))
		{
		    eofflag1 = 1;
		    continue;
		}
		pending = 1;
	    }
            // Otherwise, read from file 2
	    else if (!eofflag2)
	    {
	        if (!fgets(linebuf, 200, infile2))
		{
		    eofflag2 = 1;
		    continue;
		}
		pending = 2;
	    }
	}

        // Quit if no lines are pending - EOF on both files
	if (!pending) break;

        // Attempt to allocate a line buffer, and quit if there is no room
	next = smalloc(strlen(linebuf) + 1);
	if (!next) break;
	strcpy(next->str, linebuf);

        // Add the line to list1
        if (pending == 1)
        {
	    if (tail1) tail1->next = next;
	    else       head1 = next;
	    tail1 = next;
	    level1++;
        }
        // Add the line to list2
        else if (pending == 2)
        {
	    if (tail2) tail2->next = next;
	    else       head2 = next;
	    tail2 = next;
	    level2++;
        }
	pending = 0;
    }
}

// This routine searches for a line that matches in list1 and list2.  It returns
// the smallest number of lines between list1 and list2 where a match is found.
void FindMatch(int *pnum1, int *pnum2)
{
    int minsum = 1000000;
    int num1, minnum1, count1;
    int num2, minnum2, count2;
    StringT *next1 = head1;
    StringT *next2 = head2;

    minnum1 = level1;
    minnum2 = level2;

    num1 = 0;
    count1 = level1;
    while (count1 > 0)
    {
        num2 = 0;
        count2 = level2;
	next2 = head2;
	while (count2 > 0)
	{
	    if (strcmp(next1->str, next2->str) == 0)
	    {
		if (num2 + num1 < minsum)
		{
		    minnum2 = num2;
		    minnum1 = num1;
		    minsum = num1 + num2;
		    break;
		}
	    }
	    next2 = next2->next;
	    count2--;
	    num2++;
	}
	next1 = next1->next;
	count1--;
	num1++;
    }
    *pnum1 = minnum1;
    *pnum2 = minnum2;
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
