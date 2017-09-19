/*
 * Utility.h
 *
 *  Created on: 2017. 9. 12.
 *      Author: hangstone
 */

#ifndef __UTILITY_H_
#define __UTILITY_H_

#include "Types.h"

//  메모리를 특정 값으로 채움
void  kMemSet(void* pvDestination, BYTE bData, int nSize);

//  메모리 복사
int   kMemCpy(void* pvDestination, const void* pvSource, int nSize);

//  메모리 비교
int   kMemCmp(const void* pvDestination, const void* pvSource, int nSize);

//  RFLAGS 레지스터의 인터럽트 플래그를 변경하고 이전 인터럽트 플래그의 상태를 반환
BOOL  kSetInterruptFlag(BOOL bEnableInterrupt);

#endif /*__UTILITY_H_*/