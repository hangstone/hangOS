/*
 * Descriptor.c
 *
 *  Created on: 2017. 9. 12.
 *      Author: hangstone
 */

#include "Descriptor.h"
#include "Utility.h"
#include "ISR.h"

//==============================================================================
// GDT 및 TSS
//==============================================================================

void kInitializeGDTTableAndTSS(void)
{
  GDTR* pstGDTR;
  GDTENTRY8* pstEntry;
  TSSSEGMENT* pstTSS;
  
  // GDTR 설정
  pstGDTR = (GDTR *)GDTR_STARTADDRESS;
  pstEntry = (GDTENTRY8 *)(GDTR_STARTADDRESS + sizeof(GDTR));
  pstGDTR->wLimit = GDT_TABLESIZE - 1;
  pstGDTR->qwBaseAddress = (QWORD)pstEntry;
  
  // TSS 영역 설정
  pstTSS = (TSSSEGMENT *)((QWORD)pstEntry + GDT_TABLESIZE);
  
  // NULL, 64-bit Code/Data, TSS를 위해 총 4개의 세그먼트를 생성한다
  kSetGDTEntry8(&(pstEntry[0]), 0, 0, 0, 0, 0);
  kSetGDTEntry8(&(pstEntry[1]),
                0,
                0xFFFFF,
                GDT_FLAGS_UPPER_CODE,
                GDT_FLAGS_LOWER_KERNELCODE,
                GDT_TYPE_CODE);
  kSetGDTEntry8(&(pstEntry[2]),
                0,
                0xFFFFF,
                GDT_FLAGS_UPPER_DATA,
                GDT_FLAGS_LOWER_KERNELDATA,
                GDT_TYPE_DATA);
  kSetGDTEntry16( (GDTENTRY16 *) &(pstEntry[3]),
                  (QWORD)pstTSS,
                  sizeof(TSSSEGMENT) - 1,
                  GDT_FLAGS_UPPER_TSS,
                  GDT_FLAGS_LOWER_TSS,
                  GDT_TYPE_TSS);

  //  TSS 초기화 GDT 이하 영역을 사용함
  kInitializeTSSSegment(pstTSS);
}

void kSetGDTEntry8(GDTENTRY8* pstEntry, 
                   DWORD dwBaseAddress,
                   DWORD dwLimit,
                   BYTE bUpperFlags,
                   BYTE bLowerFlags,
                   BYTE bType)
{
  pstEntry->wLowerLimit = dwLimit & 0xFFFF;
  pstEntry->wLowerBaseAddress = dwBaseAddress & 0xFFFF;
  pstEntry->bUpperBaseAddress1 = (dwBaseAddress >> 16) & 0xFF;
  pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;
  pstEntry->bUpperLimitAndUpperFlag = ((dwLimit >> 16) & 0x0F) | bUpperFlags;
  pstEntry->bUpperBaseAddress2 = (dwBaseAddress >> 24) & 0xFF;
}

void kSetGDTEntry16(GDTENTRY16* pstEntry,
                    QWORD qwBaseAddress,
                    DWORD dwLimit,
                    BYTE bUpperFlags,
                    BYTE bLowerFlags,
                    BYTE bType)
{
  pstEntry->wLowerLimit = dwLimit & 0xFFFF;
  pstEntry->wLowerBaseAddress = qwBaseAddress & 0xFFFF;
  pstEntry->bMiddleBaseAddress1 = (qwBaseAddress >> 16) & 0xFF;
  pstEntry->bTypeAndLowerFlag = bLowerFlags | bType;
  pstEntry->bUpperLimitAndUpperFlag = ((dwLimit >> 16) & 0x0F) | bUpperFlags;
  pstEntry->bMiddleBaseAddress2 = (qwBaseAddress >> 24) & 0xFF;
  pstEntry->dwUpperBaseAddress = qwBaseAddress >> 32;
  pstEntry->dwReserved = 0;
}

void kInitializeTSSSegment(TSSSEGMENT* pstTSS)
{
  kMemSet(pstTSS, 0, sizeof(TSSSEGMENT));
  pstTSS->qwIST[0] = IST_STARTADDRESS + IST_SIZE;
  pstTSS->wIOMapBaseAddress = 0xFFFF;   //  IO를 TSS의 limit 값보다 크게 설정함으로써 
                                        //  IO Map을 사용하지 않도록 함
}

