//******************************************************************************
// ED Text Editor
// Copyright (c) 2011-2019, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef __P2GCC__
#define getchar getch
#endif

#define HDR_SIZE 8
typedef struct LineS {
        struct LineS *next;
        int size;
        char str[4];
} LineT;

#define UNSPECIFIED   -2
#define PARSING_ERROR -3
#define TAB_CHAR 9
#define MAX_CHARS 200
#define MALLOC_SIZE 16000
#define CHANGE_ONE_LINE 2
#define CHANGE_TWO_LINES_UP 3
#define CHANGE_TWO_LINES_DOWN 4
#define CHANGE_CURSOR_POSITION 5
#define PST_move_up 5
#define PST_move_down 6
#define PST_erase_line 11
#define PST_clear_screen 0
#define OFFSET 0

// Uninitialized global variables
int memorysize;
LineT *ListHead;
LineT *YankHead;
char srchstr[100];
char rplcstr1[100];
char rplcstr2[100];
char filename[100];
char inbuf[MAX_CHARS];
LineT *smfreelist;

void   start(int argc, char **argv);
int    main(int argc, char **argv);
LineT *AllocateString(char *str);
LineT *DuplicateString(LineT *link);
void   CheckMemory(void);
LineT *GetLinePtr(int linenum, LineT **pprev);
void   RemoveCRLF(char *str);
void   InsertLine(LineT *line, int linenum);
void   DeleteLine(int linenum);
int    CheckMatch(char *lineptr, char *str);
int    PrintLines(int linenum, int first, int last, int mode);
int    getnum(char **pstr);
char  *FindChar(char *str, int val);
char  *SkipChar(char *str, int val);
int    FindString(char *str1, char *str2);
int    FindStringBackward(char *str1, char *str2, int len1);
int    GetString(char **pstr);
int    SearchStringForward(char **pstr, int linenum);
int    SearchStringBackward(char **pstr, int linenum);
int    ParseReplaceString(char *str);
int    ReplaceString(char *str1, char *str2);
int    ReplaceStringCommand(char *inptr, int first, int last, int linenum);
char  *GetLineNumber(char *str, int linenum, int *num);
int    ParseCommand(char **pstr, int linenum, int *first, int *last, int *cmd, int *bang);
void   PrintScreen(int linenum);
void   MoveCursorPST(int row, int col);
void   PrintScreenPST(int linenum, int vlines1, int vlines2, int first, int last);
void   MoveCursorANSI(int row, int col);
void   PrintScreenANSI(int linenum, int vlines1, int vlines2, int first, int last);
void   PrintScreenDumb(int linenum, int vlines1, int vlines2, int first, int last);
void   EmptyList(LineT *list);
int    DeleteLines(int first, int last);
int    YankLines(int first, int last);
int    PushLines(int line);
int    AppendLines(int linenum);
int    InsertLines(int linenum);
int    JoinLines(int first, int last);
int    MoveLines(int first, int last, int dstaddr, int cmd);
int    ReadFile(char *fname, int linenum);
int    WriteFile(char *fname, int first, int last);
int    ExecuteCommand(void);
void   SetDispCol(void);
int    NumDigits(int num);
void   EraseStatusLine(void);
void   ExecuteViCommand(int cmd);
int    FindNextString(int dir, int newline);
void   VisualSearchString(char *ptr, int dir);
void   smallocinit(char *ptr, int size);
LineT *smalloc(int size);
void   sfree(LineT *curr);

// Initialized global variables
int rflag = 0;
int gflag = 0;
int vmode = 1;
int prompt = 0;
int vlines = 21;
int scroll = 20;
int changed = 0;
int numlines = 0;
int currline = 0;
int currcol = 1;
int dispcol = 1;
int retcode = 0;
int cmd0 = 0;
int count = 0;
int schange = 1;
int topline = 1;
int termtype = 0;
int cursorflag = 0;
int pstflag = 0;
int ansiflag = 0;
int statuslen = 0;
int srchdir = 1;
int lastcmd = 0;
int lastcnt = 0;
int lastval = 0;
char nullstr = 0;
char ANSI_move_up[4] = {27, '[', 'A', 0};
char ANSI_move_down[4] = {27, '[', 'B', 0};
char ANSI_erase_line[4] = {27, '[', 'K', 0};
char ANSI_scroll_up[8] = {27, '[', '9', '9', 'B', 27, 'D', 0};
char ANSI_scroll_down[3] = {27, 'M', 0};
char ANSI_clear_screen[5] = {27, '[', '2', 'J', 0};

// Main program entry
int main(int argc, char **argv)
{
    //char *tmpptr;
    int cmd;
    char *strbuf;
    //argc := 1
    // Initialization

#ifdef __P2GCC__
    sd_mount(58, 61, 59, 60);
    chdir(argv[argc]);
#endif

    srchstr[0] = 0;
    rplcstr1[0] = 0;
    rplcstr2[0] = 0;
    ListHead = 0;
    YankHead = 0;
    prompt = ':';
    // Allocate space for file
    //tmpptr = malloc(600);
    // Allocate the string buffer
    memorysize = MALLOC_SIZE;
    while (memorysize > 0)
    {
        strbuf = malloc(memorysize);
        if (strbuf) break;
        memorysize -= 1000;
    }
    //free(tmpptr);
    smallocinit(strbuf, memorysize);
    //printf(string("memorysize = %d\n"), memorysize)
    // Copy the input file name if specified
    if (argc > 1)
    {
        strcpy(filename, argv[1]);
    }
    else
    {
        filename[0] = 0;
    }
    currline = ReadFile(filename, 0);
    //printf(string("Press a key\n"))
    //getchar
    if (numlines > 0)
    {
        currline = 1;
    }
    // Get a command and execute it
    while (1)
    {
        gflag = 0;
        CheckMemory();
        if (vmode == 1)
        {
            if (schange)
            {
                PrintScreen(currline);
                schange = 0;
            }
            cmd = getchar();
            ExecuteViCommand(cmd);
        }
        else
        {
            if (prompt)
            {
                printf("%c", prompt);
            }
            gets(inbuf);
            retcode = ExecuteCommand();
            if (retcode > 0)
            {
                break;
            }
            else if (retcode < 0)
            {
                printf("E%d:\n", - retcode);
            }
            if (vmode == -1)
            {
                vmode = 1;
            }
        }
    }
    free(strbuf);
    exit(0);
}

// Allocate a string link and copy contents of "str"
LineT *AllocateString(char *str)
{
    LineT *link = smalloc(strlen(str) + 1);
    if (link) strcpy(link->str, str);
    return link;
}

// Duplicate a string link
LineT *DuplicateString(LineT *link)
{
    return AllocateString(link->str);
}

