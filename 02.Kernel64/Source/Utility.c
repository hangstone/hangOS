/*
 * Utility.h
 *
 *  Created on: 2017. 9. 12.
 *      Author: hangstone
 */

#include "Types.h"
#include "Utility.h"
#include "AssemblyUtility.h"

//  메모리를 특정 값으로 채움
void  kMemSet(void* pvDestination, BYTE bData, int nSize)
{
  for (int nIdx=0; nIdx < nSize; nIdx++)
  {
    ((char *)pvDestination)[nIdx] = bData;
  }
}

//  메모리 복사
int   kMemCpy(void* pvDestination, const void* pvSource, int nSize)
{
  for (int nIdx = 0; nIdx < nSize; nIdx++)
  {
    ((char *)pvDestination)[nIdx] = ((char *)pvSource)[nIdx];
  }

  return nSize;
}

//  메모리 비교
int   kMemCmp(const void* pvDestination, const void* pvSource, int nSize)
{
  for (int nIdx = 0; nIdx < nSize; nIdx++)
  {
    char cTemp;
    cTemp = ((char *)pvDestination)[nIdx] - ((char *)pvSource)[nIdx];
    if (0 != cTemp)
    {
      return (int)cTemp;
    }
  }

  return 0;
}

BOOL  kSetInterruptFlag(BOOL bEnableInterrupt)
{
  QWORD qwRFlags;

  //  이전의 RFLAGS 레지스터 값을 읽은 뒤에 인터럽트 가능/불가 처리
  qwRFlags = kReadRFLAGS();
  if (TRUE == bEnableInterrupt)
    kEnableInterrupt();
  else
    kDisableInterrupt();

  //  이전의 RFLAGS 레지스터의 IF 비트(9번쨰 bit)를 확인하여 이전의 인터럽트 상태를 반환
  if (qwRFlags & 0x0200)
    return TRUE;
  else
    return FALSE;
}
