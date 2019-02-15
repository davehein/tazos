//******************************************************************************
// Copyright (c) 2011 - 2014 Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __P2GCC__
#include <propeller.h>
#endif

#define TYPE_NOUN 1
#define TYPE_VERB 2
#define TYPE_PRONOUN 3
#define TYPE_ADJECT 4
#define TYPE_OTHER 5
#define TYPE_NAME 6
#define TYPE_ARTICLE 7
#define TYPE_QUERY 8
#define TYPE_RELATED 32
#define TYPE_EQUATE 33
#define TYPE_RELATE2 34
#define TYPE_MASK 0x3f
#define DELETE_MASK 0x40
#define MEMORY_MASK 31
#define BUF_SIZE 10000
#define VOCAB_SIZE 30000

#define clkfreq 80000000

void  start(int argc, char **argv);
int   rand(void);
int   kbhit(void);
int   GetTicks(void);
int   toupper(int val);
char *SkipChar(char *str, int val);
char *FindChar(char *str, int val);
int   tokenize(char *str, char **tokens);
char *FindWord(char *str);
void  ResetVocab(void);
void  ResetBasicVocab(void);
void  DumpHexWords(void);
void  DumpAsciiWords(char *name, int num);
void  AddWord(int attrib, char *str);
int   UnknownWord(char **tokens, int num, int i);
void  DeleteWord(char **tokens, int num);
void  SomethingWrong(char *str);
void  AddEquate(char *ptr1, char *ptr2);
char *GetNextItem(char *ptr);
char *GetNextEquate(char *ptr);
int   Match(char *ptr1, char *ptr2);
int   RemoveArticles(char **tokens, int num);
char *FindWords(char *word1, char *word2);
int   isvowel(int val);
void  AddToMemory(char *name, char *ptr1);
void  PrintMemory(void);
int   CheckMemory(char *ptr0, char *ptr1);
int   GetPlural(char *name);
char *GetVerb(int plural, char *ptr1);
void  PrintStatement(char *name, char *ptr1, int remember);
void  PrintQuestion(char *ptr1);
void  Process(char **tokens, int num, int question);
void  WaitForInput(void);
void  SaveVocab(char **tokens, int num);
int   CheckCommand(char **tokens, int num, char *buffer);
//void  config_linefeed(int argc, char **argv);
int   main(int argc, char **argv);
FILE *OpenFile(char **tokens, int num, char *command, char *mode);

// Global variables
FILE *speak = 0;
int seed = 1;
int debugmode = 0;
int changeflag = 0;
int MemoryIndex = 0;
char *types[] = {"noun", "verb", "pronoun", "adjective",
        "other", "name", "article", "query"};

// Basic vocabulary
static int basicsize = 249;
char basicvocab[] = {
    0x03, 0x49, 0x00, 0x00, 0x19, 0x02, 0x61, 0x6d, 0x00, 0x00, 0x6b, 0x07, 0x61, 0x00, 0x00, 0x00,
    0x01, 0x68, 0x75, 0x6d, 0x61, 0x6e, 0x00, 0x00, 0x00, 0x20, 0x00, 0x05, 0x00, 0x10, 0x00, 0x00,
    0x03, 0x79, 0x6f, 0x75, 0x00, 0x00, 0x38, 0x02, 0x61, 0x72, 0x65, 0x00, 0x00, 0x70, 0x06, 0x4c,
    0x65, 0x72, 0x6e, 0x65, 0x72, 0x00, 0x00, 0x50, 0x20, 0x00, 0x27, 0x00, 0x2e, 0x00, 0x61, 0x02,
    0x69, 0x73, 0x00, 0x00, 0x75, 0x01, 0x70, 0x72, 0x6f, 0x67, 0x72, 0x61, 0x6d, 0x00, 0x00, 0x00,
    0x20, 0x00, 0x3f, 0x00, 0x45, 0x00, 0x66, 0x02, 0x65, 0x71, 0x75, 0x61, 0x6c, 0x73, 0x00, 0x01,
    0x17, 0x21, 0x00, 0x2e, 0x00, 0x00, 0x21, 0x00, 0x20, 0x00, 0x00, 0x21, 0x00, 0x27, 0x00, 0x7a,
    0x21, 0x00, 0x05, 0x01, 0x04, 0x21, 0x00, 0x05, 0x00, 0x7f, 0x21, 0x00, 0x3f, 0x00, 0x00, 0x21,
    0x00, 0x27, 0x00, 0x00, 0x21, 0x00, 0x3f, 0x00, 0x00, 0x02, 0x65, 0x71, 0x75, 0x61, 0x6c, 0x00,
    0x01, 0x12, 0x21, 0x00, 0x57, 0x00, 0x00, 0x21, 0x01, 0x09, 0x00, 0x00, 0x08, 0x77, 0x68, 0x6f,
    0x00, 0x01, 0x2b, 0x08, 0x77, 0x68, 0x61, 0x74, 0x00, 0x01, 0x30, 0x21, 0x01, 0x23, 0x00, 0x00,
    0x21, 0x01, 0x1c, 0x00, 0x00, 0x01, 0x63, 0x6f, 0x6d, 0x70, 0x75, 0x74, 0x65, 0x72, 0x00, 0x01,
    0x49, 0x02, 0x72, 0x75, 0x6e, 0x73, 0x00, 0x01, 0x74, 0x20, 0x01, 0x41, 0x00, 0x45, 0x00, 0x00,
    0x02, 0x64, 0x6f, 0x65, 0x73, 0x00, 0x01, 0x6a, 0x02, 0x72, 0x75, 0x6e, 0x00, 0x01, 0x6f, 0x02,
    0x64, 0x6f, 0x00, 0x01, 0x65, 0x21, 0x01, 0x50, 0x00, 0x00, 0x21, 0x01, 0x5f, 0x00, 0x00, 0x21,
    0x01, 0x41, 0x00, 0x00, 0x21, 0x01, 0x58, 0x00, 0x00};

// ****************************************************
// Replace the following lines with the dumped output
// ****************************************************
int version = 2;
int size = 0;
char *vocabptr = 0;

char *Memory[MEMORY_MASK + 1];

// Return the system tick count
int GetTicks(void)
{
#ifdef __P2GCC__
    return CNT;
#elif defined(CSPIN_FLAG)
    return cnt;
#else
    return 0;
#endif
}

