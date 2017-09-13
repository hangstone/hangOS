/*
 * Descriptor.h
 *
 *  Created on: 2017. 9. 11.
 *      Author: hangstone
 */

#ifndef __DESCRIPTOR_H_
#define __DESCRIPTOR_H_

#include "Types.h"

//==============================================================================
//  GDT
//==============================================================================
//  조합에 사용할 기본 macro
#define GDT_TYPE_CODE         0x0A
#define GDT_TYPE_DATA         0x02
#define GDT_TYPE_TSS          0x09
#define GDT_FLAGS_LOWER_S     0x10
#define GDT_FLAGS_LOWER_DPL0  0x00
#define GDT_FLAGS_LOWER_DPL1  0x20
#define GDT_FLAGS_LOWER_DPL2  0x40
#define GDT_FLAGS_LOWER_DPL3  0x60
#define GDT_FLAGS_LOWER_P     0x80
#define GDT_FLAGS_UPPER_L	    0x20
#define GDT_FLAGS_UPPER_DB    0x40
#define GDT_FLAGS_UPPER_G     0x80

//  실제로 사용할 macro
//  lower flag는 code/data/TSS, DPL0, Present로 설정
#define GDT_FLAGS_LOWER_KERNELCODE ( GDT_TYPE_CODE | \
                                     GDT_FLAGS_LOWER_S | \
                                     GDT_FLAGS_LOWER_DPL0 | \
                                     GDT_FLAGS_LOWER_P )
#define GDT_FLAGS_LOWER_KERNELDATA ( GDT_TYPE_DATA | \
                                     GDT_FLAGS_LOWER_S | \
                                     GDT_FLAGS_LOWER_DPL0 | \
                                     GDT_FLAGS_LOWER_P )
#define GDT_FLAGS_LOWER_TSS ( GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P )
#define GDT_FLAGS_LOWER_USERCODE ( GDT_TYPE_CODE | \
                                   GDT_FLAGS_LOWER_S | \
                                   GDT_FLAGS_LOWER_DPL3 | \
                                   GDT_FLAGS_LOWER_P )
#define GDT_FLAGS_LOWER_USERDATA ( GDT_TYPE_DATA | \
                                   GDT_FLAGS_LOWER_S | \
                                   GDT_FLAGS_LOWER_DPL3 | \
                                   GDT_FLAGS_LOWER_P )

//  upper flags는 granularity로 설정하고 코드 및 데이터는 64-bit 추가
#define GDT_FLAGS_UPPER_CODE ( GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L )
#define GDT_FLAGS_UPPER_DATA ( GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L )
#define GDT_FLAGS_UPPER_TSS ( GDT_FLAGS_UPPER_G )

//  segment descriptor offset
#define GDT_KERNELCODESEGMENT   0x08
#define GDT_KERNELDATASEGMENT   0x10
#define GDT_TSSSEGMENT          0x18

//  기타 GDT에 관련된 메크로
//  GDTR의 시작 address, 1-MB에서 264-KB까지는 페이지 테이블 영역
#define GDTR_STARTADDRESS       0x142000
//  8-byte 엔트리의 갯수, null descriptor / kernel code / kernel data
#define GDT_MAXENTRY8COUNT      3
//  16-byte 엔트리의 갯수, TSS
#define GDT_MAXENTRY16COUNT     1
//  GDT 테이블의 크기
#define GDT_TABLESIZE   ( (sizeof(GDTENTRY8) * GDT_MAXENTRY8COUNT) + \
                          (sizeof(GDTENTRY16) * GDT_MAXENTRY16COUNT) )
#define TSS_SEGMENTSIZE ( sizeof(TSSSEGMENT) )

//==============================================================================
//  IDT
//==============================================================================
//  조합에 사용할 기본 매크로
#define IDT_TYPE_INTERRUPT    0x0E
#define IDT_TYPE_TRAP         0x0F
#define IDT_FLAGS_DPL0        0x00
#define IDT_FLAGS_DPL1        0x20
#define IDT_FLAGS_DPL2        0x40
#define IDT_FLAGS_DPL3        0x60
#define IDT_FLAGS_P           0x80
#define IDT_FLAGS_IST0        0
#define IDT_FLAGS_IST1        1

//  실제로 사용할 매크로
#define IDT_FLAGS_KERNEL      ( IDT_FLAGS_DPL0 | IDT_FLAGS_P )
#define IDT_FLAGS_USER        ( IDT_FLAGS_DPL3 | IDT_FLAGS_P )

