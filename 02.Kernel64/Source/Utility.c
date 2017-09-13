/*
 * Utility.h
 *
 *  Created on: 2017. 9. 12.
 *      Author: hangstone
 */

#include "Types.h"
#include "Utility.h"

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
  for (int nIdx = 0; nIdx = nSize; nIdx++)
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