// Convert ASCII character to upper case
int toupper( int val)
{
    if (val >= 'a' &&val <= 'z')
    {
        val -= 'a' - 'A';
    }
    return val;
}

// Find the next character that does not match val
char *SkipChar(char *str, int val)
{
    while (*str)
    {
        if (*str != val) break;
        str++;
    }
    return str;
}

// Find the next character that matches val
char *FindChar(char *str, int val)
{
    while (*str)
    {
        if (*str == val) break;
        str++;
    }
    return str;
}

// Convert the space-delimited string to a list of tokens
int tokenize(char *str, char **tokens)
{
    char *ptr;
    int num, len, val;
    num = 0;
    len = strlen(str);
    val = str[len - 1];
    if (len == 0)
    {
        return 0;
    }
    if (val < 'A' || (val > 'Z' &&val < 'a') || val > 'z')
    {
        str[len + 1] = 0;
        str[len] = str[len - 1];
        str[len - 1] = ' ';
    }
    while (*str)
    {
        str = SkipChar(str, ' ');
        if (*str == 0)
        {
             break;
        }
        tokens[num ++] = str;
        ptr = FindChar(str, ' ');
        if (*ptr) *ptr++ = 0;
        str = ptr;
    }
    return num;
}

// Find the word "str" in the vocab table
char *FindWord(char *str)
{
    char *ptr;
    ptr = vocabptr;
    while (ptr < vocabptr + size)
    {
        if ((*ptr & TYPE_MASK) == TYPE_RELATED)
        {
            ptr += 7;
        }
        else if ((*ptr & TYPE_MASK) == TYPE_RELATE2)
        {
            ptr += 9;
        }
        else if ((*ptr & TYPE_MASK) == TYPE_EQUATE)
        {
            ptr += 5;
        }
        else
        {
            if ((*ptr & DELETE_MASK) == 0)
            {
                if (strcmp(str, ptr + 1) == 0)
                {
                    return ptr;
                }
            }
            ptr += strlen(ptr) + 3;
        }
    }
    return 0;
}

// Reset the vocab table
void ResetVocab(void)
{
    char buf[100];
    printf("< Ary you sure you want to forget everything?\n");
    printf("> ");
    gets(buf);
    if (toupper(*buf) == 'Y')
    {
        size = 0;
        changeflag = 1;
        Memory[0] = 0;
        MemoryIndex = 0;
        printf("< Memory erased\n");
    }
    else
    {
        printf("< Reset aborted\n");
    }
}

// Reset to a basic vocab table
void ResetBasicVocab(void)
{
    char buf[100];
    printf("< Ary you sure you want to reset to a basic word list?\n");
    printf("> ");
    gets(buf);
    if (toupper(*buf) == 'Y')
    {
        size = basicsize;
        memcpy(vocabptr, basicvocab, size);
        changeflag = 1;
        Memory[0] = 0;
        MemoryIndex = 0;
        printf("< Memory reset to basic words\n");
    }
    else
    {
        printf("< Reset aborted\n");
    }
}

// Dump out a Spin formatted version of the vocab table
void DumpHexWords(void)
{
    int i = 0;
    char *ptr = vocabptr;

    printf("  version long %d\n", version);
    printf("  size long %d\n", size);
    printf("  vocab byte\n");
    printf("  byte ");
    while (ptr < vocabptr + size)
    {
        printf("$%2.2x", *ptr++);
        if (ptr == vocabptr + size)
        {
            printf("\n");
        }
        else if ((i ++) % 16 == 15)
        {
            printf("\n  byte ");
        }
        else
        {
            printf(",");
        }
    }
    printf("  byte 0[%d]\n", BUF_SIZE - size);
}

// Dump an ASCII formatted version of the vocab table
void DumpAsciiWords(char *name, int num)
{
    char *ptr1;
    int index, index2, index3;
    char *ptr = vocabptr;

    if (num > 1)
    {
        ptr = FindWord(name);
        if (!ptr)
        {
            printf("$ %s not found\n", name);
            return;
        }
    }
    while (ptr < vocabptr + size)
    {
        if ((*ptr & TYPE_MASK) == TYPE_RELATED)
        {
            ptr += 7;
        }
        else if ((*ptr & TYPE_MASK) == TYPE_RELATE2)
        {
            ptr += 9;
        }
        else if ((*ptr & TYPE_MASK) == TYPE_EQUATE)
        {
            ptr += 5;
        }
        else
        {
            if ((*ptr & DELETE_MASK) == 0)
            {
                printf("$ %s - %s", ptr + 1, types[*ptr - 1]);
                ptr1 = ptr + strlen(ptr) + 1;
                while (ptr1 < vocabptr + size && (*ptr1 || ptr1[1]))
                {
                    index = (*ptr1 << 7) | ptr1[1];
                    ptr1 = vocabptr + index;
                    if ((*ptr1 &TYPE_MASK) == TYPE_RELATED)
                    {
                        if ((*ptr1 & DELETE_MASK) == 0)
                        {
                            index = (ptr1[1] << 7) | ptr1[2];
                            index2 = (ptr1[3] << 7) | ptr1[4];
                            printf(" (%s %s)", vocabptr + index + 1, vocabptr + index2 + 1);
                        }
                        ptr1 += 5;
                    }
                    else if ((*ptr1 &TYPE_MASK) == TYPE_RELATE2)
                    {
                        if ((*ptr1 & DELETE_MASK) == 0)
                        {
                            index = (ptr1[1] << 7) | ptr1[2];
                            index2 = (ptr1[3] << 7) | ptr1[4];
                            index3 = (ptr1[5] << 7) | ptr1[6];
                            printf(" (%s %s %s)", vocabptr + index + 1, vocabptr + index2 + 1, vocabptr + index3 + 1);
                        }
                        ptr1 += 7;
                    }
                    else if ((*ptr1 &TYPE_MASK) == TYPE_EQUATE)
                    {
                        if ((*ptr1 & DELETE_MASK) == 0)
                        {
                            index = (ptr1[1] << 7) | ptr1[2];
                            printf(" (=%s)", vocabptr + index + 1);
                        }
                        ptr1 += 3;
                    }
                    else
                    {
                        printf("$ Invalid type = %2.2x\n", *ptr1);
                    }
                }
                printf("\n");
                if (num > 1)
                {
                    break;
                }
            }
            ptr += strlen(ptr) + 3;
        }
    }
}

