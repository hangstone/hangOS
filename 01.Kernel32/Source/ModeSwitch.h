/*
 * ModeSwitch.h
 *
 *  Created on: 2017. 5. 18.
 *      Author: hangstone
 */

#ifndef __MODESWITCH_H_
#define __MODESWITCH_H_

#include "Types.h"

//	CPUID�� ��ȯ
void kReadCPUID(DWORD dwEAX,
								DWORD* pdwEAX,
								DWORD* pdwEBX,
								DWORD* pdwECX,
								DWORD* pdwEDX);

//	IA-32e ���� ��ȯ�ϰ� 64��Ʈ Ŀ���� ����
void kSwitchAndExecute64BitKernel(void);

#endif /* __MODESWITCH_H_ */
