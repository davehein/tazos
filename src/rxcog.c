//******************************************************************************
// Copyright (c) 2019 Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <propeller.h>

#define BUFFER_SIZE 128
#define BUFFER_MASK (BUFFER_SIZE - 1)

volatile int p2bitcycles;
static stack[70];
static char buffer[BUFFER_SIZE];
static volatile int rdindex = 0;
static volatile int wrindex = 0;

// This function receives characters
__attribute__((noinline)) int __getch1(int cycles)
{
    __asm__("mov     r4, r0");
    __asm__("mov     r5, r4");
    __asm__("shr     r5, #1");
    __asm__("mov     r1, #8");
    __asm__("loop1");
    __asm__("and    inb, ##0x80000000 wz");
    __asm__("if_nz  jmp #loop1");
    __asm__("getct   r2");
    __asm__("add     r2, r5");
    __asm__("loop2");
    __asm__("addct1  r2, r4");
    __asm__("waitct1");
    __asm__("shr    r0, #1");
    __asm__("mov    r3, inb");
    __asm__("and    r3, ##0x80000000");
    __asm__("or     r0, r3");
    __asm__("djnz   r1, #loop2");
    __asm__("addct1  r2, r4");
    __asm__("waitct1");
    __asm__("shr    r0, #24");
    __asm__("jmp    lr");
}

// This function runs in a separate cog and stores received
// characters in the buffer.
static void rx_cog(void *ptr)
{
    while (1)
    {
        buffer[wrindex] = __getch1(p2bitcycles);
        wrindex = (wrindex+1) & BUFFER_MASK;
    }
}

// This function returns a 1 if a character has been received
int kbhit(void)
{
    return (rdindex != wrindex);
}

// This function replaces the library getch routine
int getch(void)
{
    int retval;
    while (!kbhit());
    retval = buffer[rdindex];
    rdindex = (rdindex+1) & BUFFER_MASK;
    return retval;
}

// This function starts the serial rx cog
void start_rx_cog(void)
{
    cogstart(rx_cog, 0, stack, sizeof(stack));
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