// Add a word to the vocab table
void AddWord( int attrib, char *str)
{
    char *ptr = vocabptr + size;

    if (size + strlen(str) + 4 > BUF_SIZE)
    {
        printf("$ Out of memory\n");
        return;
    }
    if (debugmode)
    {
        printf("AddWord %s - %s\n", str, types[attrib - 1]);
    }
    *ptr++ = attrib;
    strcpy(ptr, str);
    ptr += strlen(ptr) + 1;
    *ptr++ = 0;
    *ptr = 0;
    size += strlen(str) + 4;
    changeflag = 1;
}

// Determine the attribute of an unknown word, and add it to the vocab table
int UnknownWord(char **tokens, int num, int i)
{
    char *str = tokens[i];
    char buf[100];
    char *ptr;
    int caseflag = 0;

    if (i == 0)
    {
        if (*str >= 'A' && *str <= 'Z')
        {
            caseflag = 1;
            *str += 'a' - 'A';
            if (FindWord(str))
            {
                return 0;
            }
            *str -= 'a' - 'A';
            AddWord(TYPE_NAME, str);
            return 0;
        }
        else
        {
            if (num > 2)
            {
                if (strcmp(tokens[1], "equal") == 0 || strcmp(tokens[1], "equals") == 0)
                {
                    ptr = FindWord(tokens[2]);
                    if (ptr)
                    {
                        AddWord(*ptr, tokens[0]);
                        return 0;
                    }
                }
                else
                {
                    AddWord(TYPE_ADJECT, tokens[0]);
                    return 0;
                }
            }
        }
    }
    else
    {
        ptr = FindWord(tokens[i - 1]);
        if (ptr == 0)
        {
            SomethingWrong(tokens[i - 1]);
        }
        else
        {
            if (*ptr == TYPE_ARTICLE && (i == 1 || i == num - 1))
            {
                AddWord(TYPE_NOUN, str);
                return 0;
            }
            else if (i < 3 && (*ptr == TYPE_NOUN || *ptr == TYPE_NAME))
            {
                AddWord(TYPE_VERB, str);
                return 0;
            }
            else if ((i > 1 && *ptr == TYPE_VERB) || i > 2)
            {
                if (*str >= 'A' && *str <= 'Z')
                {
                    AddWord(TYPE_NAME, str);
                    return 0;
                }
                else if (i < num - 1 || *ptr == TYPE_VERB)
                {
                    AddWord(TYPE_ADJECT, str);
                    return 0;
                }
            }
            if (i == num - 1 && num >= 4)
            {
                ptr = FindWord(tokens[i - 2]);
                if (ptr &&*ptr == TYPE_ARTICLE)
                {
                    AddWord(TYPE_NOUN, str);
                    return 0;
                }
                else
                {
                    AddWord(TYPE_ADJECT, str);
                    return 0;
                }
            }
        }
    }
    printf("< I don't know %s\n", str);
    while (1)
    {
        printf("< Is it a noun, verb, pronoun, adjective, Name, Article, query or ignore?\n");
        printf("> ");
        gets(buf);
        if (caseflag && *buf != 'N' && strcmp(str, "I"))
        {
            *str += 'a' - 'A';
        }
        if (*buf == 'n')
        {
            AddWord(TYPE_NOUN, str);
            break;
        }
        else if (*buf == 'v')
        {
            AddWord(TYPE_VERB, str);
            break;
        }
        else if (*buf == 'a')
        {
            AddWord(TYPE_ADJECT, str);
            break;
        }
        else if (*buf == 'p')
        {
            AddWord(TYPE_PRONOUN, str);
            break;
        }
        else if (*buf == 'N')
        {
            AddWord(TYPE_NAME, str);
            break;
        }
        else if (*buf == 'A')
        {
            AddWord(TYPE_ARTICLE, str);
            break;
        }
        else if (*buf == 'q')
        {
            AddWord(TYPE_QUERY, str);
            break;
        }
        else if (*buf == 'i')
        {
            return 1;
        }
        printf("I don't understand\n");
    }
    return 0;
}

// Delete a word and all references to it from the vocab table
void DeleteWord( char **tokens, int num)
{
    char *ptr;
    char buf[100];
    int index, index2;

    if (num < 2)
    {
        printf("< I don't know what to forget\n");
        return;
    }
    ptr = FindWord(tokens[1]);
    if (ptr == 0)
    {
        printf("< I don't know %s\n", tokens[1]);
        return;
    }
    while (1)
    {
        printf("< Do you want me to forget %s?\n", tokens[1]);
        printf("> ");
        gets(buf);
        if (*buf == 'y')
        {
            index = ptr - vocabptr;
            // Delete the word
            *ptr |= DELETE_MASK;
            // Delete the chain
            while ((ptr = GetNextItem(ptr)))
            {
                *ptr |= DELETE_MASK;
            }
            // Delete any reference to this word
            ptr = vocabptr;
            while (ptr < vocabptr + size)
            {
                if ((*ptr &TYPE_MASK) == TYPE_EQUATE)
                {
                    if ((*ptr & DELETE_MASK) == 0)
                    {
                        index2 = (ptr[1] << 7) | ptr[2];
                        if (index == index2)
                        {
                            *ptr |= DELETE_MASK;
                        }
                    }
                    ptr += 5;
                }
                else if ((*ptr &TYPE_MASK) == TYPE_RELATED)
                {
                    if ((*ptr & DELETE_MASK) == 0)
                    {
                        index2 = (ptr[1] << 7) | ptr[2];
                        if (index == index2)
                        {
                            *ptr |= DELETE_MASK;
                        }
                        index2 = (ptr[3] << 7) | ptr[4];
                        if (index == index2)
                        {
                            *ptr |= DELETE_MASK;
                        }
                    }
                    ptr += 7;
                }
                else if ((*ptr &TYPE_MASK) == TYPE_RELATE2)
                {
                    if ((*ptr & DELETE_MASK) == 0)
                    {
                        index2 = (ptr[1] << 7) | ptr[2];
                        if (index == index2)
                        {
                            *ptr |= DELETE_MASK;
                        }
                        index2 = (ptr[3] << 7) | ptr[4];
                        if (index == index2)
                        {
                            *ptr |= DELETE_MASK;
                        }
                        index2 = (ptr[5] << 7) | ptr[6];
                        if (index == index2)
                        {
                            *ptr |= DELETE_MASK;
                        }
                    }
                    ptr += 9;
                }
                else
                {
                    ptr += strlen(ptr) + 3;
                }
            }
            changeflag = 1;
            printf("< %s forgotten\n", tokens[1]);
            return;
        }
        else if (*buf == 'n')
        {
            printf("< %s is not forgotten\n", tokens[1]);
            return;
        }
        else
        {
            printf("< I don't understand\n");
        }
    }
}