// Check for a memory leak
void CheckMemory(void)
{
    int total;
    LineT *curr;
    int listsize = 0;
    int yanksize = 0;
    int freesize = 0;
    curr = ListHead;

    while (curr)
    {
        listsize += curr->size;
        curr = curr->next;
    }
    curr = YankHead;
    while (curr)
    {
        yanksize += curr->size;
        curr = curr->next;
    }
    curr = smfreelist;
    while (curr)
    {
        freesize += curr->size;
        curr = curr->next;
    }
    total = listsize + yanksize + freesize;
    if (total != memorysize)
    {
        printf("Memory leak!\n");
        printf("listsize = %d, yanksize = %d, freesize = %d, total = %d\n", listsize, yanksize, freesize, listsize + yanksize + freesize);
    }
}

// Get a pointer to a string link from its line number
LineT *GetLinePtr(int linenum, LineT **pprev)
{
    LineT *prev = 0;
    LineT *curr = ListHead;
    while (curr && --linenum > 0)
    {
        prev = curr;
        curr = curr->next;
    }
    *pprev = prev;
    return curr;
}

// Remove the CR and LF characters from the end of a line
void RemoveCRLF(char *str)
{
    int len;
    len = strlen(str);
    str += len - 1;
    while (len > 0)
    {
        if (*str != 10 && *str != 13)
        {
            break;
        }
        *str-- = 0;
        len--;
    }
}

// Insert a string link at the line given by "linenum"
void InsertLine(LineT *line, int linenum)
{
    LineT *prev;
    LineT *curr;
    curr = GetLinePtr(linenum - 1, &prev);
    if (linenum < 1 || linenum > numlines + 1)
    {
        printf("InsertLine: linenum = %d, numlines = %d\n", linenum, numlines);
    }
    if (curr == 0 && (linenum != 1 || ListHead != 0))
    {
        printf("InsertLine: curr == 0, linenum = %d\n", linenum);
    }
    if (linenum == 1)
    {
        line->next = ListHead;
        ListHead = line;
    }
    else
    {
        line->next = curr->next;
        curr->next = line;
    }
    numlines++;
}

// Delete the line at "linenum"
void DeleteLine(int linenum)
{
    LineT *prev;
    LineT *curr;
    curr = GetLinePtr(linenum, &prev);
    if (!curr)
    {
        printf("DeleteLine: curr = 0, linenum = %d\n", linenum);
        return;
    }
    if (!prev)
    {
        ListHead = curr->next;
    }
    else
    {
        prev->next = curr->next;
    }
    sfree(curr);
    numlines--;
}

// Based on the value of gflag, check for if the line
// at "lineptr" does, or does not contains "str"
int CheckMatch(char *lineptr, char *str)
{
    if (gflag == 0)
        return 0;
    if (gflag == 1)
    {
        if (!FindString(lineptr, str))
            return 1;
        return 0;
    }
    if (FindString(lineptr, str))
        return 1;
    return 0;
}

// Print lines using the "n", "v", "l" or "p" format
int PrintLines(int linenum, int first, int last, int mode)
{
    int col, len, line, lastproc;
    char *LinePtr;
    LineT *prev;
    LineT *curr;
    lastproc = linenum;
    curr = GetLinePtr(first, &prev);
    line = first;
    while (line <= last)
    {
        if (!curr)
        {
            printf("PrintLines: curr = 0, line = %d\n", line);
            return 0;
        }
        LinePtr = curr->str;
        curr = curr->next;
        if (CheckMatch(LinePtr, srchstr))
        {
            line++;
            continue;
        }
        if (mode == 'n')
        {
            printf("%6d  %s\n", line, LinePtr);
        }
        else if (mode == 'v')
        {
            if (line == linenum && !cursorflag)
            {
                len = strlen(LinePtr);
                col = 0;
                while (col < len)
                {
                    if (col + 1 != dispcol)
                    {
                        printf("%c", LinePtr[col]);
                    }
                    else
                    {
                        printf("%c", '@');
                    }
                    col++;
                }
                if (dispcol > len)
                {
                    printf("%c", '@');
                }
                printf("\n");
            }
            else
            {
                printf("%s\n", LinePtr);
            }
        }
        else if (mode == 'l')
        {
            while (*LinePtr)
            {
                if (*LinePtr >= 32 && *LinePtr < 127)
                {
                    printf("%c", *LinePtr);
                }
                else if (*LinePtr == TAB_CHAR)
                {
                    printf("\\t");
                }
                else
                {
                    printf("\\x%2.2x", *LinePtr);
                }
                LinePtr++;
            }
            printf("$\n");
        }
        else
        {
            printf("%s\n", LinePtr);
        }
        lastproc = line;
        line++;
    }
    return lastproc;
}

// Decode a number from the string
int getnum(char **pstr)
{
    int val = 0;
    char *str = *pstr;
    while (isdigit(*str))
    {
        val = (val * 10) + *str++ - '0';
    }
    *pstr = str;
    return val;
}

// Find the next character that matches "val"
char *FindChar(char *str, int val)
{
    while (*str)
    {
        if (*str == val) break;
        str++;
    }
    return str;
}

// Skip the next character that does not match "val"
char *SkipChar(char *str, int val)
{
    while (*str)
    {
        if (*str != val) break;
        str++;
    }
    return str;
}

// Return column if "str2" is found in "str1", else return 0
int FindString(char *str1, char *str2)
{
    int len, col;
    col = 1;
    len = strlen(str2);
    while (*str1)
    {
        if (strncmp(str1, str2, len) == 0)
            return col;
        str1++;
        col++;
    }
    return 0;
}

// Return column if "str2" is found in "str1", else return 0
int FindStringBackward(char *str1, char *str2, int len1)
{
    int len2;
    if (len1 < 1)
    {
        return 0;
    }
    str1 += len1 - 1;
    len2 = strlen(str2);
    while (len1)
    {
        if (strncmp(str1, str2, len2) == 0)
            return len1;
        len1--;
        str1--;
    }
    return 0;
}

// Use the first character as a delimeter and copy the string to srchstr
int GetString(char **pstr)
{
    char *ptr;
    char *str = *pstr;
    int val = *str++;
    int len;

    if (!val) return 0;
    ptr = FindChar(str, val);
    len = (ptr) - (str);
    if (*ptr) ptr++;
    *pstr = ptr;
    if (len > 0)
    {
        memcpy(srchstr, str, len);
        srchstr[len] = 0;
    }
    return 1;
}

// Search forward through all the lines for the first occurance of srchstr
int SearchStringForward(char **pstr, int linenum)
{
    int i;
    LineT *prev;
    LineT *curr = GetLinePtr(linenum + 1, &prev);
    if (!GetString(pstr))
    {
        return PARSING_ERROR;
    }
    i = linenum + 1;
    while (i <= numlines)
    {
        if (!curr)
        {
            printf("SearchStringForward: curr = 0, line = %d\n", i);
            return - 3;
        }
        if (FindString(curr->str, srchstr))
            return i;
        curr = curr->next;
        i++;
    }
    curr = ListHead;
    i = 1;
    while (i <= linenum)
    {
        if (!curr)
        {
            printf("SearchStringForward: curr = 0, line = %d\n", i);
            return -3;
        }
        if (FindString(curr->str, srchstr))
            return i;
        curr = curr->next;
        i++;
    }
    return PARSING_ERROR;
}

