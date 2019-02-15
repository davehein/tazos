//******************************************************************************
// Copyright (c) 2011-2019, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globit.h"

void ConvertToLower(char *ptr);

//************************************************************
// This routine returns a 1 if the passed string contains
// a wildcard character of either "*" or "?.  Otherwise,
// it will return a value of zero.
//************************************************************
int globit_iswild(char *str)
{
  int val;
  while ((val = *str++))
  {
    if (val == '*' || val == '?')
    {
      return 1;
    }
  }
  return 0;
}

//************************************************************
// This routine matches a string containing the wildcard
// characters of "*" and "?" to a string.
//************************************************************
int patmatch(char *pat, char *str)
{
  char *pat1;
  char *str1;

  while (*str && *pat != '*')
  {
    if (*pat != *str && *pat != '?')
      return 0;
    pat++;
    str++;
  }
  while (*str)
  {
    if (*pat == '*')
    {
      if (!*++pat)
        return 1;
      pat1 = pat;
      str1 = str + 1;
    }
    else if (*pat == *str || *pat == '?')
    {
      pat++;
      str++;
    }
    else
    {
      pat = pat1;
      str = str1++;
    }
  }
  while (*pat == '*')
    pat++;
  return !*pat;
}

//************************************************************
// This routine will return matches to the string in pat
// if it contains wildcard characters.  It will return pat
// if it does not contain a wildcard character, or the
// directory path does not exist.  It returns a NULL pointer
// when there are no more items available.
//
// globptr points to a long that will contains the glob state
// information.  This long must be cleared prior to calling
// glob the first time for each pattern.
//
// str is used to hold the return value when a directory path
// is included in the pattern.  Wildcard characters cannot
// be used in the directory path, but can only be used in the
// file name located after the last "/".
//************************************************************
char *globit(char *pat, GlobT *globptr, char *str)
{
  struct dirent *pdirent;
  DIR *handle;
  int state, i, num;
  char *fname;

  state  = globptr->state;
  num    = globptr->num;
  handle = globptr->handle;

  if (state == GLOB_START)
  {
    if (!globit_iswild(pat))
    {
      globptr->state = GLOB_DONE;
      return pat;
    }

    // Locate the last "/" char
    strcpy(str, pat);
    i = num = 0;
    while (str[i])
    {
      if (str[i] == '/')
        num = i;
      i++;
    }

    // Get the directory path
    if (num)
    {
      str[++num] = 0;
      if (globit_iswild(str))
      {
        globptr->state = GLOB_DONE;
        return pat;
      }
    }
    else
      *str = 0;
    globptr->num = num;

    // Open the directory
    handle = opendir(str);
    if (!handle)
    {
      globptr->state = GLOB_DONE;
      return pat;
    }
    globptr->state = state = GLOB_ACTIVE;
    globptr->handle = handle;
  }

  if (state == GLOB_ACTIVE)
  {
    while ((pdirent = readdir(handle)))
    {
      fname = pdirent->d_name;
      if (!strcmp(fname, ".") || !strcmp(fname, ".."))
        continue;
      ConvertToLower(pdirent->d_name);
      if (!num)
      {
        if (patmatch(pat, pdirent->d_name))
          return pdirent->d_name;
      }
      else
      {
        if (patmatch(pat+num, pdirent->d_name))
        {
          strcpy(str+num, pdirent->d_name);
          return str;
        }
      }
    }
    closedir(handle);
    globptr->state = GLOB_DONE;
  }
  return 0;
}

void globit_exit(GlobT *globptr)
{
  int state  = globptr->state;
  DIR *handle = globptr->handle;
  if (state == GLOB_ACTIVE)
    closedir(handle);
  exit(1);
}

void ConvertToLower(char *ptr) 
{
   int val;

   while ((val = *ptr))
   {
     if (val >= 'A' && val <= 'Z')
       *ptr = val + 32;
     ptr++;
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