// Could find a word that was expected.  Print a message.
void SomethingWrong(char *str)
{
    printf("Something's wrong\n");
    printf("I couldn't find %s\n", str);
}

// Equate the first word to the second word
void AddEquate(char *ptr1, char *ptr2)
{
    int index;
    char *ptr3 = ptr1 + strlen(ptr1) + 1;
    if (size + 5 > BUF_SIZE)
    {
        printf("< Out of memory\n");
        return;
    }
    while (ptr3 < vocabptr + size && (*ptr3 || ptr3[1]))
    {
        index = (*ptr3 << 7) | ptr3[1];
        ptr3 = vocabptr + index;
        if ((*ptr3 & TYPE_MASK) == TYPE_EQUATE)
        {
            if ((*ptr3 & DELETE_MASK) == 0)
            {
                index = (ptr3[1] << 7) | ptr3[2];
                if (ptr2 == vocabptr + index)
                {
                    return;
                }
            }
            ptr3 += 3;
        }
        else if ((*ptr3 & TYPE_MASK) == TYPE_RELATED)
        {
            ptr3 += 5;
        }
        else if ((*ptr3 & TYPE_MASK) == TYPE_RELATE2)
        {
            ptr3 += 7;
        }
        else
        {
            printf("Unknown type = %2.2x\n", *ptr3);
        }
    }
    *ptr3 = size >> 7;
    ptr3[1] = size &127;
    ptr3 = vocabptr + size;
    *ptr3++ = TYPE_EQUATE;
    index = ptr2 - vocabptr;
    *ptr3 = index >> 7;
    ptr3[1] = index &127;
    ptr3[2] = 0;
    ptr3[3] = 0;
    size += 5;
    changeflag = 1;
}

// Get the next item in a chain
char *GetNextItem(char *ptr)
{
    int index;
    if (ptr == 0)
    {
        return 0;
    }
    while (ptr < vocabptr + size)
    {
        if ((*ptr &TYPE_MASK) == TYPE_EQUATE)
        {
            ptr += 3;
        }
        else if ((*ptr &TYPE_MASK) == TYPE_RELATED)
        {
            ptr += 5;
        }
        else if ((*ptr &TYPE_MASK) == TYPE_RELATE2)
        {
            ptr += 7;
        }
        else
        {
            ptr += strlen(ptr) + 1;
        }
        index = (*ptr << 7) | ptr[1];
        if (index == 0)
        {
            break;
        }
        ptr = vocabptr + index;
        if ((*ptr & DELETE_MASK) == 0)
        {
            return ptr;
        }
    }
    return 0;
}

// Get the next equate item in a chain
char *GetNextEquate(char *ptr)
{
    while ((ptr = GetNextItem(ptr)))
    {
        if (*ptr == TYPE_EQUATE)
        {
            return ptr;
        }
    }
    return 0;
}

// Match two word entries and their equates in the vocab table
int Match(char *ptr1, char *ptr2)
{
    int index;
    char *ptr3;
    char *name1;
    char *name2;
    name1 = ptr1 + 1;
    while (1)
    {
        ptr3 = ptr2;
        name2 = ptr2 + 1;
        while (1)
        {
            if (strcmp(name1, name2) == 0)
            {
                return 1;
            }
            ptr3 = GetNextEquate(ptr3);
            if (!ptr3)
            {
                break;
            }
            index = (ptr3[1] << 7) | ptr3[2];
            name2 = vocabptr + index + 1;
        }
        ptr1 = GetNextEquate(ptr1);
        if (!ptr1)
        {
            break;
        }
        index = (ptr1[1] << 7) | ptr1[2];
        name1 = vocabptr + index + 1;
    }
    return 0;
}

// Remove articles from a token list
int RemoveArticles(char **tokens, int num)
{
    int i, j;
    char *ptr;
    i = 0;
    while (i < num)
    {
        ptr = FindWord(tokens[i]);
        if (*ptr == 0)
        {
            printf("RemoveArticles: Can't find %s\n", tokens[i]);
            i++;
            continue;
        }
        if (*ptr == TYPE_ARTICLE)
        {
            num--;
            j = i;
            while (j < num)
            {
                tokens[j] = tokens[j + 1];
                j++;
            }
            continue;
        }
        i++;
    }
    return num;
}

// Locate exact matches of a word and a verb in a related field
char *FindWords(char *word1, char *word2)
{
    char *ptr1;
    char *ptr2;
    int index;
    ptr1 = FindWord(word1);
    while (ptr1)
    {
        if (*ptr1 == TYPE_RELATED)
        {
            index = (ptr1[1] << 7) | ptr1[2];
            ptr2 = vocabptr + index;
            if (strcmp(ptr2 + 1, word2) == 0)
            {
                return ptr1;
            }
        }
        ptr1 = GetNextItem(ptr1);
    }
    return 0;
}

// Return true if val is a vowel
int isvowel(int val)
{
    val = toupper(val);
    if (val == 'A' || val == 'E' || val == 'I' || val == 'O' || val == 'U')
    {
        return 1;
    }
    return 0;
}

void AddToMemory(char *name, char *ptr1)
{
    char *ptr0 = FindWord(name);
    if (ptr0 == 0 && *name >= 'A' && *name <= 'Z')
    {
        *name += 'a' - 'A';
        ptr0 = FindWord(name);
        *name -= 'a' - 'A';
        if (ptr0 == 0) return;
    }
    Memory[MemoryIndex] = ptr0;
    Memory[MemoryIndex + 1] = ptr1;
    MemoryIndex = (MemoryIndex + 2) & MEMORY_MASK;
}

