#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    int num;
    int i, ival;
    int filesize;
    char fname[100];
    int checksum = 0;
    char *ptr;
    char *ptr1 = malloc(300000);
    FILE *outfile;

    sd_mount(58, 61, 59, 60);
    start_rx_cog();

    ptr = (char *)&filesize;
    for (i = 0; i < 4; i++)
        ptr[i] = getch();

    ptr = fname;
    for (i = 0; i < 100; i++)
    {
        ival = getch();
        *ptr++ = ival;
        if (!ival) break;
    }

    ptr = ptr1;
    num = (filesize+3)/4;

    while (num-- > 0)
    {
        for (i = 0; i < 4; i++)
            ptr[i] = getch();
        checksum += *(int *)ptr;
        ptr += 4;
    }

    ptr = (char *)&ival;
    for (i = 0; i < 4; i++)
        ptr[i] = getch();

    if (ival == checksum)
        putch('+');
    else
        putch('-');

    outfile = fopen(fname, "wb");
    fwrite(ptr1, 1, filesize, outfile);
    fclose(outfile);
    free(ptr1);

    outfile = fopen("logfile.txt", "wb");
    sprintf(fname, "ival     = %d, %x\n", ival, ival);
    fputs(fname, outfile);
    fputc(10, outfile);
    sprintf(fname, "checksum = %d, %x\n", checksum, checksum);
    fputs(fname, outfile);
    fputc(10, outfile);
    fclose(outfile);

    stop_rx_cog();

    return 0;
}

        
        