// Search backward through all the lines for the first occurance of srchstr
// Note: This is not as efficient as the forward seach because we must use
// GetLinePtr to get the string link pointer
int SearchStringBackward(char **pstr, int linenum)
{
    int i;
    LineT *prev;
    LineT *curr;
    if (!GetString(pstr))
    {
        return PARSING_ERROR;
    }
    i = linenum - 1;
    while (i >= 1)
    {
        curr = GetLinePtr(i, &prev);
        if (!curr)
        {
            printf("SearchStringBackward: curr = 0, line = %d\n", i);
            return -3;
        }
        if (FindString(curr->str, srchstr))
            return i;
        i--;
    }
    i = numlines;
    while (i >= linenum)
    {
        curr = GetLinePtr(i, &prev);
        if (!curr)
        {
            printf("SearchStringBackward: curr = 0, line = %d\n", i);
            return - 3;
        }
        if (FindString(curr->str, srchstr))
            return i;
        i--;
    }
    return PARSING_ERROR;
}

// Parse the replace string command and extract rplcstr1 and rplcstr2
int ParseReplaceString(char *str)
{
    char *ptr;
    if (*str++ != 's')
    {
        return 0;
    }
    if (*str == 0)
    {
        return 1;
    }
    if (*str++ != '/')
    {
        return 0;
    }
    ptr = FindChar(str, '/');
    if (*ptr != '/')
    {
        return 0;
    }
    *ptr++ = 0;
    if (*str)
    {
        strcpy(rplcstr1, str);
    }
    else
    {
        strcpy(rplcstr1, srchstr);
    }
    if (*ptr == 0)
    {
        return 1;
    }
    str = FindChar(ptr, '/');
    if (*str != '/')
    {
        return 0;
    }
    *str++ = 0;
    strcpy(rplcstr2, ptr);
    rflag = (*str == 'g');
    return 1;
}

// Store the results of the replace-string command on str1 into str2
int ReplaceString(char *str1,    char *str2)
{
    int changeflag, len1, len2;
    changeflag = 0;
    len1 = strlen(rplcstr1);
    len2 = strlen(rplcstr2);
    // Search through str1 for rplcstr1 and replace with rplcstr2
    while (*str1)
    {
        if (strncmp(str1, rplcstr1, len1) == 0)
        {
            changeflag = 1;
            memcpy(str2, rplcstr2, len2);
            str2 += len2;
            str1 += len1;
            // Quit after the first time if 'g' was not specified
            if (!rflag)
            {
                break;
            }
        }
        else
        {
            *str2++ = *str1++;
        }
    }
    // Copy the remainder of str1 to str2
    while (*str1)
    {
        *str2++ = *str1++;
    }
    *str2 = 0;
    return changeflag;
}

// Execute the replace-string command on multiple lines in the text buffer
int ReplaceStringCommand(char *inptr, int first, int last, int linenum)
{
    int i, lastproc, changeflag;
    LineT *link;
    LineT *prev;
    LineT *curr;
    lastproc = linenum;
    changeflag = 0;
    curr = GetLinePtr(first, &prev);
    if (!ParseReplaceString(inptr))
    {
        printf("?\n");
        return last;
    }
    i = first;
    while (i <= last)
    {
        if (!curr)
        {
            printf("ReplaceStringCommand: line = %d\n", i);
            return 1;
        }
        if (CheckMatch(curr->str, srchstr))
        {
            prev = curr;
            curr = curr->next;
            i++;
            continue;
        }
        if (ReplaceString(curr->str, inbuf))
        {
            link = curr;
            curr = smalloc(strlen(inbuf) + 1);
            if (!curr)
            {
                return lastproc;
            }
            strcpy(curr->str, inbuf);
            if (prev)
            {
                prev->next = curr;
            }
            else
            {
                ListHead = curr;
            }
            curr->next = link->next;
            sfree(link);
            changeflag = 1;
            changed = 1;
        }
        prev = curr;
        curr = curr->next;
        lastproc = i;
        i++;
    }
    if (!changeflag)
    {
        printf("?\n");
    }
    return lastproc;
}

// Extract a line number parameter from a command
char *GetLineNumber(char *str, int linenum, int *num)
{
    int val;
    if (*str == '.')
    {
        *num = linenum;
        str++;
    }
    else if (*str == '-')
    {
        str++;
        if (isdigit(*str))
        {
            val = getnum(&str);
        }
        else
        {
            val = 1;
        }
        *num = linenum - val;
    }
    else if (*str == '+')
    {
        str++;
        if (isdigit(*str))
        {
            val = getnum(&str);
        }
        else
        {
            val = 1;
        }
        *num = linenum + val;
    }
    else if (*str == '$')
    {
        *num = numlines;
        str++;
    }
    else if (*str == '/')
    {
        *num = SearchStringForward(&str, linenum);
    }
    else if (*str == '?')
    {
        *num = SearchStringBackward(&str, linenum);
    }
    else if (isdigit(*str))
    {
        *num = getnum(&str);
    }
    else
    {
        *num = UNSPECIFIED;
    }
    if (*num != UNSPECIFIED && (*num < 0 || *num > numlines))
    {
        *num = PARSING_ERROR;
    }
    return str;
}

// Extract the first and last numbers and the command from a command string
int ParseCommand(char **pstr, int linenum, int *first, int *last, int *cmd, int *bang)
{
    int done = 0;
    char *str = *pstr;
    *bang = 0;
    if (*str == '%')
    {
        *first = 1;
        *last = numlines;
        done = 1;
    }
    else if (*str == ';')
    {
        *first = linenum;
        *last = numlines;
        done = 1;
    }
    else if (*str == ',')
    {
        *first = *last = numlines;
        done = 1;
    }
    if (done)
    {
        str++;
        *cmd = *str;
        if (*str && str[1] == '!')
        {
            *bang = 1;
            str++;
        }
        *pstr = str;
        return 1;
    }
    str = GetLineNumber(str, linenum, first);
    if (*first == PARSING_ERROR)
    {
        return 0;
    }
    if (*first == UNSPECIFIED)
    {
        if (numlines == 0 && *str == 'i')
        {
            *first = *last = 1;
        }
        else if (*str == 'g' || *str == 'v' || *str == 'w' || *str == 'x')
        {
            *first = 1;
            *last = numlines;
        }
        else if (*str == 'j')
        {
            *first = linenum;
            *last = linenum + 1;
            if (*last > numlines)
            {
                return 0;
            }
        }
        else if (*str == 'r')
        {
            *first = numlines;
            *last = numlines;
        }
        else if (*str == 0 || *str == 'z')
        {
            *first = *last = linenum + 1;
            if (*first > numlines)
            {
                return 0;
            }
        }
        else if (*str == '=')
        {
            *first = *last = numlines;
        }
        else
        {
            *first = linenum;
            *last = linenum;
        }
    }
    else if (*str == ',' || *str == ';')
    {
        str = GetLineNumber(str + 1, linenum, last);
        if (*last == PARSING_ERROR)
        {
            return 0;
        }
        if (*last == UNSPECIFIED)
        {
            *last = *first;
        }
        if (*last < (*first))
        {
            return 0;
        }
    }
    else
    {
        *last = *first;
    }
    *cmd = *str;
    if (*str && str[1] == '!')
    {
        *bang = 1;
        str++;
    }
    *pstr = str;
    // If first is zero, check if commands allows that
    if (*first == 0)
    {
        str = FindChar("aq=EerxQhVfP", *cmd);
        if (*str == 0)
        {
            return 0;
        }
    }
    return 1;
}