void PrintMemory(void)
{
    char *ptr0;
    char *ptr1;
    int index = MemoryIndex;

    while (1)
    {
        ptr0 = Memory[index];
        ptr1 = Memory[index + 1];
        if (ptr0 && ptr1)
        {
            if ((*ptr0 & DELETE_MASK) == 0 && (*ptr1 & DELETE_MASK) == 0)
            {
                PrintStatement(ptr0 + 1, ptr1, 0);
            }
        }
        index = (index + 2) & MEMORY_MASK;
        if (index == MemoryIndex) break;
    }
    printf("$\n");
}

int CheckMemory(char *ptr0, char *ptr1)
{
    char *ptr0a;
    char *ptr1a;
    int index = MemoryIndex;

    while (1)
    {
        ptr0a = Memory[index];
        ptr1a = Memory[index + 1];
        if (ptr0 == ptr0a && ptr1 == ptr1a) return 1;
        index = (index + 2) & MEMORY_MASK;
        if (index == MemoryIndex) break;
    }

    return 0;
}

// Determine whether the word is plural, singular or something else
int GetPlural(char *name)
{
    int len = strlen(name);
    char *ptr1 = FindWord(name);
    //printf("GetPlural %s\n", name);
    if (strcmp(name, "I") == 0)
    {
        return -1;
    }
    if (len < 2)
    {
        return 0;
    }
    if (strcmp(name, "you") == 0 || strcmp(name, "You") == 0)
    {
        return 1;
    }
    if (ptr1 && *ptr1 != TYPE_NOUN && *ptr1 != TYPE_ADJECT)
    {
        return 0;
    }
    if (name[len - 1] == 's' &&name[len - 2] != 's')
    {
        return 1;
    }
    return 0;
}

char *GetVerb(int plural, char *ptr1)
{
    char *verbptr;
    char *isptr = FindWord("is");
    if (isptr && Match(isptr, ptr1))
    {
        if (plural == 1)
        {
            verbptr = "are";
        }
        else if (plural == -1)
        {
            verbptr = "am";
        }
        else
        {
            verbptr = "is";
        }
    }
    else
    {
        verbptr = ptr1 + 1;
    }
    //printf("GetVerb %d %s %s\n", plural, ptr1+1, verbptr);
    return verbptr;
}

// Print a statement formed from the name and the field pointed to by ptr1
// Substitue I for you, and You for I in the name
void PrintStatement(char *name, char *ptr1, int remember)
{
    char *ptr;
    int nullstr;
    char *Astr;
    int index, index2, index3;
    char namestr[100];
    char *astr_0;
    int plural;
    char *verbptr;
    nullstr = 0;
    Astr = (char *)&nullstr;
    astr_0 = (char *)&nullstr;
    if (remember)
    {
        if (strcmp(name, "you") == 0)
        {
            name = "I";
        }
        else if (strcmp(name, "I") == 0)
        {
            name = "you";
        }
    }
    strcpy(namestr, name);
    plural = GetPlural(namestr);
    ptr = FindWord(namestr);
    if (ptr)
    {
        if (*ptr == TYPE_NOUN && plural == 0 && (*ptr1 == TYPE_RELATED || *ptr1 == TYPE_RELATE2))
        {
            if (isvowel(*namestr))
            {
                Astr = "An ";
            }
            else
            {
                Astr = "A ";
            }
        }
    }
    if (!Astr[0] && *namestr >= 'a' && *namestr <= 'z')
    {
        *namestr -= 'a' - 'A';
    }
    if (remember)
    {
        printf("< ");
    }
    else
    {
        printf("$ ");
    }
    if (*ptr1 == TYPE_RELATED)
    {
        index = (ptr1[1] << 7) | ptr1[2];
        index2 = (ptr1[3] << 7) | ptr1[4];
        if (vocabptr[index2] == TYPE_NOUN &&GetPlural(vocabptr + index2 + 1) == 0)
        {
            if (isvowel(vocabptr[index2 + 1]))
            {
                astr_0 = "an ";
            }
            else
            {
                astr_0 = "a ";
            }
        }
        verbptr = GetVerb(plural, vocabptr + index);
        printf("%s%s %s %s%s\n", Astr, namestr, verbptr, astr_0, vocabptr + index2 + 1);
        if (speak)
        {
            fprintf(speak, "%s%s %s %s%s\n", Astr, namestr, verbptr, astr_0, vocabptr + index2 + 1);
        }
    }
    else if (*ptr1 == TYPE_RELATE2)
    {
        index = (ptr1[1] << 7) | ptr1[2];
        index2 = (ptr1[3] << 7) | ptr1[4];
        index3 = (ptr1[5] << 7) | ptr1[6];
        if (vocabptr[index3] == TYPE_NOUN &&GetPlural(vocabptr + index3 + 1) == 0)
        {
            if (isvowel(vocabptr[index2 + 1]))
            {
                astr_0 = "an ";
            }
            else
            {
                astr_0 = "a ";
            }
        }
        verbptr = GetVerb(plural, vocabptr + index);
        printf("%s%s %s %s%s %s\n", Astr, namestr, verbptr, astr_0, vocabptr + index2 + 1, vocabptr + index3 + 1);
        if (speak)
        {
            fprintf(speak, "%s%s %s %s%s %s\n", Astr, namestr, verbptr, astr_0, vocabptr + index2 + 1, vocabptr + index3 + 1);
        }
    }
    else if (*ptr1 == TYPE_EQUATE)
    {
        index = (ptr1[1] << 7) | ptr1[2];
        printf("%s is %s\n", namestr, vocabptr + index + 1);
        if (speak)
        {
            fprintf(speak, "%s is %s\n", namestr, vocabptr + index + 1);
        }
    }
    else
    {
        printf("%s is a %s\n", namestr, types[*ptr1 - 1]);
        if (speak)
        {
            fprintf(speak, "%s is a %s\n", namestr, types[*ptr1 - 1]);
        }
    }
    if (remember)
    {
        AddToMemory(namestr, ptr1);
    }
}

void PrintQuestion(char *ptr1)
{
    int nullstr;
    char *Astr;
    char *isptr;
    int plural;
    char *verbptr;
    nullstr = 0;
    Astr = (char *)&nullstr;
    isptr = FindWord("is");
    plural = GetPlural(ptr1 + 1);
    verbptr = GetVerb(plural, isptr);
    if (*ptr1 == TYPE_NOUN && plural != 1)
    {
        if (isvowel(ptr1[1]))
        {
            Astr = "an ";
        }
        else
        {
            Astr = "a ";
        }
    }
    printf("< What %s %s%s?\n", verbptr, Astr, ptr1 + 1);
    if (speak)
    {
        fprintf(speak, "What %s %s%s\n", verbptr, Astr, ptr1 + 1);
    }
}

