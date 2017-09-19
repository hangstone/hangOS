/*
 * Queue.h
 *
 *  Created on: 2017. 9. 19.
 *      Author: hangstone
 */

#ifndef __QUEUE_H_
#define __QUEUE_H_

#include "Types.h"

/*
 *  structures
 */

//  1-byte로 정렬
#pragma pack (push, 1)

typedef struct kQueueManagerStruct
{
  int   nDataSize;      //  큐를 구성하는 데이터 하나의 크기
  int   nMaxDataCount;  //  큐에 삽입할 수 있는 최대 갯수

  void* pvQueueArray;   //  큐에서 데이터를 저장하는 용도로 사용할 버퍼
  int   nPutIndex;      //  데이터를 삽입할 때 사용하는 index
  int   nGetIndex;      //  데이터를 제거할 때 사용하는 index

  BOOL  bIsLastOperation_Put;   //  큐에 마지막으로 수행한 명령이 put인지 get인지 저장
                                //  큐의 버퍼가 비었는지 혹은 가득 찼는지 확인하는 용도
} QUEUE;

#pragma pack (pop)

/*
 *  functions
 */

/*
 *  큐를 초기화
 */
void kInitializeQueue(QUEUE* pstQueue, 
                      void* pvQueueBuffer, 
                      int nMaxDataCount, 
                      int nDataSize);

/*
 *  큐가 가득 찼는지의 여부를 반환
 */
BOOL kIsQueueFull(const QUEUE* pstQueue);

/*
 *  큐가 비어있는지의 여부를 반환
 */
BOOL kIsQueueEmpty(const QUEUE* pstQueue);

/*
 *  큐에 데이터를 삽입
 */
BOOL kPutQueue(QUEUE* pstQueue, const void* pvData);

/*
 *  큐에서 데이터를 제거
 */
BOOL kGetQueue(QUEUE* pstQueue, void* pvData);

#endif  /*__QUEUE_H_*/