//  기타 IDT에 관련된 매크로
//  IDT 엔트리의 갯수
#define IDT_MAXENTRYCOUNT     100
//  IDT의 start address, TSS segment의 뒤쪽에 위치
#define IDTR_STARTADDRESS     ( GDTR_STARTADDRESS + sizeof(GDTR) + \
                                GDT_TABLESIZE + TSS_SEGMENTSIZE)
#define IDT_STARTADDRESS      ( IDTR_STARTADDRESS + sizeof(IDTR) )
//  IDT 테이블의 전체 크기
#define IDT_TABLESIZE         ( IDT_MAXENTRYCOUNT * sizeof(IDTENTRY))

//  IST의 start address
#define IST_STARTADDRESS      0x700000
//  IST의 크기
#define IST_SIZE              0x100000

//  구조체
//  1-byte로 정렬
#pragma pack(push ,1)

//  GDTR과 IDTR 구조체
typedef struct kGDTRStruct
{
  WORD  wLimit;
  QWORD qwBaseAddress;
  //  16-byte address 정렬을 위해 추가
  WORD  wPadding;
  DWORD dwPadding;
} GDTR, IDTR;

//  8-byte 크기의 GDT entry 구조
typedef struct kGDTEntry8Struct
{
  WORD  wLowerLimit;
  WORD  wLowerBaseAddress;
  BYTE  bUpperBaseAddress1;
  //  4-bit Type, 1-bit S, 2-bit DPL, 1-bit P
  BYTE  bTypeAndLowerFlag;
  //  4-bit Segment Limit, 1-bit AVL, L, D/B, G
  BYTE  bUpperLimitAndUpperFlag;
  BYTE  bUpperBaseAddress2;
} GDTENTRY8;

//  16-byte 크기의 GDT 엔트리 구조
typedef struct kGDTEntry16Struct
{
  WORD  wLowerLimit;
  WORD  wLowerBaseAddress;
  BYTE  bMiddleBaseAddress1;
  //  4-bit Type, 1-bit 0, 2-bit DPL, 1-bit P
  BYTE  bTypeAndLowerFlag;
  //  4-bit Segment Limit, 1-bit AVL, 0, 0, G
  BYTE  bUpperLimitAndUpperFlag;
  BYTE  bMiddleBaseAddress2;
  DWORD dwUpperBaseAddress;
  DWORD dwReserved;
} GDTENTRY16;

//  TSS Data Structure
typedef struct kTSSDataStruct
{
  DWORD dwReserved;
  QWORD qwRsp[3];
  QWORD qwReserved2;
  QWORD qwIST[7];
  QWORD qwReserved3;
  WORD  wReserved;
  WORD  wIOMapBaseAddress;
} TSSSEGMENT;

//  IDT Gate Descriptor Structure
typedef struct kIDTEntryStruct
{
  WORD  wLowerBaseAddress;
  WORD  wSegmentSelector;
  //  3-bit IST, 5-bit 0
  BYTE  bIST;
  //  4-bit Type, 1-bit 0, 2-bit DPL, 1-bit P
  BYTE  bTypeAndFlags;
  WORD  wMiddleBaseAddress;
  DWORD dwUpperBaseAddress;
  DWORD dwReserved;
} IDTENTRY;

#pragma pack (pop)


//  functions

//  GDT 테이블 초기화
void kInitializeGDTTableAndTSS(void);

//  8-byte 크기의 GDT entry에 값을 설정
//    코드와 데이터 세그먼트 디스크립터를 설정하는 데 사용
void kSetGDTEntry8(GDTENTRY8* pstEntry, 
                   DWORD dwBaseAddress,
                   DWORD dwLimit,
                   BYTE bUpperFlags,
                   BYTE bLowerFlags,
                   BYTE bType);

//  16-byte 크기의 GDT entry에 값을 설정
//    TSS segment descriptor를 설정하는 데 사용
void kSetGDTEntry16(GDTENTRY16* pstEntry,
                    QWORD qwBaseAddress,
                    DWORD dwLimit,
                    BYTE bUpperFlags,
                    BYTE bLowerFlags,
                    BYTE bType);

//  TSS segment의 정보를 초기화
void kInitializeTSSSegment(TSSSEGMENT* pstTSS);

//  IDT 테이블을 초기화
void kInitializeIDTTables(void);

//  IDT 게이트 디스크립터에 값을 설정
void kSetIDTEntry(IDTENTRY* pstEntry,
                  void* pvHandler,
                  WORD wSelector,
                  BYTE bIST,
                  BYTE bFlags,
                  BYTE bType);

//  임시 예외 또는 인터럽트 핸들러
void kDummyHandler(void);

#endif /*__DESCRIPTOR_H_ */