#if 1
// Process a list of tokens received from the user
void Process(char **tokens, int num, int question)
{
    int i, index, index2, index3;
    char *ptr[4];
    char *ptr1;
    char *ptr0;
    char *ptrx; 
    int count, counta;
    char *ptry;
    char *ptrya;
    int match_0, equalflag, tempflag;

#if 1
    match_0 = 0;
    equalflag = 0;
    tempflag = 0;
    // Equate the two words if it's an equal statement
    if (num == 3)
    {
        if (!question && (strcmp(tokens[1], "equal") == 0 || strcmp(tokens[1], "equals") == 0))
        {
            if (debugmode)
            {
                printf("equals mode\n");
            }
            equalflag = 1;
            ptr0 = FindWord(tokens[0]);
            ptr1 = FindWord(tokens[2]);
            if (!ptr0)
            {
                SomethingWrong(tokens[0]);
                return;
            }
            if (!ptr1)
            {
                SomethingWrong(tokens[2]);
                return;
            }
            AddEquate(ptr0, ptr1);
            AddEquate(ptr1, ptr0);
            return;
        }
    }
    // Check if the first word is do/does/did.
    // If so, remove the first word and make it a question.
    if ((ptr0 = FindWord("do")))
    {
        if (debugmode)
        {
            printf("does query\n");
        }
        ptr1 = FindWord(tokens[0]);
        if (!ptr1)
        {
            SomethingWrong(tokens[0]);
            return;
        }
        if (Match(ptr0, ptr1))
        {
            question = 1;
            num--;
            i = 0;
            while (i < num)
            {
                tokens[i] = tokens[i + 1];
                i++;
            }
        }
    }
    // Remove the articles
    num = RemoveArticles(tokens, num);
    // There must be three items in the list or it is too short or too long
    if (num < 3)
    {
        printf("I don't understand\n");
        printf("Use more words\n");
        return;
    }
    if (num > 4)
    {
        printf("I don't understand\n");
        printf("Use less words\n");
        return;
    }
    // Locate the tokens in the vocab table
    i = 0;
    while (i < num)
    {
        ptr[i] = FindWord(tokens[i]);
        if (ptr[i] == 0)
        {
            SomethingWrong(tokens[i]);
            return;
        }
        i++;
    }
    // Change the word type if commanded to do so
    if (num == 3 && strcmp(tokens[1], "is") == 0)
    {
        if (strcmp(tokens[2], "noun") == 0)
        {
            *ptr[0] = TYPE_NOUN;
            return;
        }
        else if (strcmp(tokens[2], "name") == 0)
        {
            *ptr[0] = TYPE_NAME;
            return;
        }
        else if (strcmp(tokens[2], "adjective") == 0)
        {
            *ptr[0] = TYPE_ADJECT;
            return;
        }
        else if (strcmp(tokens[2], "verb") == 0)
        {
            *ptr[0] = TYPE_VERB;
            return;
        }
    }
#endif
    // If the first word is a query, the second word must be is/am/are
    if (*ptr[0] == TYPE_QUERY)
    {
#if 1
        if (debugmode)
        {
            printf("query mode\n");
        }
        question = 1;
        ptr1 = FindWord("is");
        tempflag = 0;
        //if (num != 3 || !ptr1 || !Match(ptr1, ptr[1]))
        if (num == 3)
        {
            if (ptr1)
            {
                if (Match(ptr1, ptr[1]))
                {
                    tempflag = 1;
                }
            }
        }
        if (!tempflag)
        {
            printf("I don't understand\n");
            return;
        }
        // Scan related entries and equate's related entries
        ptr1 = ptr[2];
        ptr0 = ptr1;
        ptrx = ptr1;
        ptry = ptr1;
        count = 0;
        // variables for items not in short term memory
        ptrya = 0;
        counta = 0;
        if (debugmode)
        {
            PrintStatement(tokens[2], ptr1, 0);
        }
        while (1)
        {
            while (1)
            {
                ptr1 = GetNextItem(ptr1);
                if (!ptr1)
                {
                    break;
                }
                if (*ptr1 != TYPE_RELATED &&*ptr1 != TYPE_RELATE2)
                {
                    continue;
                }
                //index = (ptr1[1] << 7) | ptr1[2];
                //index2 = (ptr1[3] << 7) | ptr1[4];
                if (debugmode)
                {
                    PrintStatement(tokens[2], ptr1, 0);
                }
                if (CheckMemory(ptr[2], ptr1) == 0)
                {
                    counta++;
                    if ((rand()% counta) == 0)
                    {
                        ptrya = ptr1;
                    }
                }
                if (ptrya == 0)
                {
                    count++;
                    if ((rand()% count) == 0)
                    {
                        ptry = ptr1;
                    }
                }
            }
            if (ptrya == 0)
            {
                ptrya = ptry;
            }
            ptr0 = GetNextEquate(ptr0);
            if (!ptr0)
            {
                break;
            }
            index = (ptr0[1] << 7) | ptr0[2];
            ptrx = vocabptr + index;
            ptr1 = ptrx;
        }
        if (ptrya)
        {
            if (*ptrya == TYPE_RELATED || *ptrya == TYPE_RELATE2 || ptrya[1] == TYPE_EQUATE)
            {
                PrintStatement(tokens[2], ptrya, 1);
            }
            else
            {
                printf("< I don't know\n");
            }
        }
        return;
#endif
    }
#if 1
    // If not an equal statement or a question check if first word is a verb
    // Is so, swap the first and second tokens
    if (!equalflag && !question && *ptr[0] == TYPE_VERB)
    {
        if (debugmode)
        {
            printf("Lead verb query\n");
        }
        question = 1;
        ptr1 = ptr[0];
        ptr[0] = ptr[1];
        ptr[1] = ptr1;
        ptr1 = tokens[0];
        tokens[0] = tokens[1];
        tokens[1] = ptr1;
    }
    // Look for a match to the three words.  If it is a question and
    // a match is found print "Yes"
    ptr1 = ptr[0];
    ptr0 = ptr1;
    ptrx = ptr1;
    while (1)
    {
        while (1)
        {
            ptr1 = GetNextItem(ptr1);
            if (!ptr1)
            {
                break;
            }
            if (*ptr1 != TYPE_RELATED &&*ptr1 != TYPE_RELATE2)
            {
                continue;
            }
            index = (ptr1[1] << 7) | ptr1[2];
            index2 = (ptr1[3] << 7) | ptr1[4];
            tempflag = Match(ptr[1], vocabptr + index) &&Match(ptr[2], vocabptr + index2);
            if (num == 4)
            {
                index3 = (ptr1[5] << 7) | ptr1[6];
                tempflag = tempflag &&Match(ptr[3], vocabptr + index3);
                if (debugmode)
                {
                    printf("%s (%s,%s) (%s,%s) (%s,%s)\n", ptrx + 1, ptr[1] + 1, vocabptr + index + 1, ptr[2] + 1, vocabptr + index2 + 1, ptr[3], vocabptr + index3 + 1);
                }
            }
            else
            {
                if (debugmode)
                {
                    printf("%s (%s,%s) (%s,%s)\n", ptrx + 1, ptr[1] + 1, vocabptr + index + 1, ptr[2] + 1, vocabptr + index2 + 1);
                }
            }
            if (tempflag)
            {
                if (!question)
                {
                    printf("< I know that\n");
                }
                else
                {
                    printf("< Yes\n");
                }
                return;
            }
        }
        ptr0 = GetNextEquate(ptr0);
        if (!ptr0)
        {
            break;
        }
        index = (ptr0[1] << 7) | ptr0[2];
        ptrx = vocabptr + index;
        ptr1 = ptrx;
    }
#endif
    // Print "No" if it was a question and no match found
    if (question)
    {
        printf("< No\n");
        return;
    }
#if 1
    // Add relationship if there was no match and it's not a question
    if (!match_0)
    {
        if (size + 1 + (num << 1) > BUF_SIZE)
        {
            printf("< Out of memory\n");
            return;
        }
        ptr1 = ptr[0];
        while ((ptr0 = GetNextItem(ptr1)))
        {
            ptr1 = ptr0;
        }
        if (*ptr1 == TYPE_RELATED)
        {
            ptr1 += 5;
        }
        else if (*ptr1 == TYPE_RELATE2)
        {
            ptr1 += 7;
        }
        else if (*ptr1 == TYPE_EQUATE)
        {
            ptr1 += 3;
        }
        else
        {
            ptr1 += strlen(ptr1) + 1;
        }
        *ptr1 = size >> 7;
        ptr1[1] = size & 127;
        ptr1 = vocabptr + size;
        if (num == 3)
        {
            *ptr1 = TYPE_RELATED;
        }
        else
        {
            *ptr1 = TYPE_RELATE2;
        }
        index = ptr[1] - vocabptr;
        ptr1[1] = index >> 7;
        ptr1[2] = index & 127;
        index = ptr[2] - vocabptr;
        ptr1[3] = index >> 7;
        ptr1[4] = index & 127;
        if (num == 3)
        {
            ptr1[5] = 0;
            ptr1[6] = 0;
            size += 7;
        }
        else
        {
            index = ptr[3] - vocabptr;
            ptr1[5] = index >> 7;
            ptr1[6] = index & 127;
            ptr1[7] = 0;
            ptr1[8] = 0;
            size += 9;
        }
        changeflag = 1;
        // Check if predicate is undefined
        index = 0;
        ptr1 = ptr[2];
        if (*ptr1 < 32)
        {
            ptr0 = ptr1 + strlen(ptr1) + 1;
            index = (*ptr0 << 7) | ptr0[1];
        }
        if (index == 0)
        {
            PrintQuestion(ptr1);
        }
        else
        {
            printf("< I will remember that\n");
        }
    }
#endif
}
#endif