// Print a visualization mode screen
void PrintScreen(int linenum)
{
    int vlines1, vlines2, first, last;
    vlines1 = vlines - 1;
    vlines2 = vlines / 2;
    first = topline;
    last = first + vlines1;
    if (pstflag)
    {
        PrintScreenPST(linenum, vlines1, vlines2, first, last);
    }
    else if (ansiflag)
    {
        PrintScreenANSI(linenum, vlines1, vlines2, first, last);
    }
    else
    {
        PrintScreenDumb(linenum, vlines1, vlines2, first, last);
    }
    // Print status line
    if (retcode < 0)
    {
        printf("E%d:", - retcode);
        statuslen = 2 + NumDigits(- retcode);
        retcode = 0;
    }
    else
    {
        printf("line %d, column %d, %d lines", currline, dispcol, numlines);
        statuslen = 22 + NumDigits(currline) + NumDigits(dispcol) + NumDigits(numlines);
        if (filename[0])
        {
            printf(", %s", filename);
            statuslen += strlen(filename) + 2;
        }
    }
    if (termtype == 2)
        MoveCursorANSI(currline - topline + 1, dispcol);
    else if (termtype == 3)
        MoveCursorPST(currline - topline + 1, dispcol);
}

void MoveCursorPST(int row, int col)
{
    putchar(2);
    putchar(col - 1);
    putchar(row - 1);
}

void PrintScreenPST(int linenum, int vlines1, int vlines2, int first, int last)
{
    int linenum1, linenum2;
    if (schange > 1)
    {
        if (schange == 2)
        {
            linenum2 = linenum1 = linenum;
        }
        else if (schange == 3)
        {
            linenum1 = linenum - 1;
            linenum2 = linenum;
        }
        else
        {
            linenum1 = linenum;
            linenum2 = linenum + 1;
        }
    }
    // Check if we need to only repaint the current line
    if (schange > 1 &&linenum1 >= first && linenum2 <= last)
    {
        MoveCursorPST(linenum1 - topline + 1, 1);
        putchar(PST_erase_line);
        if (linenum1 != linenum2)
        {
            putchar(PST_move_down);
            putchar(PST_erase_line);
            putchar(PST_move_up);
        }
        PrintLines(linenum, linenum1, linenum2, 'v');
        EraseStatusLine();
    }
    else
    {
        // Clear the screen
        putchar(PST_clear_screen);
        PrintScreenDumb(linenum, vlines1, vlines2, first, last);
    }
}

void MoveCursorANSI(int row, int col)
{
    printf("\033[%d;%dH", row, col);
}

void PrintScreenANSI(int linenum, int vlines1, int vlines2, int first, int last)
{
    int linenum1, linenum2;
    if (schange > 1)
    {
        if (schange == 2 || schange == 5)
        {
            linenum2 = linenum1 = linenum;
        }
        else if (schange == 3)
        {
            linenum1 = linenum - 1;
            linenum2 = linenum;
        }
        else
        {
            linenum1 = linenum;
            linenum2 = linenum + 1;
        }
    }
    // Check if we need to only repaint the current line
    if (schange > 1 && linenum1 >= first && linenum2 <= last)
    {
        if (schange == 5)
        {
            EraseStatusLine();
            return;
        }
        MoveCursorANSI(linenum1 - topline + 1, 1);
        fputs(ANSI_erase_line, stdout);
        if (linenum1 != linenum2)
        {
            fputs(ANSI_move_down, stdout);
            fputs(ANSI_erase_line, stdout);
            fputs(ANSI_move_up, stdout);
        }
        PrintLines(linenum, linenum1, linenum2, 'v');
        EraseStatusLine();
    }
    else if ( schange == 5 &&linenum == first - 1)
    {
        EraseStatusLine();
        MoveCursorANSI(1, 1);
        fputs(ANSI_scroll_down, stdout);
        MoveCursorANSI(1, 1);
        topline = -- first;
        last = first + vlines1;
        if (last > numlines)
        {
            last = numlines;
        }
        PrintLines(linenum, linenum1, linenum2, 'v');
        EraseStatusLine();
    }
    else if ( schange == 5 &&linenum == last + 1)
    {
        //MoveCursorANSI(1, 1)
        //fputs(@ANSI_erase_line, stdout)
        EraseStatusLine();
        fputs(ANSI_scroll_up, stdout);
        //putchar(27)
        //putchar("D")
        MoveCursorANSI(vlines, 1);
        topline = ++ first;
        last = first + vlines1;
        if (last > numlines)
        {
            last = numlines;
        }
        //printf(string("SU:"))
        PrintLines(linenum, linenum1, linenum2, 'v');
        EraseStatusLine();
    }
    else
    {
        // Clear the screen
        fputs(ANSI_clear_screen, stdout);
        MoveCursorANSI(1, 1);
        PrintScreenDumb(linenum, vlines1, vlines2, first, last);
    }
}

void PrintScreenDumb(int linenum, int vlines1, int vlines2, int first, int last)
{
    int i;
    // Clear the screen
    if (!(ansiflag || pstflag))
    {
        i = 0;
        while (i < 50)
        {
            printf("\n");
            i++;
        }
    }
    // Determine the first and last lines to display
    if (first > currline)
    {
        first = currline;
        last = first + vlines1;
    }
    else if (currline > last)
    {
        last = currline;
        first = last - vlines1;
        if (first < 1)
        {
            first = 1;
        }
    }
    if (last > numlines)
    {
        last = numlines;
    }
    if (first > 0 &&numlines > 0)
    {
        topline = first;
        PrintLines(linenum, first, last, 'v');
    }
    else
    {
        last = first - 1;
    }
    i = last - first;
    while (i < vlines1)
    {
        printf("~\n");
        i++;
    }
}

// Empty a list and free the string links
void EmptyList(LineT *list)
{
    LineT *next;
    while (list)
    {
        next = list->next;
        sfree(list);
        list = next;
    }
}

