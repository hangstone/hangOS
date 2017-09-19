/*
 * Queue.c
 *
 *  Created on: 2017. 9. 15.
 *      Author: hangstone
 */

#include "Queue.h"

void kInitializeQueue(QUEUE* pstQueue, 
                      void* pvQueueBuffer, 
                      int nMaxDataCount, 
                      int nDataSize)
{
  //  큐의 최대 갯수, 크기 그리고 buffer address를 저장
  pstQueue->nMaxDataCount = nMaxDataCount;
  pstQueue->nDataSize = nDataSize;
  pstQueue->pvQueueArray = pvQueueBuffer;

  //  큐의 삽입 위치와 제거 위치를 초기화하고, 마지막으로 수행된 명령을 '제거'로 설정하여
  //  큐를 비어있는 상태로 설정
  pstQueue->nPutIndex = 0;
  pstQueue->nGetIndex = 0;
  pstQueue->bIsLastOperation_Put = FALSE;
}

BOOL kIsQueueFull(const QUEUE* pstQueue)
{
  //  큐의 삽입 인덱스와 제거 인덱스가 같고, 마지막으로 수행된 명령이 삽입이면
  //  큐가 가득 찼으므로 삽입할 수 없음
  if ((pstQueue->nPutIndex == pstQueue->nGetIndex) &&
      (TRUE == pstQueue->bIsLastOperation_Put))
    return TRUE;
  else
    return FALSE;
}

BOOL kIsQueueEmpty(const QUEUE* pstQueue)
{
  //  큐의 삽입 인덱스와 제거 인덱스가 같고, 마지막으로 수행된 명령이 제거이면
  //  큐가 비어 있으므로 제거할 수 없음
  if ((pstQueue->nPutIndex == pstQueue->nGetIndex) &&
      (FALSE == pstQueue->bIsLastOperation_Put))
    return TRUE;
  else
    return FALSE;
}

BOOL kPutQueue(QUEUE* pstQueue, const void* pvData)
{
  //  큐가 가득 찼으면 삽입할 수 없음
  if (TRUE == kIsQueueFull(pstQueue))
    return FALSE;

  //  삽입 인덱스가 가리키는 위치에서 데이터의 크기 만큼을 복사
  int nPositionToPut = (pstQueue->nDataSize * pstQueue->nPutIndex);
  char* pDest = (char*)pstQueue->pvQueueArray + nPositionToPut;
  kMemCpy(pDest, pvData, pstQueue->nDataSize);

  //  삽입 인덱스를 변경하고 삽입 동작을 수행했음을 등록
  pstQueue->nPutIndex = (pstQueue->nPutIndex + 1) % pstQueue->nMaxDataCount;
  pstQueue->bIsLastOperation_Put = TRUE;

  return TRUE;
}

BOOL kGetQueue(QUEUE* pstQueue, void* pvData)
{
  //  큐가 비었으면 제거할 수 없음
  if (TRUE == kIsQueueEmpty(pstQueue))
    return FALSE;

  //  제거 인덱스가 가리키는 위치에서 데이터의 크기만큼을 복사
  int nPositionToPut = (pstQueue->nDataSize * pstQueue->nGetIndex);
  char* pDest = (char*)pstQueue->pvQueueArray + nPositionToPut;
  kMemCpy(pvData, pDest, pstQueue->nDataSize);

  //  제거 인덱스를 변경하고 제거 동작을 수행했음을 등록
  pstQueue->nGetIndex = (pstQueue->nGetIndex + 1) % pstQueue->nMaxDataCount;
  pstQueue->bIsLastOperation_Put = FALSE;

  return TRUE;
}
