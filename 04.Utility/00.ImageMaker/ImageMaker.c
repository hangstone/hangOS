/*
 * ImageMaker.c
 *
 *  Created on: 2017. 5. 14.
 *      Author: hangstone
 */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#define BYTESOFSECTOR  512

//  function declaration

/**
 *  ���� ��ġ���� 512����Ʈ ��� ��ġ���� ���߾� 0x00���� ä��
 */
int   AdjustInSectorSize(int nFd, int nSourceSize);

/**
 *  ��Ʈ �δ��� Ŀ�ο� ���� ������ ����
 */
void  WriteKernelInformation(int nTargetFd,
                             int nTotalKernelSectorCount,
                             int nKernel32SectorCount);

/**
 *  �ҽ� ����(Source FD)�� ������ ��ǥ ����(Target FD)�� �����ϰ� �� ũ�⸦ �ǵ�����
 */
int   CopyFile(int nSourceFd, int nTargetFd);

int   main(int argc, char* argv[])
{
  int nSourceFd;
  int nTargetFd;
  int nBootLoaderSize;
  int nKernel32SectorCount;
  int nKernel64SectorCount;
  int nSourceSize;

  //	check command line parameter
  if (4 > argc)
  {
      fprintf(stderr, "[ERROR] ImageMaker.exe BootLoader.bin Kernel32.bin Kernel64.bin\n");
      exit(-1);
  }

  //	Disk.img ������ ����
  nTargetFd = open("Disk.img",
									 O_RDWR | O_CREAT | O_TRUNC | O_BINARY,
									 S_IREAD |S_IWRITE);
  if (-1 == nTargetFd)
  {
      fprintf(stderr, "[ERROR] faied to open Disk.img.\n");
      exit(-1);
  }

  /*
   *	��Ʈ�δ� ������ �о ��� ������ ��ũ �̹��� ���Ϸ� ����
   */
  printf("[INFO] Copy boot loader to image file\n");
  nSourceFd = open(argv[1], O_RDONLY | O_BINARY);
  if (-1 == nSourceFd)
  {
    fprintf(stderr, "[ERROR] failed to open %s\n", argv[1]);
    exit(-1);
  }

  nSourceSize = CopyFile(nSourceFd, nTargetFd);
  close(nSourceFd);

  //  ���� ũ�⸦ ���� ũ���� 512-byte�� ���߱� ���� ������ �κ��� 0x00���� ä��
  nBootLoaderSize = AdjustInSectorSize(nTargetFd, nSourceSize);
  printf("[INFO] %s size = [%d] and sector count = [%d]\n",
         argv[1],
         nSourceSize,
         nBootLoaderSize);

  /*
   *  32-bit Ŀ�� ������ ��� ��� ������ ��ũ �̹��� ���Ϸ� ����
   */
  printf("[INFO] Copy protected mode kernel to image file\n");
  if ((nSourceFd = open(argv[2], O_RDONLY | O_BINARY)) == -1)
  {
    fprintf(stderr, "[ERROR] failed to open %s", argv[2]);
    exit(-1);
  }

  nSourceSize = CopyFile(nSourceFd, nTargetFd);
  close(nSourceFd);

  //  ���� ũ�⸦ ���� ũ���� 512-byte�� ���߱� ���� ������ �κ��� 0x00���� ä��
  nKernel32SectorCount = AdjustInSectorSize(nTargetFd, nSourceSize);
  printf("[INFO] %s size = [%d] and sector count = [%d]\n",
         argv[2],
         nSourceSize,
         nKernel32SectorCount);

  /*
   *  64-bit Ŀ�� ����(IA-32e ��� Ŀ�� �̹���)�� ��� ��� ������ ��ũ �̹��� ���Ϸ� ����
   */
  printf("[INFO] Copy IA-32e mode kernel to image file\n");
  nSourceFd = open(argv[3], O_RDONLY | O_BINARY);
  if (-1 == nSourceFd)
  {
  	fprintf(stderr, "[ERROR] failed to open %s", argv[3]);
    exit(-1);
  }

  nSourceSize = CopyFile(nSourceFd, nTargetFd);
  close(nSourceFd);

  //  ���� ũ�⸦ ���� ũ���� 512-byte�� ���߱� ���� ������ �κ��� 0x00���� ä��
  nKernel64SectorCount = AdjustInSectorSize(nTargetFd, nSourceSize);
  printf("[INFO] %s size = [%d] and sector count = [%d]\n",
         argv[3],
         nSourceSize,
         nKernel64SectorCount);

  /**
   *  ��ũ �̹����� Ŀ�� ���� ����
   */
  printf("[INFO] Start to write kernel information\n");
  //  ��Ʈ������ 5��° ����Ʈ���� Ŀ�ο� ���� ������ ����
  WriteKernelInformation(nTargetFd,
												 nKernel32SectorCount + nKernel64SectorCount,
												 nKernel32SectorCount);
  printf("[INFO] Complete to create image file\n");

  close(nTargetFd);
  return 0;
}

