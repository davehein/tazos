//******************************************************************************
// Copyright (c) 2014-2019 Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <string.h>
#include "smalloc.h"

StringT *smfreelist = 0;
  
void smallocinit(char *ptr, int size)
{
    StringT *curr = (StringT *)ptr;
  
    smfreelist = curr;
    curr->next = 0;
    curr->size = size;    
}

// Allocate a block of "size" bytes.  Return a pointer to the block if
// successful, or zero if a large enough memory block could not be found
StringT *smalloc(int size)
{
    StringT *prev;
    StringT *curr;
    StringT *next;
    
    //printf("smalloc: %d\n", size);
    
    if (size <= 0) return 0;

    // Adjust size to nearest long plus the header size
    size = ((size + 3) & (~3)) + HDR_SIZE;

    // Search for a block of memory
    prev = 0;
    curr = smfreelist;
    while (curr)
    {
        if (curr->size >= size) break;
        prev = curr;
        curr = curr->next;
    }

    // Return null if block not found
    if (curr == 0) return 0;

    // Get next pointer
    next = curr->next;

    // Split block if larger than needed
    if (curr->size >= size + HDR_SIZE + 16)
    {
	next = STR_ADD(curr, size);
	next->next = curr->next;
	next->size = curr->size - size;
	curr->next = next;
	curr->size = size;
    }

    // Remove block from the free list
    curr->next = 0;
    if (prev) prev->next = next;
    else      smfreelist = next;

    return curr;
}

// Insert a memory block back into the free list.  Merge blocks together if
// the memory block is contiguous with other blocks on the list.
void sfree(StringT *curr)
{
    StringT *prev = 0;
    StringT *next = smfreelist;
    
    // Find Insertion Point
    while (next)
    {
        if (curr >= prev && curr <= next) break;
	prev = next;
	next = next->next;
    }
    
    // Merge with the previous block if contiguous
    if (prev && STR_ADD(prev, prev->size) == curr)
    {
        prev->size += curr->size;
        // Also merge with next block if contiguous
	if (STR_ADD(prev, prev->size) == next)
	{
	    prev->size += next->size;
	    prev->next = next->next;
	}
    }
    
    // Merge with the next block if contiguous
    else if (next && STR_ADD(curr, curr->size) == next)
    {
        curr->size += next->size;
	curr->next = next->next;
	if (prev) prev->next = curr;
	else      smfreelist = curr;
    }
    
    // Insert in the middle of the free list if not contiguous
    else if (prev)
    {
        prev->next = curr;
	curr->next = next;
    }

    // Otherwise, insert at beginning of the free list
    else
    {
        smfreelist = curr;
	curr->next = next;
    }
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