//==============================================================================
//  IST
//==============================================================================
void kInitializeIDTTables(void)
{
  IDTR* pstIDTR;
  IDTENTRY* pstEntry;

  //  IDTR의 start address
  pstIDTR = (IDTR *)IDTR_STARTADDRESS;
  //  IDT 테이블의 정보 생성
  pstEntry = (IDTENTRY *)(IDTR_STARTADDRESS + sizeof(IDTR));
  pstIDTR->qwBaseAddress = (QWORD)pstEntry;
  pstIDTR->wLimit = IDT_TABLESIZE - 1;

  //======================================================================
  //  예외 ISR 등록
  //======================================================================
  kSetIDTEntryForSimple(&(pstEntry[0]), kISRDivideError);
  kSetIDTEntryForSimple(&(pstEntry[1]), kISRDebug);
  kSetIDTEntryForSimple(&(pstEntry[2]), kISRNMI);
  kSetIDTEntryForSimple(&(pstEntry[3]), kISRBreakPoint);
  kSetIDTEntryForSimple(&(pstEntry[4]), kISROverflow);
  kSetIDTEntryForSimple(&(pstEntry[5]), kISRBoundRangeExceeded);
  kSetIDTEntryForSimple(&(pstEntry[6]), kISRInvaliedOpcode);
  kSetIDTEntryForSimple(&(pstEntry[7]), kISRDeviceNotAvailable);
  kSetIDTEntryForSimple(&(pstEntry[8]), kISRDoubleFault);
  kSetIDTEntryForSimple(&(pstEntry[9]), kISRCoprocessorSegmentOverrun);
  kSetIDTEntryForSimple(&(pstEntry[10]), kISRInvalidTSS);
  kSetIDTEntryForSimple(&(pstEntry[11]), kISRSegmentNotPresent);
  kSetIDTEntryForSimple(&(pstEntry[12]), kISRStackSegmentFault);
  kSetIDTEntryForSimple(&(pstEntry[13]), kISRGeneralProtection);
  kSetIDTEntryForSimple(&(pstEntry[14]), kISRPageFault);
  kSetIDTEntryForSimple(&(pstEntry[15]), kISR15);
  kSetIDTEntryForSimple(&(pstEntry[16]), kISRFPUError);
  kSetIDTEntryForSimple(&(pstEntry[17]), kISRAlignmentCheck);
  kSetIDTEntryForSimple(&(pstEntry[18]), kISRMachineCheck);
  kSetIDTEntryForSimple(&(pstEntry[19]), kISRSMDError);
  kSetIDTEntryForSimple(&(pstEntry[20]), kISRException);

  for (int nIdx=21; nIdx<32; nIdx++)
  {
    kSetIDTEntryForSimple(&(pstEntry[nIdx]), kISRException);
  }
  
  //======================================================================
  //  예외 ISR 등록
  //======================================================================
  kSetIDTEntryForSimple(&(pstEntry[32]), kISRTimer);
  kSetIDTEntryForSimple(&(pstEntry[33]), kISRSlavePIC);
  kSetIDTEntryForSimple(&(pstEntry[34]), kISRKeyboard);
  kSetIDTEntryForSimple(&(pstEntry[35]), kISRSerialPort2);
  kSetIDTEntryForSimple(&(pstEntry[36]), kISRSerialPort1);
  kSetIDTEntryForSimple(&(pstEntry[37]), kISRParallelPort2);
  kSetIDTEntryForSimple(&(pstEntry[38]), kISRFloppyDiskController);
  kSetIDTEntryForSimple(&(pstEntry[39]), kISRParallelPort1);
  kSetIDTEntryForSimple(&(pstEntry[40]), kISRRTC);
  kSetIDTEntryForSimple(&(pstEntry[41]), kISRReserved);
  kSetIDTEntryForSimple(&(pstEntry[42]), kISRNotUsed1);
  kSetIDTEntryForSimple(&(pstEntry[43]), kISRNotUsed2);
  kSetIDTEntryForSimple(&(pstEntry[44]), kISRMouse);
  kSetIDTEntryForSimple(&(pstEntry[45]), kISRCoProcessor);
  kSetIDTEntryForSimple(&(pstEntry[46]), kISRHardDisk1);
  kSetIDTEntryForSimple(&(pstEntry[47]), kISRHardDisk2);

  for (int nIdx=48; nIdx<IDT_MAXENTRYCOUNT; nIdx++)
  {
    kSetIDTEntryForSimple(&(pstEntry[nIdx]), kISREtcInterrupts);
  }
  
  //  0~99까지 vector를 모두 DummyHandler로 연결
  for (int nIdx = 0; nIdx < IDT_MAXENTRYCOUNT; nIdx++)
  {
    kSetIDTEntry(&(pstEntry[nIdx]),
                 kDummyHandler,
                 0x08,
                 IDT_FLAGS_IST1,
                 IDT_FLAGS_KERNEL,
                 IDT_TYPE_INTERRUPT);
  }
}

void kSetIDTEntry(IDTENTRY* pstEntry,
                  void* pvHandler,
                  WORD wSelector,
                  BYTE bIST,
                  BYTE bFlags,
                  BYTE bType)
{
  pstEntry->wLowerBaseAddress = (QWORD)pvHandler & 0xFFFF;
  pstEntry->wSegmentSelector = wSelector;
  pstEntry->bIST = bIST & 0x3;
  pstEntry->bTypeAndFlags = bType | bFlags;
  pstEntry->wMiddleBaseAddress = ((QWORD)pvHandler >> 16) & 0xFFFF;
  pstEntry->dwUpperBaseAddress = (QWORD)pvHandler >> 32;
  pstEntry->dwReserved = 0;
}

void kSetIDTEntryForSimple(IDTENTRY* pstEntry, void* pvHandler)
{
  kSetIDTEntry( pstEntry,
                pvHandler,
                0x08,
                IDT_FLAGS_IST1,
                IDT_FLAGS_KERNEL,
                IDT_TYPE_INTERRUPT );
}

void kDummyHandler(void)
{
  kPrintString(0, 0,  "=======================================================");
  kPrintString(0, 1,  "          Dummy Interrupt Handler Execute~!!!          ");
  kPrintString(0, 2,  "            Interrupt or Exception Occur~!!            ");
  kPrintString(0, 3,  "=======================================================");

  while(1);
}