int   AdjustInSectorSize(int iFd, int iSourceSize)
{
  int i;
      int iAdjustSizeToSector;
      char cCh;
      int iSectorCount;

      iAdjustSizeToSector = iSourceSize % BYTESOFSECTOR;
      cCh = 0x00;

      if( iAdjustSizeToSector != 0 )
      {
          iAdjustSizeToSector = 512 - iAdjustSizeToSector;
          printf( "[INFO] File size [%lu] and fill [%u] byte\n", iSourceSize,
              iAdjustSizeToSector );
          for( i = 0 ; i < iAdjustSizeToSector ; i++ )
          {
              write( iFd , &cCh , 1 );
          }
      }
      else
      {
          printf( "[INFO] File size is aligned 512 byte\n" );
      }

      // ���� ���� �ǵ�����
      iSectorCount = ( iSourceSize + iAdjustSizeToSector ) / BYTESOFSECTOR;
      return iSectorCount;
}

void  WriteKernelInformation(int nTargetFd,
														 int nTotalKernelSectorCount,
														 int nKernel32SectorCount)
{
	unsigned short usData;
  long lPosition;

  // ������ ���ۿ��� 5����Ʈ ������ ��ġ�� Ŀ���� �� ���� �� ������ ��Ÿ��
  lPosition = lseek(nTargetFd, (off_t)5, SEEK_SET);
  if (lPosition == -1)
  {
  	fprintf(stderr,
						"lseek fail. Return value = %d, errno = %d, %d\n",
						lPosition,
						errno,
						SEEK_SET);
    exit( -1 );
  }
  else
  {
  	printf("[INFO] Found sector count position: %d\n", lPosition);
  }

  //	��Ʈ�δ��� ������ �� sector �� �� ��ȣ ��� Ŀ���� ���� �� ����
  //	bootloader image���� ��ȣ ��� Ŀ���� �� sector �� ������ �� sector �� ������ �ٷ� ���ķ� ��ġ�ϹǷ�,
  //	�� sector �� ������ �̾ 2-byte�� ����ϵ��� ����
  usData = (unsigned short)nTotalKernelSectorCount;
  write(nTargetFd, &usData, 2);
  usData = (unsigned short)nKernel32SectorCount;
  write(nTargetFd, &usData, 2);

  printf("[INFO] Total sector count except boot loader [%d]\n",
				 nTotalKernelSectorCount);
  printf("[INFO] Total sector count of protected mode kernel [%d]\n",
				 nKernel32SectorCount);
}

int   CopyFile(int nSourceFd, int nTargetFd)
{
	int nSourceFileSize;
  int nRead;
  int nWrite;
  char vcBuffer[BYTESOFSECTOR];

  nSourceFileSize = 0;
  while (1)
  {
  	nRead		= read(nSourceFd, vcBuffer, sizeof(vcBuffer));
  	nWrite	= write(nTargetFd, vcBuffer, nRead);

    if (nRead != nWrite)
    {
    	fprintf(stderr,
							"[ERROR] iRead != iWrite.. \n");
		  exit(-1);
    }

    nSourceFileSize += nRead;

    if(nRead != sizeof(vcBuffer))
    {
    	break;
    }
  }

  return nSourceFileSize;
}