#ifndef __P2GCC__
int kbhit(void)
{
    return 1;
}
#endif

// Wait from 3 to 10 seconds for input.  Print a random fact from
// the vocab table if no input received.
void WaitForInput(void)
{
    int waittime, starttime;
    char *ptr1;
    char *ptr2;
    char *ptr1a;
    char *ptr2a;
    int count, FirstTime;
    waittime = ((rand() % 8) + 3) * clkfreq;
    starttime = GetTicks();
    printf("> ");
    while (!kbhit())
    {
        // Scan all the entries in the vocab table
        if (GetTicks()- starttime > waittime)
        {
            count = 0;
            ptr1 = vocabptr;
            ptr1a = ptr2a = 0;
            while (ptr1 < vocabptr + size)
            {
                if ((*ptr1 & TYPE_MASK) == TYPE_RELATED)
                {
                    ptr1 += 7;
                }
                else if ((*ptr1 & TYPE_MASK) == TYPE_RELATE2)
                {
                    ptr1 += 9;
                }
                else if ((*ptr1 & TYPE_MASK) == TYPE_EQUATE)
                {
                    ptr1 += 5;
                }
                else if (*ptr1 & DELETE_MASK)
                {
                    ptr1 += strlen(ptr1) + 3;
                }
                else
                {
                    ptr2 = ptr1;
                    FirstTime = 1;
                    while ((ptr2 = GetNextItem(ptr2)) || FirstTime)
                    {
                        FirstTime = 0;
                        if (ptr2 == 0 && *ptr1 != TYPE_NAME && *ptr1 != TYPE_NOUN && *ptr1 != TYPE_ADJECT)
                        {
                            break;
                        }
                        if (ptr2 == 0 || *ptr2 == TYPE_RELATED)
                        {
                            count++;
                            if ((rand()% count) == 0)
                            {
                                if (count == 1 || CheckMemory(ptr1, ptr2) == 0)
                                {
                                    ptr1a = ptr1;
                                    ptr2a = ptr2;
                                }
                            }
                        }
                    }
                    ptr1 += strlen(ptr1) + 3;
                }
            }
            // Print the entry that was randomly selected
            if (ptr1a && ptr2a)
            {
                if (*ptr2a == TYPE_RELATED)
                {
                    putchar(8);
                    putchar(8);
                    PrintStatement(ptr1a + 1, ptr2a, 1);
                    printf("> ");
                }
            }
            else if (ptr1a)
            {
                putchar(8);
                putchar(8);
                PrintQuestion(ptr1a);
                printf("> ");
            }
            waittime = ((rand()% 10) + 5) * clkfreq;
            starttime = GetTicks();
        }
    }
}

