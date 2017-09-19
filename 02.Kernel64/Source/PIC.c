/*
 * PIC.c
 *
 *  Created on: 2017. 9. 14.
 *      Author: hangstone
 */

#include "PIC.h"
#include "AssemblyUtility.h"

void  kInitializePIC(void)
{
  /* mater PIC 초기화 */
  //  ICW1(port 0x20), IC4 비트('0'번째 bit) = 1
  kOutPortByte(PIC_MASTER_PORT1, 0x11);

  //  ICW2(port 0x21), interrupt vector(0x20)
  kOutPortByte(PIC_MASTER_PORT2, PIC_IRQSTARTVECTOR);

  //  ICW3(port 0x21), slave PIC가 연결 위치(비트로 표현)
  //  mater PIC의 2번 pin에 연결되어 있으므로, 0x04('2'번째 bit)로 설정
  kOutPortByte(PIC_MASTER_PORT2, 0x04);

  //  ICW4(port 0x21), uPM 비트('0'번째 bit) = 1
  kOutPortByte(PIC_MASTER_PORT2, 0x01);

  /* slave PIC 초기화 */
  //  ICW1(port 0xA0), IC4 비트('0'번째 bit) = 1
  kOutPortByte(PIC_SLAVE_PORT1, 0x11);
  
  //  ICW2(port 0xA1), interrupt vector(0x20 + 8)
  kOutPortByte(PIC_SLAVE_PORT2, PIC_IRQSTARTVECTOR + 8);
  
  //  ICW3(port 0xA1), slave PIC가 연결 위치(비트로 표현)
  //  mater PIC의 2번 pin에 연결되어 있으므로, 0x02로 설정
  kOutPortByte(PIC_SLAVE_PORT2, 0x02);

  //  ICW4(port 0xA1), uPM 비트('0'번째 bit) = 1
  kOutPortByte(PIC_SLAVE_PORT2, 0x01);
}

void  kMaskPICInterrupt(WORD wIRQBitMask)
{
  //  master PIC에 IMR 설정
  //  OCW1(port 0x21), IRQ 0 ~ IRQ 7
  kOutPortByte(PIC_MASTER_PORT2, (BYTE)wIRQBitMask);  //  하위 8-bit는 master PIC에 연결되어 있으므로 
               //master PIC의 OCW1 port                //  OCW1 커맨드에 하위 8-bit만 전송
  
  //  slave PIC에 IMR 설정
  //  OCW1(port 0xA1), IRQ 8 ~ IRQ 15
  kOutPortByte(PIC_SLAVE_PORT2, (BYTE)(wIRQBitMask >> 8));  //  상위 8-bit는 slave PIC에 연결되어 있으므로 
               //master PIC의 OCW1 port                     //  OCW1 커맨드에 상위 8-bit만 전송
}

void  kSendEOIToPIC(int nIRQNumber)
{
  //  master PIC에 EOI 전송
  //  OCW2(port 0x20), EOI 비트(5번째 bit) = 1
  kOutPortByte(PIC_MASTER_PORT1, 0x20);

  //  slave PIC의 인터럽트인 경우 slave PIC에게도 EOI 전송
  if (NUMBER_OF_MASTER_IRQ <= nIRQNumber)
  {
    //  OCW2(port 0xA0), EOI 비트(5번째 bit) = 1
    kOutPortByte(PIC_SLAVE_PORT1, 0x20);
  }
}
