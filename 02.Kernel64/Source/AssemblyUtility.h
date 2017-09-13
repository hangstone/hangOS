/*
 * AssemblyUtility.h
 *
 *  Created on: 2017. 5. 24.
 *      Author: hangstone
 */

#ifndef __ASSEMBLYUTILITY_H_
#define __ASSEMBLYUTILITY_H_

#include "Types.h"

/*
 * Functions
 */

/*
 * 포트로부터 한바이트를 읽음
 */
BYTE  kInPortByte(WORD wPort);

/*
 * 포트에 한바이트를 씀
 */
void  kOutPortByte(WORD wPort, BYTE bData);


/*
 * GDTR 레지스터에 GDT 테이블을 설정
 */
void kLoadGDTR(QWORD qwGDTRAddress);

/*
 * TR 레지스터에 TSS 세그먼트 디스크립터 설정
 */
void kLoadTR(WORD wTSSSegmentOffset);

/*
 * IDTR 레지스터에 IDT 테이블을 설정
 */
void kLoadIDTR(QWORD qwIDTRAddress);

#endif /* __ASSEMBLYUTILITY_H_ */
