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
	//	ù���� ��Ʈ�� �ܿ� �������� ��� '0'���� �ʱ�ȭ
	pstPML4TEntry = (PML4TENTRY *)(PAGE_PML4_DEFAULT_ADDRESS);
	kSetPageEntryData(&(pstPML4TEntry[0]), 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0);
	for (int nIdx=1; nIdx<PAGE_MAXENTRYCOUNT; nIdx++)
	{
		kSetPageEntryData(&(pstPML4TEntry[nIdx]), 0, 0, 0, 0);
	}

	//	create PDP(Page Directory Pointer) table
	//	�ϳ��� PDPT�� 512GB���� mapping�� �����ϹǷ� �ϳ��� �����
	//	64���� entry�� �����Ͽ� 64GB���� mapping��
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
	//	�ϳ��� ������ ���͸��� 1GB���� mapping ����
	//	�����ְ� 64���� page directory�� �����Ͽ� �� 64GB���� ����
	pstPDEntry = (PDENTRY *)(PAGE_PDE_DEFAULT_ADDRESS);
	dwMappingAddress = 0;
	for (int nIdx=0; nIdx<PAGE_MAXENTRYCOUNT*64; nIdx++)
	{
		//	32-bit�δ� ���� address�� ǥ���� �� �����Ƿ�, MB ������ ����� ����
		//	���� ����� �ٽ� 4KB�� ������ 32-bit �̻��� address�� �����
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