// Delete lines from the text file list and move them to the yank list
int DeleteLines(int first, int last)
{
    int i;
    LineT *prev;
    LineT *curr = GetLinePtr(first, &prev);
    LineT *yank = 0;
    EmptyList(YankHead);
    YankHead = 0;
    i = first;
    while (i <= last)
    {
        if (!curr)
        {
            printf("DeleteLines: line = %d\n", first);
            return 1;
        }
        if (!yank)
        {
            YankHead = curr;
        }
        else
        {
            yank->next = curr;
        }
        yank = curr;
        curr = curr->next;
        numlines--;
        i++;
    }
    if (prev)
    {
        prev->next = curr;
    }
    else
    {
        ListHead = curr;
    }
    yank->next = 0;
    changed = 1;
    if (first > numlines)
    {
        first = numlines;
    }
    return first;
}

// Copy lines from the text file list to the yank list
int YankLines(int first, int last)
{
    int line;
    LineT *link;
    LineT *list = 0;
    LineT *curr = GetLinePtr(first, &link);
    EmptyList(YankHead);
    YankHead = 0;
    line = first;
    while (line <= last)
    {
        if (!curr)
        {
            printf("YankLines: line = %d\n", line);
            return 1;
        }
        link = DuplicateString(curr);
        if (!link)
        {
            break;
        }
        if (!list)
        {
            YankHead = link;
        }
        else
        {
            list->next = link;
        }
        curr = curr->next;
        list = link;
        line++;
    }
    if (list)
    {
        list->next = 0;
    }
    return last;
}

// Copy the lines on the yank list to the text file list
int PushLines(int line)
{
    LineT *link;
    LineT *curr = YankHead;
    while (curr)
    {
        link = DuplicateString(curr);
        if (!link) break;
        InsertLine(link, ++line);
        curr = curr->next;
        changed = 1;
    }
    return line;
}

// Go into the text input mode and append after "linenum"
int AppendLines(int linenum)
{
    LineT *prev;
    LineT *curr;
    LineT *link;
    curr = GetLinePtr(linenum + 1, &prev);
    while (1)
    {
        gets(inbuf);
        RemoveCRLF(inbuf);
        if (inbuf[0] == '.' && inbuf[1] == 0)
        {
            break;
        }
        link = AllocateString(inbuf);
        if (!link)
        {
            break;
        }
        link->next = curr;
        if (prev)
        {
            prev->next = link;
        }
        else
        {
            ListHead = link;
        }
        prev = link;
        linenum++;
        numlines++;
        changed = 1;
    }
    return linenum;
}

// Go into the text input mode and insert lines before "linenum"
int InsertLines(int linenum)
{
    int linenum1;
    linenum1 = AppendLines(linenum - 1);
    if (linenum1 > linenum)
    {
        linenum = linenum1;
    }
    return linenum;
}

// Join lines together and move the original lines to the yank buffer
int JoinLines(int first, int last)
{
    int i, len;
    char *str;
    LineT *prev;
    LineT *curr;
    LineT *link;
    LineT *yank;
    len = 0;
    curr = GetLinePtr(first, &prev);
    link = curr;
    yank = 0;
    EmptyList(YankHead);
    YankHead = 0;
    // Determine the total length of the selected lines
    i = first;
    while (i <= last)
    {
        if (!link)
        {
            printf("JoinLines: line = %d\n", i);
            return 1;
        }
        len += strlen(link->str);
        link = link->next;
        i++;
    }
    // Allocate a new string buffer and concatenate the strings
    link = smalloc(len + 1);
    if (!link)
    {
        return first;
    }
    str = link->str;
    *str = 0;
    i = first;
    while (i <= last)
    {
        if (!curr)
        {
            printf("JoinLines: line = %d\n", i);
            return 1;
        }
        strcat(str, curr->str);
        if (!yank)
        {
            YankHead = curr;
        }
        else
        {
            yank->next = curr;
        }
        yank = curr;
        curr = curr->next;
        numlines--;
        changed = 1;
        i++;
    }
    // Insert the new string
    if (prev)
    {
        prev->next = link;
    }
    else
    {
        ListHead = link;
    }
    link->next = curr;
    yank->next = 0;
    numlines++;
    return first;
}

// Move or copy lines to after the destination address
int MoveLines(int first, int last, int dstaddr, int cmd)
{
    int i;
    LineT *prev;
    LineT *curr;
    LineT *link;
    int srcaddr, insertbefore;
    if (dstaddr < 0 || dstaddr > numlines || (first <= dstaddr &&dstaddr < last))
    {
        printf("?\n");
    }
    else
    {
        srcaddr = first;
        insertbefore = dstaddr < first;
        i = first;
        while (i <= last)
        {
            curr = GetLinePtr(srcaddr++, &prev);
            link = DuplicateString(curr);
            if (!link)
            {
                break;
            }
            InsertLine(link, ++dstaddr);
            if (insertbefore)
            {
                srcaddr++;
            }
            if (cmd == 'm')
            {
                DeleteLine(--srcaddr);
                if (!insertbefore)
                {
                    dstaddr--;
                }
            }
            i++;
        }
        changed = 1;
    }
    return dstaddr;
}

// Read a file after "linenum"
int ReadFile(char *fname, int linenum)
{
    int numchar;
    FILE *infile;
    LineT *link;
    numchar = 0;
    infile = fopen(fname, "r");
    if (infile)
    {
        while (fgets(inbuf, MAX_CHARS, infile))
        {
            RemoveCRLF(inbuf);
            link = AllocateString(inbuf);
            if (!link)
            {
                break;
            }
            InsertLine(link, ++ linenum);
            numchar += strlen(inbuf);
        }
        fclose(infile);
    }
    printf("%d\n", numchar);
    return linenum;
}

// Write lines first to last to the output file
int WriteFile(char *fname, int first, int last)
{
    int i;
    char *LinePtr;
    LineT *curr;
    LineT *prev;
    int numchar;
    FILE *outfile;
    numchar = 0;
    outfile = fopen(fname, "w");
    if (!outfile)
    {
        return - 1;
    }
    i = first;
    while (i <= last)
    {
        curr = GetLinePtr(i, &prev);
        LinePtr = curr->str;
        numchar += strlen(LinePtr);
        fprintf(outfile, "%s\n", LinePtr);
        i++;
    }
    fclose(outfile);
    printf("%d\n", numchar);
    if (first == 1 && last == numlines)
    {
        changed = 0;
    }
    return 0;
}

