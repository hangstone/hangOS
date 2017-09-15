/*
 * PIC.h
 *
 *  Created on: 2017. 9. 14.
 *      Author: hangstone
 */

#ifndef __PIC_H_
#define __PIC_H_

#include "Types.h"

//  macro
//  define I/O port
#define PIC_MASTER_PORT1    0x20
#define PIC_MASTER_PORT2    0x21
#define PIC_SLAVE_PORT1     0xA0
#define PIC_SLAVE_PORT2     0xA1

//  IDT 테이블에서 인터럽트 벡터가 시작되는 위치, 0x20
#define PIC_IRQSTARTVECTOR  0x20

#define NUMBER_OF_MASTER_IRQ  8

//  functions
/*
 *  PIC(Programmable Interrupt Controller)를 초기화
 */
void  kInitializePIC(void);

/*
 *  인터럽트를 마스크하여 해당 인터럽트가 발생하지 않도록 처리
 */
void  kMaskPICInterrupt(WORD wIRQBitMask);

/*
 *  인터럽트 처리가 완료되었음을 전송 (EOI)
 *    master PIC의 경우, master PIC에만 EOI 전송
 *    slave PIC의 경우, master 및 slave PIC에 모두 EOI 전송
 */
void  kSendEOIToPIC(int nIRQNumber);

#endif  // __PIC_H_