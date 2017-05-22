/*
 * ModeSwitch.h
 *
 *  Created on: 2017. 5. 18.
 *      Author: hangstone
 */

#ifndef __MODESWITCH_H_
#define __MODESWITCH_H_

#include "Types.h"

//	CPUID를 반환
void kReadCPUID(DWORD dwEAX,
								DWORD* pdwEAX,
								DWORD* pdwEBX,
								DWORD* pdwECX,
								DWORD* pdwEDX);

//	IA-32e 모드로 전환하고 64비트 커널을 수행
void kSwitchAndExecute64BitKernel(void);

#endif /* __MODESWITCH_H_ */
