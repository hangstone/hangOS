/*
 * Page.h
 *
 *  Created on: 2017. 5. 17.
 *      Author: hangstone
 */

#ifndef __PAGE_H_
#define __PAGE_H_

#include "Types.h"

//	Macros
#define PAGE_FLAGS_P				0x00000001		//	Present
#define	PAGE_FLAGS_RW				0x00000002		//	Read/Write
#define	PAGE_FLAGS_US				0x00000004		//	User/Supervisor (flag ���� �� ���� ����)
#define	PAGE_FLAGS_PWT			0x00000008		//	Page Level Write-Through
#define	PAGE_FLAGS_PCD			0x00000010		//	Page Level Cache Disable
#define	PAGE_FLAGS_A				0x00000020		//	Accessed
#define	PAGE_FLAGS_D				0x00000040		//	Dirty
#define	PAGE_FLAGS_PS				0x00000080		//	Page Size
#define	PAGE_FLAGS_G				0x00000100		//	Global
#define	PAGE_FLAGS_PAT			0x00001000		//	Page Attribute Table Index
#define	PAGE_FLAGS_EXB			0x80000000		//	Execute Disable Bit
#define	PAGE_FLAGS_DEFAULT	(PAGE_FLAGS_P | PAGE_FLAGS_RW)
#define	PAGE_TABLESIZE			0x1000
#define	PAGE_MAXENTRYCOUNT	512
#define	PAGE_DEFAULTSIZE		0x200000
//	PML4 ���̺��� 0x100000(1MB)���� �����Ͽ� 4KB�� ������ ����
#define PAGE_PML4_DEFAULT_ADDRESS		0x100000
//	PDP ���̺��� 0x100000(1MB)���� �����Ͽ� 4KB�� ������ ����
#define PAGE_PDP_DEFAULT_ADDRESS		0x101000
//	PD �� ���� �ּ�
#define PAGE_PDE_DEFAULT_ADDRESS		0x102000

//	structures
#pragma pack(push, 1)

typedef struct kPageTableEntry		//	data structure for page entry
{
	//	PML4T�� PDPTE�� ���
	//	1-bit: P, RW, US, PWT, A
	//	3-bit: Reserved, Avail
	//	20-bit: Base Address
	//
	//	PDE�� ���
	//	1-bit: P, RW, US, PWT, PCD, A, D, PS, G
	//	3-bit: Avail
	//	1-bit: PAT
	//	8-bit: Reserved
	//	11-bit: Base Address
	DWORD dwAttributeAndLowerBaseAddress;
	//	8-bit: Upper BaseAddress
	//	12-bit: Reserved
	//	11-bit: Avail
	//	1-bit: EXB
	DWORD dwUpperBaseAddressAndEXB;
} PML4TENTRY, PDPTENTRY, PDENTRY, PTENTRY;

#pragma pack(pop)


//	functions

//	IA-32e ��� Ŀ���� ���� ������ ���̺� ����
void kInitializePageTables(void);

//	������ ��Ʈ���� ���� �ּҿ� �Ӽ� �÷��׸� ����
void kSetPageEntryData(PTENTRY* pstEntry,
											 DWORD dwUpperBaseAddress,
											 DWORD dwLowerBaseAddress,
											 DWORD dwLowerFlags,
											 DWORD dwUpperFlags);

#endif /* __PAGE_H_ */
