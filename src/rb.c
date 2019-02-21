//******************************************************************************
// Copyright (c) 2011-2019, Dave Hein
// See end of file for terms of use.
//******************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef __P2GCC__
#include <propeller.h>
#endif

//XMODEM chars from ymodem.txt
#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define CEE 0x43  //liberties here
#define ZEE 0x1A  //or SUB  (DOS EOF?)

#define MSG_TIMEOUT  -2
#define MSG_EOT      -3
#define BYTE_TIMEOUT -4
#define MSG_BADNUM   -5
#define MSG_CRCERR   -6

#ifdef __P2GCC__
#define p2clkfreq (*(int *)0x14)
#endif

#undef LOGFILE

int DecodeFirstPacket(unsigned char *ptr, int num, char *fname);
int ReceivePacket(unsigned char *ptr);
int CheckCRC(unsigned char *ptr, int num);
int ComputeCRC(unsigned char *ptr, int num);;
void putch(int val);
int rxtime(int msec);

#ifdef __P2GCC__
int kbhit(void);
int getch(void);
#endif

static unsigned char *pdata = 0;
static int waittime = 15;
#ifdef LOGFILE
static FILE *logfile;
#endif

static unsigned short fcstab[] = {
  0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
  0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
  0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
  0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
  0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
  0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
  0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
  0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
  0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
  0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
  0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
  0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
  0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
  0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
  0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
  0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
  0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
  0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
  0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
  0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
  0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
  0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
  0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
  0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
  0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
  0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
  0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
  0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
  0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
  0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
  0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
  0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0};

int main(int argc, char **argv)
{
  int packetnum, num, filesize;
  char fname[80];
  FILE *outfile;
  unsigned char *ptr;

#ifdef __P2GCC__
  sd_mount(58, 61, 59, 60);
  chdir(argv[argc]);
  start_rx_cog();
#endif

  if (argc > 1)
  {
    sscanf(argv[1], "%d", &waittime);
    if (waittime < 0)
      waittime = 0;
    else if (waittime > 30)
      waittime = 30;
  }
  pdata = malloc(1028);
  if (!pdata)
  {
    printf("Could not malloc 1028 bytes for the data buffer\n");
#ifdef __P2GCC__
    stop_rx_cog();
#endif
    exit(1);
  }
#ifdef LOGFILE
  logfile = fopen("logfile.txt", "wb");
#endif
  if (argc <= 1)
    printf("ymodem receive starts in %d seconds\n", waittime);
  if (waittime)
    sleep(waittime);
  outfile = 0;
  ptr = pdata;
  packetnum = 0;
  while (1)
  {
    if (packetnum < 2)
      putch(CEE);
    num = ReceivePacket(ptr);
#ifdef LOGFILE
    fprintf(logfile, "ReceivePacket returned %d %d\n", num, pdata[0]);
#endif
    if (num == MSG_TIMEOUT)
    {
      putch(EOT);
      break;
    }
    else if (num == MSG_EOT)
    {
      packetnum = 0;
      putch(ACK);
      continue;
    }
    else if (num <= 0)
    {
      putch(NAK);
      continue;
    }
    if (packetnum == 0)
    {
      if (pdata[2] == 0)
      {
        putch(ACK);
        break;
      }
      filesize = DecodeFirstPacket(ptr+2, num, fname);
#ifdef LOGFILE
      fprintf(logfile, "fname = %s, size = %d\n", fname, filesize);
#endif
      outfile = fopen(fname, "wb"); //Open file
    }
    else
    {
      if (num > filesize)
        num = filesize;
      fwrite(&pdata[2], 1, num, outfile);
      filesize -= num;
    }
    putch(ACK);
    packetnum++;
  }
  if (outfile)
    fclose(outfile);;
#ifdef LOGFILE
  fclose(logfile);
#endif
  free(pdata);
#ifdef __P2GCC__
  stop_rx_cog();
#endif
  return 0;
}

int DecodeFirstPacket(unsigned char *ptr, int num, char *fname)
{
  int i, val, filesize;

  if (!*ptr)
  {
    *fname = 0;
    return -1;
  }
  for (i = 0; i <= 40; i++)
  {
    fname[i] = (char)ptr[i];
    if (!ptr[i])
    {
      i++;
      break;
    }
  }
  filesize = 0;
  while (i < num)
  {
    val = ptr[i++];
    if (val < '0' || val > '9')
      break;
    filesize = (filesize * 10) + val - '0';
  }

  return filesize;
}

#ifdef __P2GCC__
int rxtime(int msec)
{
  int cycles, cycles0, elapsed;

  cycles = msec * (p2clkfreq/1000);
  cycles0 = CNT;
  while (1)
  {
    elapsed = CNT - cycles0;
    if (elapsed > cycles)
      return -1;
    if (kbhit())
      return getch();
  }
}
#endif

int ReceivePacket(unsigned char *ptr)
{
  int num, val, count;
  unsigned char *ptr0 = ptr;

  // Get start of packet
  while (1)
  {
    if ((val = rxtime(10000)) == -1)
        return MSG_TIMEOUT;
#ifdef LOGFILE
    fprintf(logfile, "rx %02x\n", val);
#endif
    if (val == SOH)
    {
      num = 128;
      break;
    }
    else if (val == STX)
    {
      num = 1024;
      break;
    }
    else if (val == EOT)
      return MSG_EOT;
  }
      
  // Read the packet    
  count = num + 4;
  while (count)
  {
    if ((val = rxtime(1000)) == -1)
        return BYTE_TIMEOUT;
    *ptr++ = val;
    count--;
  }

  // Check for valid packet number pair
  if (ptr0[0] + ptr0[1] != 255)
    return MSG_BADNUM;
    
  // Check the CRC
  if (!CheckCRC(ptr0+2, num))
    return MSG_CRCERR;
    
  return num;
}

int CheckCRC(unsigned char *ptr, int num)
{
  int crc = ComputeCRC(ptr, num);
  return ((ptr[num] == (crc >> 8)) && (ptr[num+1] == (crc & 0xff)));
}

int ComputeCRC(unsigned char *ptr, int num)
{
  int result = 0;

  while (num-- > 0)
    result = ((result << 8) & 0xffff) ^ fcstab[(result >> 8) ^ *ptr++];

  return result;
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
