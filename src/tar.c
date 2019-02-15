//*****************************************************************************
// Copyright (c) 2011-2019, Dave Hein
// See end of file for terms of use.
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <memory.h>

#define NAME 0
#define MODE 100
#define OWNER 108
#define GROUP 116
#define SIZE 124
#define TIME 136
#define CHECKSUM 148
#define FILETYPE 156
#define LINKEDFILE 157

void create(int argc, char **argv);
void extract(int argc, char **argv);
void usage(void);

unsigned char buffer[512];

int main(int argc, char **argv)
{
#ifdef __P2GCC__
    sd_mount(58, 61, 59, 60);
    chdir(argv[argc]);
#endif
    if (argc < 3) usage();
    if (strcmp(argv[1], "-xf") == 0) extract(argc, argv);
    else if (strcmp(argv[1], "-cf") == 0) create(argc, argv);
    else usage();
    exit(0);
}

void usage(void)
{
    printf("usage: tar [flags] tarfile [filelist]\n");
    printf("         -xf Extract files from tarfile\n");
    printf("         -cf Create tarfile and add files\n");
    exit(1);
}

void create(int argc, char **argv)
{
    FILE *infile;
    FILE *outfile;
    int i, num, filesize, checksum, argi;

    outfile = fopen(argv[2], "w");
    if (!outfile)
    {
        printf("Could not create tarfile %s\n", argv[2]);
        exit(1);
    }

    for (argi = 3; argi < argc; argi++)
    {
        infile = fopen(argv[argi], "r");
        if (infile == 0)
        {
            printf("Could not open input file %s\n", argv[argi]);
            //continue;
        }
        memset(buffer, 0, 512);
        strcpy((char *)buffer, argv[argi]);
        strcpy((char *)&buffer[MODE], "0000666");
        strcpy((char *)&buffer[OWNER], "0001370");
        strcpy((char *)&buffer[GROUP], "0000765");
        fseek(infile, 0, SEEK_END);
        filesize = ftell(infile);
        fseek(infile, 0, SEEK_SET);
        sprintf((char *)&buffer[SIZE], "%011o", filesize);
        strcpy((char *)&buffer[TIME], "11374320374");
        buffer[FILETYPE] = '0';
        //checksum = 1718 + buffer[FILETYPE];
        checksum = 256;
        for (i = 0; i < CHECKSUM; i++) { checksum += buffer[i];}
        for (i = FILETYPE; i < 512; i++) { checksum += buffer[i];}
        buffer[CHECKSUM+7] = ' ';
        sprintf((char *)&buffer[CHECKSUM], "%06o", checksum);
        fwrite(buffer, 1, 512, outfile);
        while (filesize > 0)
        {
            num = fread(buffer, 1, 512, infile);
            if (num <= 0) break;
            if (num < 512) memset(&buffer[num], 0, 512 - num);
            fwrite(buffer, 1, 512, outfile);
            filesize -= 512;
        }
        fclose(infile);
    }
    memset(buffer, 0, 512);
    fwrite(buffer, 1, 512, outfile);
    fwrite(buffer, 1, 512, outfile);
    fclose(outfile);
}

void extract(int argc, char **argv)
{
    int i;
    int num;
    //int blocknum = 0;
    FILE *infile;
    FILE *outfile;
    //int skipcount = 0;
    int filesize;
    int checksum = 0;
    int attributes;

    infile = fopen(argv[2], "r");

    if (infile == 0)
    {
        printf("Could not open input file\n");
        exit(1);
    }

    while (1)
    {
        if (fread(buffer, 1, 512, infile) != 512) break;
        //if (skipcount-- > 0) continue;
        for (i = 0; i < 512; i++) { if (buffer[i]) break;}
        if (i == 512) break;
        
#if 0
        printf("Block number %d\n", blocknum++);
        for (i = 0; i < 512; i++)
        {
            if ((i&63) == 0) printf("\n%3d: ", i);
            if (buffer[i] >= 32 && buffer[i] <= 126) printf("%c", buffer[i]);
            else printf(".");
        }
        printf("\n");
#else
#if 0
        printf("%s, ", &buffer[NAME]);
        printf("%s, ", &buffer[MODE]);
        printf("%s, ", &buffer[OWNER]);
        printf("%s, ", &buffer[GROUP]);
        printf("%s, ", &buffer[SIZE]);
        printf("%s, ", &buffer[TIME]);
        printf("%s, ", &buffer[CHECKSUM]);
        printf("%c, ", buffer[FILETYPE]);
        printf("%s, ", &buffer[LINKEDFILE]);
        printf("\n");
#endif
        sscanf((char *)&buffer[CHECKSUM], "%o", &checksum);
        //checksum = 1718 + buffer[FILETYPE] - checksum;
        checksum = 256 - checksum;
        for (i = 0; i < CHECKSUM; i++) { checksum += buffer[i];}
        for (i = FILETYPE; i < 512; i++) { checksum += buffer[i];}
        //printf("checksum = %o", checksum);
        sscanf((char *)&buffer[SIZE], "%o", &filesize);
        sscanf((char *)&buffer[MODE], "%o", &attributes);
        printf("filename = %s, filesize = %d, attributes = %o, checksum = %d\n", buffer, filesize, attributes, checksum);
        //skipcount = (filesize + 511) >> 9;
#endif
        if (checksum)
        {
            printf("Checksum error\n");
            exit(1);
        }
        outfile = fopen((char *)buffer, "w");
        if (!outfile)
        {
            printf("Could not open output file %s\n", buffer);
            exit(1);
        }
        while (filesize > 0)
        {
            num = fread(buffer, 1, 512, infile);
            if (num != 512)
            {
                printf("Encountered EOF on tar file\n");
                exit(1);
            }
            num = 512;
            if (filesize < num) num = filesize;
            fwrite(buffer, 1, num, outfile);
            filesize -= 512;
        }
        fclose(outfile);
    }
    fclose(infile);
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

