/*
 * Utility.h
 *
 *  Created on: 2017. 9. 12.
 *      Author: hangstone
 */

#ifndef __UTILITY_H_
#define __UTILITY_H_

#include <stdarg.h>
#include "Types.h"

/*
 *  Macro
 */
#define NULL_CHAR                     ('\0')

/*
 *  메모리를 특정 값으로 채움
 */
void  kMemSet(void* pvDestination, BYTE bData, int nSize);

/*
 *  메모리 복사
 */
int   kMemCpy(void* pvDestination, const void* pvSource, int nSize);

/*
 *  메모리 비교
 */
int   kMemCmp(const void* pvDestination, const void* pvSource, int nSize);

/*
 *  RFLAGS 레지스터의 인터럽트 플래그를 변경하고 이전 인터럽트 플래그의 상태를 반환
 */
BOOL  kSetInterruptFlag(BOOL bEnableInterrupt);

/*
 *  문자열의 길이를 반환
 */
int   kStrLen(const char* pBuffer);

/*
 *  64MB 이상의 위치부터 램 크기를 체크
 *    caution: 최초 부팅과정에서 한번만 호출 해야 함!!
 */
int   kCheckTotalRAMSize(void);

/*
 *  RAM 크기를 반환
 */
QWORD kGetTotalRAMSize(void);

/*
 *  atoi()를 구현한 함수
 */
long  kAtoI(const char* pBuffer, int nRadix);

/*
 *  16진수 문자열을 QWORD로 변환
 */
QWORD kHexStringToQword(const char* pBuffer);

/*
 *  10진수 문자열을 long으로 변환
 */
long  kDecimalStringToLong(const char* pBuffer);

/*
 *  itoa()를 구현한 함수
 */
int   kItoA(long lValue, char* pBuffer, int nRadix);

/*
 *  16진수 값을 문자열로 변환
 */
int   kHexToString(QWORD qwValue, char* pBuffer);

/*
 *  10진수 값을 문자열로 변환
 */
int   kDecimalToString(long lValue, char* pBuffer);

/*
 *  문자열의 순서를 뒤집는 함수
 */
void  kReverseString(char* pBuffer);

/*
 *  sprintf()를 구현한 함수
 */
int   kSprintf(char* pBuffer, const char* pFormatString, ...);

/*
 *  vsprintf()를 구현한 함수
 */
int   kVSPrintf(char* pBuffer, const char* pFormatString, va_list ap);

#endif /*__UTILITY_H_*/