// Execute a command
int ExecuteCommand(void)
{
    LineT *curr;
    LineT *prev;
    char *LinePtr;
    char *inptr;
    int first, last, cmd, temp, bang;
    inptr = inbuf;
    // Parse the command and extract the parameters
    if (!ParseCommand(&inptr, currline, &first, &last, &cmd, &bang))
    {
        return - 2;
    }
    // Parse a "g" or "v" command if present
    if (cmd == 'g' || cmd == 'v')
    {
        inptr++;
        if (GetString(&inptr))
        {
            if (cmd == 'g')
            {
                gflag = 1;
            }
            else
            {
                gflag = 2;
            }
            cmd = *inptr;
            if (cmd == 0)
            {
                cmd = 'p';
            }
            printf("cmd = %c\n", cmd);
        }
        else
        {
            return - 3;
        }
    }
    // Select the command and execute it
    switch (cmd)
    {
        case 0 :
            // Print the current line if no command
            if (numlines == 0)
            {
                return - 4;
            }
            if (last >= 0)
            {
                currline = last;
            }
            else if (first >= 0)
            {
                currline = first;
            }
            curr = GetLinePtr(currline, &prev);
            LinePtr = curr->str;
            printf("%s\n", LinePtr);
            break;
        case 'q' :
            if (changed == 0 || bang)
            {
                return 1;
            }
            return - 5;
            break;
        case 'e' :
            if (changed &&bang == 0)
            {
                return - 6;
            }
            inptr = SkipChar(inptr + 1, ' ');
            if (*inptr == 0 && filename[0]== 0)
            {
                return - 7;
            }
            if (*inptr)
            {
                strcpy(filename, inptr);
            }
            EmptyList(YankHead);
            YankHead = 0;
            EmptyList(ListHead);
            ListHead = 0;
            numlines = 0;
            currline = ReadFile(filename, 0);
            changed = 0;
            break;
        case 'r' :
            // Read the contents of a file after the specified line
            inptr = SkipChar(inptr + 1, ' ');
            if (*inptr == 0 && filename[0]== 0)
            {
                return - 8;
            }
            if (*inptr)
            {
                if (!filename[0])
                {
                    strcpy(filename, inptr);
                }
            }
            else
            {
                inptr = filename;
            }
            temp = numlines;
            currline = ReadFile(inptr, first);
            if (temp && numlines != temp)
            {
                changed = 1;
            }
            break;
        case 'x' :
        case 'w' :
            // Write the text buffer to a file
            inptr = SkipChar(inptr + 1, ' ');
            if (*inptr == 0)
            {
                if (filename[0]== 0)
                {
                    return - 9;
                }
                inptr = filename;
            }
            if (WriteFile(inptr, first, last) < 0)
            {
                return - 10;
            }
            if (cmd == 'x')
            {
                return 1;
            }
            if (filename[0]== 0)
            {
                strcpy(filename, inptr);
            }
            break;
        case 'd' :
            // Delete the specified lines
            currline = DeleteLines(first, last);
            break;
        case 'm' :
        case 't' :
            // Move or copy the specified lines
            if (inptr[1])
            {
                sscanf(inptr + 1, "%d", &temp);
            }
            else
            {
                temp = currline;
            }
            currline = MoveLines(first, last, temp, cmd);
            break;
        case 'c' :
            // Change the specified lines
            temp = (last == numlines);
            currline = DeleteLines(first, last);
            if (temp)
            {
                currline = AppendLines(currline);
            }
            else
            {
                currline = InsertLines(currline);
            }
            break;
        case 'a' :
            // Append text after the specified line
            currline = AppendLines(first);
            break;
        case 'i' :
            // Insert text before the specified line
            currline = InsertLines(first);
            break;
        case 'j' :
            // Join together lines
            currline = JoinLines(first, last);
            break;
        case 'p' :
        case 'l' :
        case '#' :
            // Print lines in either the "p", "l" or "n" format
            currline = PrintLines(currline, first, last, cmd);
            break;
        case 'V' :
            // Set the number of lines in the visualization mode or
            // toggle the visualization mode if no lines are specified
            if (inptr[1])
            {
                sscanf(inptr + 1, "%d", &temp);
                if (temp <= 0)
                {
                    return -11;
                }
                vlines = temp - 1;
                vmode = 1;
            }
            else
            {
                vmode = 1;
            }
            break;
        case 'T' :
            // Set the terminal type, or toggle the terminal
            // type if no parameter is specified
            if (inptr[1])
                sscanf(inptr + 1, "%d", &termtype);
            else
                termtype = 0;
            break;
        case 's' :
            // Replace text in the specified lines
            currline = ReplaceStringCommand(inptr, first, last, currline);
            break;
        case 'P' :
            // Push text from the yank buffer
            currline = PushLines(first);
            break;
        case 'y' :
            // Yank lines to the yank buffer
            currline = YankLines(first, last);
            break;
        case '=' :
            // Print the number of lines, or the line number if specified
            printf("%d\n", last);
            break;
        default :
            // Return error code if not a valid command
            return - 14;
            break;
    }
    return 0;
}

void SetDispCol(void)
{
    int len;
    LineT *curr = GetLinePtr(currline, &curr);
    if (curr)
    {
        len = strlen(curr->str);
        if (currcol > len)
        {
            dispcol = len;
        }
        else
        {
            dispcol = currcol;
        }
        if (dispcol < 1)
        {
            dispcol = 1;
        }
    }
}

int NumDigits(int num)
{
    int retval = 1;
    while (num > 9)
    {
        retval++;
        num = num / 10;
    }
    return retval;
}

void EraseStatusLine(void)
{
    int i;
    if (pstflag)
    {
        MoveCursorPST(vlines + 1, 1);
        putchar(PST_erase_line);
    }
    else if (ansiflag)
    {
        MoveCursorANSI(vlines + 1, 1);
        fputs(ANSI_erase_line, stdout);
    }
    else
    {
        i = 0;
        while (i < statuslen)
        {
            putchar(8);
            i++;
        }
        i = 0;
        while (i < statuslen)
        {
            putchar(' ');
            i++;
        }
        i = 0;
        while (i < statuslen)
        {
            putchar(8);
            i++;
        }
    }
}

