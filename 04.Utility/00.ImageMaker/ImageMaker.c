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

#define BYTESOFSECTOR  512

//  function declaration

/**
 *  현재 위치부터 512바이트 배수 위치까지 맞추어 0x00으로 채움
 */
int   AdjustInSectorSize(int nFd, int nSourceSize);

/**
 *  부트 로더에 커널에 대한 정보를 삽입
 */
void  WriteKernelInformation(int nTargetFd,
                             int nTotalKernelSectorCount,
                             int nKernel32SectorCount);

/**
 *  소스 파일(Source FD)의 내용을 목표 파일(Target FD)에 복사하고 그 크기를 되돌려줌
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

  //	Disk.img 파일을 생성
  nTargetFd = open("Disk.img",
									 O_RDWR | O_CREAT | O_TRUNC | O_BINARY,
									 S_IREAD |S_IWRITE);
  if (-1 == nTargetFd)
  {
      fprintf(stderr, "[ERROR] faied to open Disk.img.\n");
      exit(-1);
  }

  /*
   *	부트로더 파일을 읽어서 모든 내용을 디스크 이미지 파일로 복사
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

  //  파일 크기를 섹터 크기인 512-byte로 맞추기 위해 나머지 부분을 0x00으로 채움
  nBootLoaderSize = AdjustInSectorSize(nTargetFd, nSourceSize);
  printf("[INFO] %s size = [%d] and sector count = [%d]\n",
         argv[1],
         nSourceSize,
         nBootLoaderSize);

  /*
   *  32-bit 커널 파일을 열어서 모든 내용을 디스크 이미지 파일로 복사
   */
  printf("[INFO] Copy protected mode kernel to image file\n");
  if ((nSourceFd = open(argv[2], O_RDONLY | O_BINARY)) == -1)
  {
    fprintf(stderr, "[ERROR] failed to open %s", argv[2]);
    exit(-1);
  }

  nSourceSize = CopyFile(nSourceFd, nTargetFd);
  close(nSourceFd);

  //  파일 크기를 섹터 크기인 512-byte로 맞추기 위해 나머지 부분을 0x00으로 채움
  nKernel32SectorCount = AdjustInSectorSize(nTargetFd, nSourceSize);
  printf("[INFO] %s size = [%d] and sector count = [%d]\n",
         argv[2],
         nSourceSize,
         nKernel32SectorCount);

  /*
   *  64-bit 커널 파일(IA-32e 모드 커널 이미지)을 열어서 모든 내용을 디스크 이미지 파일로 복사
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

  //  파일 크기를 섹터 크기인 512-byte로 맞추기 위해 나머지 부분을 0x00으로 채움
  nKernel64SectorCount = AdjustInSectorSize(nTargetFd, nSourceSize);
  printf("[INFO] %s size = [%d] and sector count = [%d]\n",
         argv[3],
         nSourceSize,
         nKernel64SectorCount);

  /**
   *  디스크 이미지에 커널 정보 갱신
   */
  printf("[INFO] Start to write kernel information\n");
  //  부트섹터의 5번째 바이트부터 커널에 대한 정보를 넣음
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

      // 섹터 수를 되돌려줌
      iSectorCount = ( iSourceSize + iAdjustSizeToSector ) / BYTESOFSECTOR;
      return iSectorCount;
}

void  WriteKernelInformation(int nTargetFd,
														 int nTotalKernelSectorCount,
														 int nKernel32SectorCount)
{
	unsigned short usData;
  long lPosition;

  // 파일의 시작에서 5바이트 떨어진 위치가 커널의 총 섹터 수 정보를 나타냄
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

  //	부트로더를 제외한 총 sector 수 및 보호 모드 커널의 섹터 수 저장
  //	bootloader image에서 보호 모드 커널의 총 sector 수 영역은 총 sector 수 영역의 바로 이후레 위치하므로,
  //	총 sector 수 정보에 이어서 2-byte를 기록하도록 수정
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