// Load the vocab table
void LoadVocab(char **tokens, int num)
{
    FILE *infile;

    if ((infile = OpenFile(tokens, num, "load", "r")))
    {
        size = fread(vocabptr, 1, 8000, infile);
        fclose(infile);
        printf("Vocab size = %d bytes.  %d bytes remaining\n", size, VOCAB_SIZE - size);
        Memory[0] = 0;
        MemoryIndex = 0;
    }
}

// Save the vocab table
void SaveVocab(char **tokens, int num)
{
    FILE *outfile;

    if ((outfile = OpenFile(tokens, num, "save", "w")))
    {
        fwrite(vocabptr, 1, size, outfile);
        fclose(outfile);
    }
}

FILE *OpenFile(char **tokens, int num, char *command, char *mode)
{
    FILE *fd;
    char *fname;

    if (num > 2)
    {
        printf("usage: %s [file]\n", command);
        return 0;
    }

    if (num == 2)
        fname = tokens[1];
    else
        fname = "lerner.voc";
    if (!(fd = fopen(fname, mode)))
        printf("Couldn't open %s\n", fname);

    return fd;
}

int CheckCommand(char **tokens, int num, char *buffer)
{
    char *ptr1;
    int retval, caseshift;
    retval = 1;
    caseshift = (tokens[0][0] >= 'A' && tokens[0][0] <= 'Z');
    if (num > 2)
    {
        return 0;
    }
    if (caseshift)
    {
        tokens[0][0] += 'a' - 'A';
    }
    if (strcmp(tokens[0], "help") == 0)
    {
        printf("$ Type a sentence, or enter one of these commands\n");
        printf("$ vocab [word] - Print the words I know\n");
        printf("$ dump         - Print the words as hex codes\n");
        printf("$ forget       - Remove a word from memory\n");
        printf("$ save [file]  - Save vocabulary to a file (default lerner.voc\n");
        printf("$ load [file]  - Load vocabulary from a file (default lerner.voc\n");
        printf("$ reset        - Reset the vocabulary\n");
        printf("$ basic        - Reset the vocabulary to a basic set\n");
        printf("$ history      - Print the short term memory\n");
        printf("$ goodbye      - Exit the program\n");
    }
    else if (!strcmp(tokens[0], "goodbye") || !strcmp(tokens[0], "bye"))
    {
        if (changeflag)
        {
            while (1)
            {
                printf("< Should I save?\n");
                printf("> ");
                gets(buffer);
                ptr1 = SkipChar(buffer, ' ');
                if (toupper(*ptr1) == 'Y')
                {
#if 0
                    tokens[1] = "lerner.voc";
                    SaveVocab(tokens, 2);
#else
                    SaveVocab(tokens, 1);
#endif
                    break;
                }
                else if (toupper(*ptr1) == 'N')
                {
                    break;
                }
                printf("< I don't undertand\n");
            }
        }
        printf("< Goodbye\n");
        exit(0);
    }
    else if (strcmp(tokens[0], "vocab") == 0)
    {
        DumpAsciiWords(tokens[1], num);
    }
    else if (strcmp(tokens[0], "dump") == 0)
    {
        DumpHexWords();
    }
    else if (strcmp(tokens[0], "forget") == 0)
    {
        DeleteWord(tokens, num);
    }
    else if (strcmp(tokens[0], "save") == 0)
    {
        SaveVocab(tokens, num);
    }
    else if (strcmp(tokens[0], "load") == 0)
    {
        LoadVocab(tokens, num);
    }
    else if (strcmp(tokens[0], "reset") == 0)
    {
        ResetVocab();
    }
    else if (strcmp(tokens[0], "basic") == 0)
    {
        ResetBasicVocab();
    }
    else if (strcmp(tokens[0], "history") == 0)
    {
        PrintMemory();
    }
    else if (strcmp(tokens[0], "debug") == 0)
    {
        debugmode = !debugmode;
    }
    else
    {
        retval = 0;
    }
    // Restore the case if needed
    if (caseshift)
    {
        tokens[0][0] -= 'a' - 'A';
    }
    return retval;
}

#if 0
// This routine set the linefeed enable flag based on the config word
// The config word is the first long after the last argument
void config_linefeed(int argc, char **argv)
{
    char *ptr = argv[argc - 1];
    ptr += strlen(ptr) + 1;
    ptr = (char *)((((int)ptr + 3) >> 2) << 2);
#ifdef CSPIN_FLAG
    set_linefeed(*ptr & 1);
#endif
}
#endif

int main( int argc, char **argv)
{
    int i, num;
    char *ptr1;
    int ignore, question;
    char buffer[100];
    char *tokens[50];

#ifdef __P2GCC__
    sd_mount(58, 61, 59, 60);
    chdir(argv[argc]);
    start_rx_cog();
#endif

    //speak := c.getstdout
    //speak := c.openserial(16, 16, 0, 9600, 64, 64)
    //config_linefeed(argc, argv);
    printf("Lerner 0.09\n");
    vocabptr = malloc(VOCAB_SIZE);
#if 0
    size = basicsize;
    memcpy(vocabptr, basicvocab, size);
#else
    LoadVocab(tokens, 1);
#endif
    i = 0;
    while (i <= MEMORY_MASK)
    {
        Memory[i] = 0;
        i++;
    }
    //printf("< Hello\n");
    ptr1 = FindWords("you", "are");
    if (ptr1)
    {
        PrintStatement("you", ptr1, 1);
    }
    // Loop on each user input
    while (1)
    {
        WaitForInput();
        gets(buffer);
        num = tokenize(buffer, tokens);
        if (num == 0)
        {
            continue;
        }
        if (CheckCommand(tokens, num, buffer))
        {
            continue;
        }
        // Check for a question mark at the end
        if (strcmp(tokens[num - 1], "?") == 0)
        {
            question = 1;
            num--;
        }
        else
        {
            question = 0;
        }
        // Check if all the words are in the vocab table
        ignore = 0;
        i = 0;
        while (i < num)
        {
            if (FindWord(tokens[i]) == 0)
            {
                if ((ignore = UnknownWord(tokens, num, i)))
                {
                    break;
                }
            }
            i++;
        }
        if (ignore)
        {
            continue;
        }
        // Process the user input
        Process(tokens, num, question);
    }
    return 0;
}
/*
+-----------------------------------------------------------------------------+
|                          TERMS OF USE: MIT License                          |
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
