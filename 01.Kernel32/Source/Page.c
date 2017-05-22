/*
 * Page.c
 *
 *  Created on: 2017. 5. 17.
 *      Author: hangstone
 */


#include "Page.h"

void kInitializePageTables(void)
{
	PML4TENTRY*	pstPML4TEntry;
	PDPTENTRY*	pstPDPTEntry;
	PDENTRY*		pstPDEntry;
	DWORD				dwMappingAddress;

	//	create PML4(Page Map Level 4) table
	//	첫번쨰 엔트리 외에 나머지는 모두 '0'으로 초기화
	pstPML4TEntry = (PML4TENTRY *)(PAGE_PML4_DEFAULT_ADDRESS);
	kSetPageEntryData(&(pstPML4TEntry[0]), 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0);
	for (int nIdx=1; nIdx<PAGE_MAXENTRYCOUNT; nIdx++)
	{
		kSetPageEntryData(&(pstPML4TEntry[nIdx]), 0, 0, 0, 0);
	}

	//	create PDP(Page Directory Pointer) table
	//	하나의 PDPT로 512GB까지 mapping이 가능하므로 하나로 충분함
	//	64개의 entry를 설정하여 64GB까지 mapping함
	pstPDPTEntry = (PDPTENTRY *)(PAGE_PDP_DEFAULT_ADDRESS);
	for (int nIdx=0; nIdx<64; nIdx++)
	{
		kSetPageEntryData(&(pstPDPTEntry[nIdx]),
											0,
											0x102000 + (nIdx * PAGE_TABLESIZE),
											PAGE_FLAGS_DEFAULT,
											0);
	}
	for (int nIdx=64; nIdx<PAGE_MAXENTRYCOUNT; nIdx++)
	{
		kSetPageEntryData(&(pstPDPTEntry[nIdx]),
											0,
											0,
											0,
											0);
	}

	//	create PD(page directory) table
	//	하나의 페이지 디렉터리가 1GB까지 mapping 가능
	//	여유있게 64개의 page directory를 생성하여 총 64GB까지 지원
	pstPDEntry = (PDENTRY *)(PAGE_PDE_DEFAULT_ADDRESS);
	dwMappingAddress = 0;
	for (int nIdx=0; nIdx<PAGE_MAXENTRYCOUNT*64; nIdx++)
	{
		//	32-bit로는 상위 address를 표현할 수 없으므로, MB 단위로 계산한 다음
		//	최종 결과를 다시 4KB로 나누어 32-bit 이상의 address를 계산함
		kSetPageEntryData(&(pstPDEntry[nIdx]),
											(nIdx * (PAGE_DEFAULTSIZE >> 20)) >> 12,
											dwMappingAddress,
											PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS,
											0);
		dwMappingAddress += PAGE_DEFAULTSIZE;
	}
}

void kSetPageEntryData(PTENTRY* pstEntry,
											 DWORD dwUpperBaseAddress,
											 DWORD dwLowerBaseAddress,
											 DWORD dwLowerFlags,
											 DWORD dwUpperFlags)
{
	pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
	pstEntry->dwUpperBaseAddressAndEXB = (dwUpperBaseAddress & 0xFF) | dwUpperFlags;
}
