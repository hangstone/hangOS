/*
 * Interrupt Handler.h
 *
 *  Created on: 2017. 9. 15.
 *      Author: hangstone
 */

#ifndef __INTERRUPTHANDLER_H_
#define __INTERRUPTHANDLER_H_

#include "Types.h"

//  functions

/*
 *  공통으로 사용하는 예외 핸들러
 */
void  kCommonExceptionHandler(int nVectorNumber, QWORD qwErrorCode);

/*
 *  공통으로 사용하는 인터럽트 핸들러
 */
void  kCommonInterruptHandler(int nVectorNumber);

/*
 *  키보드 인터럽트 핸들러
 */
void  kKeyboardHandler(int nVectorNumber);

#endif  // __INTERRUPTHANDLER_H_