void ExecuteViCommand(int cmd)
{
    int temp2, index, val;
    char *inptr;
    int temp;
    LineT *link;
    int i, len;
    LineT *curr;
    inptr = inbuf;
    temp = currline;
    val = 0;
    // Check for repeat command
    if (cmd == '.')
    {
        if (!lastcmd)
        {
            return;
        }
        cmd = lastcmd;
        if (!count)
        {
            count = lastcnt;
        }
        if (cmd == 'd')
        {
            cmd0 = 'd';
        }
        else if ( cmd == 'r')
        {
            val = lastval;
        }
        else
        {
            cmd0 = 0;
        }
    }
    switch (cmd)
    {
        case 'Q' :
            vmode = 0;
            count = 0;
            schange = 1;
            prompt = ':';
            EraseStatusLine();
            break;
        case 'Y' :
            if (count <= 1)
            {
                temp2 = currline;
            }
            else
            {
                temp2 = currline + count - 1;
            }
            if (temp2 > numlines)
            {
                temp2 = numlines;
            }
            YankLines(currline, temp2);
            lastcmd = cmd;
            lastcnt = count;
            count = 0;
            break;
        case 'p' :
            currline = PushLines(currline);
            schange = (currline != temp);
            lastcmd = cmd;
            lastcnt = count;
            count = 0;
            break;
        case ':' :
            vmode = - 1;
            count = 0;
            schange = 1;
            prompt = cmd;
            EraseStatusLine();
            break;
        case '/' :
        case '?' :
            srchdir = (cmd == '/');
            EraseStatusLine();
            putchar(cmd);
            gets(inptr);
            VisualSearchString(inptr, srchdir);
            count = 0;
            break;
        case 'n' :
            *inptr = 0;
            VisualSearchString(inptr, srchdir);
            count = 0;
            break;
        case 'N' :
            *inptr = 0;
            VisualSearchString(inptr, ! srchdir);
            count = 0;
            break;
        case 13 :
        case 'j' :
            if (currline < numlines)
            {
                currline++;
                if (ansiflag)
                {
                    schange = 5;
                }
                else
                {
                    schange = 3;
                }
                curr = GetLinePtr(currline, &curr);
                if (curr)
                {
                    len = strlen(curr->str);
                    if (currcol > len)
                    {
                        dispcol = len;
                    }
                    else
                    {
                        dispcol = currcol;
                    }
                    if (dispcol < 1)
                    {
                        dispcol = 1;
                    }
                }
            }
            count = 0;
            break;
        case 'k' :
            if (currline > 1)
            {
                currline--;
                if (ansiflag)
                {
                    schange = 5;
                }
                else
                {
                    schange = 4;
                }
                curr = GetLinePtr(currline, &curr);
                if (curr)
                {
                    len = strlen(curr->str);
                    if (currcol > len)
                    {
                        dispcol = len;
                    }
                    else
                    {
                        dispcol = currcol;
                    }
                    if (dispcol < 1)
                    {
                        dispcol = 1;
                    }
                }
            }
            count = 0;
            break;
        case 'h' :
            if (dispcol > 1)
            {
                dispcol--;
                currcol = dispcol;
                if (ansiflag)
                {
                    schange = 5;
                }
                else
                {
                    schange = 2;
                }
            }
            count = 0;
            break;
        case 'l' :
            curr = GetLinePtr(currline, &curr);
            if (curr)
            {
                len = strlen(curr->str);
            }
            else
            {
                len = 0;
            }
            if (dispcol < len)
            {
                dispcol++;
                currcol = dispcol;
                if (ansiflag)
                {
                    schange = 5;
                }
                else
                {
                    schange = 2;
                }
            }
            count = 0;
            break;
        case 6 :
            // ^F
            currline += vlines;
            if (currline > numlines)
            {
                currline = numlines;
            }
            schange = (currline != temp);
            SetDispCol();
            count = 0;
            break;
        case 2 :
            // ^B
            currline -= vlines;
            if (currline < 1)
            {
                currline = 1;
            }
            schange = (currline != temp);
            SetDispCol();
            count = 0;
            break;
        case 4 :
            // ^D
            currline += vlines / 2;
            if (currline > numlines)
            {
                currline = numlines;
            }
            schange = (currline != temp);
            SetDispCol();
            count = 0;
            break;
        case 21 :
            // ^U
            currline -= vlines / 2;
            if (currline < 1)
            {
                currline = 1;
            }
            schange = (currline != temp);
            SetDispCol();
            count = 0;
            break;
        case 'G' :
            if (count >= 1 &&count <= numlines)
            {
                currline = count;
            }
            else if (count == 0)
            {
                currline = numlines;
            }
            schange = (currline != temp);
            count = 0;
            SetDispCol();
            break;
        case 'J' :
            if (count <= 1)
            {
                temp2 = currline + 1;
            }
            else
            {
                temp2 = currline + count;
            }
            if (temp2 > numlines)
            {
                temp2 = numlines;
            }
            currline = JoinLines(currline, temp2);
            lastcmd = cmd;
            lastcnt = count;
            count = 0;
            cmd0 = 0;
            schange = 1;
            break;
        case 'd' :
            if (cmd0 == 'd')
            {
                if (count <= 1)
                {
                    temp2 = currline;
                }
                else
                {
                    temp2 = currline + count - 1;
                }
                if (temp2 > numlines)
                {
                    temp2 = numlines;
                }
                currline = DeleteLines(currline, temp2);
                lastcmd = cmd;
                lastcnt = count;
                count = 0;
                cmd0 = 0;
                schange = 1;
                SetDispCol();
                return;
            }
            break;
        case 'x' :
            if ((curr = GetLinePtr(currline, &curr)))
            {
                inptr = curr->str;
                len = strlen(inptr);
                lastcmd = cmd;
                lastcnt = count;
                if (count <= 0)
                {
                    count = 1;
                }
                if (len)
                {
                    changed = 1;
                    index = dispcol - 1;
                    if (dispcol + count - 1 > len)
                    {
                        count = len - dispcol + 1;
                    }
                    len -= count;
                    while (index <= len)
                    {
                        inptr[index] = inptr[index + count];
                        index++;
                    }
                    schange = 2;
                    if (dispcol > len && len > 0)
                    {
                        dispcol = len;
                    }
                    currcol = dispcol;
                }
                count = 0;
            }
            break;
        case 'r' :
            if ((curr = GetLinePtr(currline, &curr)))
            {
                inptr = curr->str;
                len = strlen(inptr);
                if (!val)
                {
                    val = getchar();
                }
                lastcmd = cmd;
                lastcnt = count;
                lastval = val;
                if (val != 27)
                {
                    schange = 2;
                    changed = 1;
                    if (len)
                    {
                        index = dispcol - 1;
                        inptr[index] = val;
                    }
                    else
                    {
                        link = AllocateString(" ");
                        link->str[0] = val;
                        InsertLine(link, currline);
                        DeleteLine(currline + 1);
                    }
                    currcol = dispcol;
                }
            }
            count = 0;
            break;
        case 'O' :
        case 'A' :
        case 'I' :
        case 'i' :
        case 'a' :
            count = 0;
            changed = 1;
            if (numlines == 0)
            {
                index = 0;
                inbuf[0] = 0;
                len = 0;
                currline = 1;
                schange = 1;
            }
            else if (cmd == 'O')
            {
                index = 0;
                inbuf[0] = 0;
                len = 0;
                schange = 1;
            }
            else
            {
                link = GetLinePtr(currline, &link);
                strcpy(inbuf, link->str);
                len = strlen(inbuf);
                if (cmd == 'A')
                {
                    index = len;
                }
                else
                {
                    if (len == 0 || cmd == 'I')
                    {
                        index = 0;
                    }
                    else
                    {
                        index = dispcol;
                        if (cmd == 'i')
                        {
                            index--;
                        }
                    }
                }
                DeleteLine(currline);
                schange = 2;
            }
            currcol = dispcol = index + 1;
            while (1)
            {
                link = AllocateString(inbuf);
                InsertLine(link, currline);
                PrintScreen(currline);
                val = getchar();
                if (val == 27)
                {
                    if (len > 0 &&index >= len)
                    {
                        index--;
                        schange = 2;
                        dispcol = currcol = index + 1;
                    }
                    break;
                }
                if (val == 8)
                {
                    if (index > 0)
                    {
                        index--;
                        i = index;
                        while (i < len)
                        {
                            inbuf[i] = inbuf[i + 1];
                            i++;
                        }
                        len--;
                    }
                    DeleteLine(currline);
                    schange = 2;
                }
                else if (val == 13)
                {
                    if (index == 0)
                    {
                        link = AllocateString(&nullstr);
                        InsertLine(link, currline);
                        currline++;
                        DeleteLine(currline);
                    }
                    else
                    {
                        i = len + 1;
                        while (i > index)
                        {
                            inbuf[i] = inbuf[i - 1];
                            i--;
                        }
                        inbuf[index ++] = 0;
                        len++;
                        link = AllocateString(inbuf);
                        InsertLine(link, currline);
                        currline++;
                        DeleteLine(currline);
                        len -= index;
                        i = 0;
                        while (i <= len)
                        {
                            inbuf[i] = inbuf[i + index];
                            i++;
                        }
                        index = 0;
                    }
                    schange = 1;
                }
                else
                {
                    i = len + 1;
                    while (i > index)
                    {
                        inbuf[i] = inbuf[i - 1];
                        i--;
                    }
                    inbuf[index ++] = val;
                    len++;
                    DeleteLine(currline);
                    schange = 2;
                }
                dispcol = currcol = index + 1;
            }
            break;
        case '$' :
            curr = GetLinePtr(currline, &curr);
            if (curr)
            {
                len = strlen(curr->str);
                if (!len)
                {
                    len++;
                }
                if (len != dispcol)
                {
                    dispcol = len;
                    currcol = dispcol;
                    schange = 1;
                }
            }
            count = 0;
            break;
        default :
            if (isdigit(cmd))
            {
                if (count == 0 && cmd == '0')
                {
                    if (dispcol != 1)
                    {
                        dispcol = 1;
                        currcol = dispcol;
                        schange = 1;
                    }
                }
                count = (count * 10) + cmd - '0';
            }
            break;
    }
    cmd0 = cmd;
}

