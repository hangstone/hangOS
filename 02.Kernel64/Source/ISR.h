/*
 * ISR.h
 *
 *  Created on: 2017. 9. 14.
 *      Author: hangstone
 */

#ifndef __ISR_H_
#define __ISR_H_

//  functions
//  예외 처리용 ISR

/*
 *  #0, Divide Error ISR(In-Service Register)
 */
void  kISRDivideError(void);

/*
 *  #1, Debug ISR
 */
void  kISRDebug(void);

/*
 *  #2, NMI ISR
 */
void  kISRNMI(void);

/*
 *  #3, BreakPoint ISR
 */
void  kISRBreakPoint(void);

/*
 *  #4, Overflow ISR
 */
void  kISROverflow(void);

/*
 *  #5, Bound Range Exceeded ISR
 */
void  kISRBoundRangeExceeded(void);

/*
 *  #6, Invalid Opcode ISR
 */
void  kISRInvaliedOpcode(void);

/*
 *  #7, Device Not Available ISR
 */
void  kISRDeviceNotAvailable(void);

/*
 *  #8, Double Fault ISR 
 */
void  kISRDoubleFault(void);

/*
 *  #9, Coprocessor Segment Overrun ISR
 */
void  kISRCoprocessorSegmentOverrun(void);

/*
 *  #10, Invalid TSS ISR
 */
void  kISRInvalidTSS(void);

/*
 *  #11, Segment Not Present ISR
 */
void  kISRSegmentNotPresent(void);

/*
 *  #12, Stack Segment Fault ISR
 */
void  kISRStackSegmentFault(void);

/*
 *  #13, General Protection ISR
 */
void  kISRGeneralProtection(void);

/*
 *  #14, Page Fault ISR
 */
void  kISRPageFault(void);

/*
 *  #15, Reserved ISR
 */
void  kISR15(void);

/*
 *  #16, FPU Error ISR
 */
void  kISRFPUError(void);

/*
 *  #17, Alignment Check ISR
 */
void  kISRAlignmentCheck(void);

/*
 *  #18, Machine Check ISR
 */
void  kISRMachineCheck(void);

/*
 *  #19, SIMD Floating Point Exception ISR(In-Service Engineer)
 */
void  kISRSIMDError(void);

/*
 *  #20 ~ #31, Reserved ISR
 */
void  kISREtcException(void);


//  interrupt 처리용 ISR
/*
 *  #32, 타이머 ISR
 */
void  kISRTimer(void);

/*
 *  #33, 키보드 ISR
 */
void  kISRKeyboard(void);

/*
 *  #34, 슬레이브 PIC ISR
 */
void  kISRSlavePIC(void);

/*
 *  #35, 시리얼 포트 2 ISR
 */
void  kISRSerialPort2(void);

/*
 *  #36, 시리얼 포트 1 ISR
 */
void  kISRSerialPort1(void);

/*
 *  #37, 패럴렐 포트 2 ISR
 */
void  kISRParallelPort2(void);

/*
 *  #38, 플로피 디스크 컨트롤러 ISR
 */
void  kISRFloppyDiskController(void);

/*
 *  #39, 패럴렐 포트 1 ISR
 */
void  kISRParallelPort1(void);

/*
 *  #40, RTC ISR
 */
void  kISRRTC(void);

/*
 *  #41, 예약된 인터럽트의 ISR
 */
void  kISRReserved(void);

/*
 *  #42, 사용 안함 1
 */
void  kISRNotUsed1(void);

/*
 *  #43, 사용 안함 2
 */
void  kISRNotUsed2(void);

/*
 *  #44, 마우스 ISR
 */
void  kISRMouse(void);

/*
 *  #45, 코-프로세서 ISR
 */
void  kISRCoProcessor(void);

/*
 *  #46, 하드 디스크 1 ISR
 */
void  kISRHardDisk1(void);

/*
 *  #47, 하드 디스크 2 ISR
 */
void  kISRHardDisk2(void);

/*
 *  #48, 이외의 모든 인터럽트에 대한 ISR
 */
void  kISREtcInterrupts(void);

#endif  // __ISR_H_