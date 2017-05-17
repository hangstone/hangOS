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
int   AdjustInSectorSize(int nFd, int nSourceSize);
void  WriteKernelInformation(int nTargetFd, int nKernelSectorCount);
int   CopyFile(int nSourceFd, int nTargetFd);

int   main(int argc, char* argv[])
{
  int nSourceFd;
  int nTargetFd;
  int nBootLoaderSize;
  int nKernel32SectorCount;
  int nSourceSize;

  if (3 > argc)
  {
      fprintf(stderr, "[ERROR] ImageMaker.exe BootLoader.bin Kernel32.bin\n");
      exit(-1);
  }

  nTargetFd = open("Disk.img",
									 O_RDWR | O_CREAT | O_TRUNC | O_BINARY,
									 S_IREAD |S_IWRITE);
  if (-1 == nTargetFd)
  {
      fprintf(stderr, "[ERROR] faied to open Disk.img.\n");
      exit(-1);
  }

  //  부트로더 파일을 읽어서 모든 내용을 디스크 이미지 파일로 복사
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

  //  32-bit 커널 파일을 열어서 모든 내용을 디스크 이미지 파일로 복사
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

  //  디스크 이미지에 커널 정보 갱신
  printf("[INFO] Start to write kernel information\n");
  //  부트섹터의 5번째 바이트부터 커널에 대한 정보를 넣음
  WriteKernelInformation(nTargetFd, nKernel32SectorCount);
  printf("[INFO] Complete to create image file\n");

  close(nTargetFd);
  return 0;
}

/**
 *  현재 위치부터 512바이트 배수 위치까지 맞추어 0x00으로 채움
 */
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

/**
 *  부트 로더에 커널에 대한 정보를 삽입
 */
void  WriteKernelInformation(int nTargetFd, int nKernelSectorCount)
{
    unsigned short usData;
    long lPosition;

    // 파일의 시작에서 5바이트 떨어진 위치가 커널의 총 섹터 수 정보를 나타냄
    lPosition = lseek( nTargetFd, 5, SEEK_SET );
    if( lPosition == -1 )
    {
        fprintf( stderr, "lseek fail. Return value = %d, errno = %d, %d\n",
            lPosition, errno, SEEK_SET );
        exit( -1 );
    }

    usData = ( unsigned short ) nKernelSectorCount;
    write( nTargetFd, &usData, 2 );

    printf( "[INFO] Total sector count except boot loader [%d]\n",
        nKernelSectorCount );
}

/**
 *  소스 파일(Source FD)의 내용을 목표 파일(Target FD)에 복사하고 그 크기를 되돌려줌
 */
int   CopyFile(int nSourceFd, int nTargetFd)
{
    int iSourceFileSize;
    int iRead;
    int iWrite;
    char vcBuffer[ BYTESOFSECTOR ];

    iSourceFileSize = 0;
    while( 1 )
    {
        iRead   = read( nSourceFd, vcBuffer, sizeof( vcBuffer ) );
        iWrite  = write( nTargetFd, vcBuffer, iRead );

        if( iRead != iWrite )
        {
            fprintf( stderr, "[ERROR] iRead != iWrite.. \n" );
            exit(-1);
        }
        iSourceFileSize += iRead;

        if( iRead != sizeof( vcBuffer ) )
        {
            break;
        }
    }
    return iSourceFileSize;
}