int FindNextString(int dir, int newline)
{
    int retval = 0;
    LineT *curr;
    LineT *prev;
    char *lineptr;
    int len;
    int col;

    if ((curr = GetLinePtr(currline, &prev)))
    {
        lineptr = curr->str;
        len = strlen(lineptr);
        if (dir)
        {
            if (newline)
            {
                dispcol = currcol = 1;
            }
            if (dispcol < len)
            {
                col = FindString(lineptr + dispcol, srchstr);
                if (col)
                    retval = dispcol + col;
            }
        }
        else
        {
            if (newline)
            {
                dispcol = currcol = len + 1;
            }
            if (dispcol > 1)
            {
                col = FindStringBackward(lineptr, srchstr, dispcol - 1);
                if (col)
                    retval = col;
            }
        }
    }
    return retval;
}

void VisualSearchString(char *ptr, int dir)
{
    int newline, newcol;
    if (*ptr)
    {
        strcpy(srchstr, ptr);
    }
    // Search current line
    if ((newcol = FindNextString(dir, 0)))
    {
        if (ansiflag)
        {
            schange = 5;
        }
        else
        {
            schange = 1;
        }
        currcol = dispcol = newcol;
        return;
    }
    // Search other lines
    strcpy(ptr, "//");
    if (dir)
        newline = SearchStringForward(&ptr, currline);
    else
        newline = SearchStringBackward(&ptr, currline);
    // Search for first occurance on new line
    if (newline > 0 && newline != currline)
    {
        if (ansiflag)
        {
            schange = 5;
        }
        else
        {
            schange = 1;
        }
        currline = newline;
        if ((newcol = FindNextString(dir, 1)))
        {
            currcol = dispcol = newcol;
        }
    }
    else
    {
        if (ansiflag)
        {
            schange = 5;
        }
        else
        {
            schange = 2;
        }
    }
}

//******************************************************************************
// String Allocation Routines
// Copyright (c) 2011, Dave Hein
// See end of file for terms of use.
//******************************************************************************
void smallocinit(char *ptr, int size)
{
    LineT *curr = (LineT *)ptr;
    smfreelist = curr;
    curr->next = 0;
    curr->size = size;
}

// Allocate a block of "size" bytes.  Return a pointer to the block if
// successful, or zero if a large enough memory block could not be found
LineT *smalloc(int size)
{
    LineT *prev;
    LineT *curr;
    LineT *next;
    //printf("smalloc: %d\n", size);
    if (size <= 0)
    {
        return 0;
    }
    // Adjust size to nearest long plus the header size
    size = ((size + 3) &(~ 3)) + HDR_SIZE;
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
    if (curr == 0)
    {
        printf("Out of memory!\n");
        return 0;
    }
    // Get next pointer
    next = curr->next;
    // Split block if larger than needed
    if (curr->size >= size + HDR_SIZE + 16)
    {
        next = curr + size;
        next->next = curr->next;
        next->size = curr->size - size;
        curr->next = next;
        curr->size = size;
    }
    // Remove block from the free list
    curr->next = 0;
    if (prev)
    {
        prev->next = next;
    }
    else
    {
        smfreelist = next;
    }
    return curr;
}

// Insert a memory block back into the free list.  Merge blocks together if
// the memory block is contiguous with other blocks on the list.
void sfree(LineT *curr)
{
    LineT *prev;
    LineT *next;
    prev = 0;
    next = smfreelist;
    // Find Insertion Point
    while (next)
    {
        if (curr >= prev && curr <= next)
        {
            break;
        }
        prev = next;
        next = next->next;
    }
    // Merge with the previous block if contiguous
    if (prev && (prev + prev->size) == curr)
    {
        prev->size += curr->size;
        // Also merge with next block if contiguous
        if ((prev + prev->size) == next)
        {
            prev->size += next->size;
            prev->next = next->next;
        }
    }
    // Merge with the next block if contiguous
    else if (next && (curr + curr->size) == next)
    {
        curr->size += next->size;
        curr->next = next->next;
        if (prev)
        {
            prev->next = curr;
        }
        else
        {
            smfreelist = curr;
        }
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
+----------------------------------------------------------------------------+
|                          TERMS OF USE: MIT License                         |
+----------------------------------------------------------------------------+
|Permission is hereby granted, free of charge, to any person obtaining a copy|
|of this software and associated documentation files (the "Software"), to    |
|deal in the Software without restriction, including without limitation the  |
|rights to use, copy, modify, merge, publish, distribute, sublicense, and/or |
|sell copies of the Software, and to permit persons to whom the Software is  |
|furnished to do so, subject to the following conditions:                    |
|                                                                            |
|The above copyright notice and this permission notice shall be included in  |
|all copies or substantial portions of the Software.                         |
|                                                                            |
|THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  |
|IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    |
|FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE |
|AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      |
|LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     |
|FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS|
|IN THE SOFTWARE.                                                            |
+----------------------------------------------------------------------------+
*/
