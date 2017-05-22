/*
 * main.c
 *
 *  Created on: 2017. 5. 12.
 *      Author: hangstone
 */

#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

//  ���ڿ��� X, Y ��ġ�� ���
void kPrintString(int nX, int nY, const char* pszString);
//	IA-32e ���� Ŀ�� ������ '0'���� �ʱ�ȭ
//		1MB ~ 6MB ���� ������ �ʱ�ȭ
BOOL kInitializeKernel64Area(void);
//	HANG64 OS�� �����ϱ⿡ ����� �޸𸮸� ������ �ִ��� üũ
//		64MB �̻��� �޸𸮸� ������ �ִ��� �˻�
BOOL kIsMemoryEnough(void);
//	IA-32e ��� Ŀ���� 0x200000(2MB) ��巹���� ����
void kCopyKernel64ImageTo2MByte(void);

void Main(void)
{
	BOOL bRet = FALSE;
	DWORD dwEAX, dwEBX, dwECX, dwEDX;
	char szVendorString[13] = {0,};		//	������ ���ڿ��� ���� ���ڿ� ����
																		//	kPrintString() �Լ��� ����ϱ� ����

  kPrintString(0, 3, "Protected Mode C Language Kernel Start...........[Pass]");

  //	�ּ� �޸� ũ�⸦ �����ϴ� �� �˻�
  kPrintString(0, 4, "Minimum Memory Size Check........................[    ]");
  bRet = kIsMemoryEnough();
  if(FALSE == bRet)
  {
  	kPrintString(50, 4, "Fail");
  	kPrintString(0, 5, "Not Enough Memory!! HANG64 OS Requires Over 64MB Memory!!");

  	while(1);
  }
  else
  {
  	kPrintString(50, 4, "Pass");
  }

  //	IA-32e ����� Ŀ�� ������ �ʱ�ȭ
  kPrintString(0, 5, "IA-32e Kernel Area Initialize....................[    ]");
  bRet = kInitializeKernel64Area();
  if (FALSE == bRet)
  {
  	kPrintString(50, 5, "Fail");
  	kPrintString(0, 6, "Failed To Initialize Kernel Area");

  	while(1);
  }
  else
  {
  	kPrintString(50, 5, "Pass");
  }

  //	IA-32e ��� Ŀ���� ���� ������ ���̺� ����
  kPrintString(0, 6, "IA-32e Page Table Initialize.....................[    ]");
  kInitializePageTables();
  kPrintString(50, 6, "Pass");

  //	���μ��� ������ ���� ���
  kReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
  *(DWORD *)szVendorString = dwEBX;					//	���ڰ� ����� ������ ��������Ʈ���� ��������Ʈ��
  *((DWORD *)szVendorString + 1) = dwEDX;		//	�����̹Ƿ� �״�� ���ڿ� ���ۿ� �����ϸ� ���������� ��°���
  *((DWORD *)szVendorString + 2) = dwECX;		//	4-byte�� �ѹ��� �����Ϸ��� DWORD�� ĳ���� ��
  kPrintString(0, 7, "64bit Mode Vendor String.........................[            ]");
  kPrintString(50, 7, szVendorString);

  //	64��Ʈ ���� ���� Ȯ��
  kReadCPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);
  kPrintString(0, 8, "64bit Mode Support Check.........................[    ]");
  if (dwEDX & (1<<29))
  {
  	kPrintString(50, 8, "Pass");
  }
  else
  {
  	kPrintString(50, 8, "Fail");
  	kPrintString(0, 9, "This Processor does not support 64bit Mode!!");
  	while(1);
  }

  //	IA-32e ��� Ŀ���� 0x200000(2MB) ��巹���� �̵�
  kPrintString(0, 9, "Copy IA-32e Kernel to 2MB Address................[    ]");
  kCopyKernel64ImageTo2MByte();
  kPrintString(50, 9, "Pass");

  //	convert to 'IA-32e' mode
  kPrintString(0, 10, "Switch To IA-32e Mode");
  kSwitchAndExecute64BitKernel();

  while(1);
}

void kPrintString(int nX, int nY, const char* pszString)
{
  CHARACTER* pstScreen = (CHARACTER *)0xB8000;

  pstScreen += (nY * 80) + nX;
  for (int nIdx = 0; pszString[nIdx] != 0; ++nIdx)
  {
    pstScreen[nIdx].bCharacter = pszString[nIdx];
  }
}

BOOL kInitializeKernel64Area(void)
{
	DWORD* pCurrentAddress;

	//	�ʱ�ȭ�� ������ address�� 0x100000(1MB)�� ����
	pCurrentAddress = (DWORD *)0x100000;

	//	������ address�� 0x600000(6MB)���� ������ ���鼭 4-byte�� '0'���� ä��
	while (0x600000 > (DWORD)pCurrentAddress)
	{
		*pCurrentAddress = 0x00;

		//	'0'���� ������ ��, �ٽ� �о��� �� '0'�� ������ ������ �ش� address�� ����ϴ� ��
		//	������ ���� ���̹Ƿ� ���̻� �������� �ʰ� ����
		if (0 != *pCurrentAddress)
		{
			return FALSE;
		}

		//	move to next address
		pCurrentAddress++;
	}

	return TRUE;
}

BOOL kIsMemoryEnough(void)
{
	DWORD* pCurrentAddress;

	//	0x100000(1MB)���� �˻� ����
	pCurrentAddress = (DWORD *)0x100000;

	//	0x4000000(64MB)���� ������ ���鼭 Ȯ��
	while ((DWORD)pCurrentAddress < 0x4000000)
	{
		*pCurrentAddress = 0x12345678;

		//	0x12345678�� ������ �� �ٽ� �о��� �� 0x12345678�� ������ ������
		//	�ش� address�� ����ϴ� �� ������ ���� ���̹Ƿ� ���̻� �������� �ʰ� �����Ѵ�
		if (*pCurrentAddress != 0x12345678)
		{
			return FALSE;
		}

		//	1MB�� �̵��ϸ鼭 Ȯ��
		pCurrentAddress += (0x100000 / 4);
	}

	return TRUE;
}

void kCopyKernel64ImageTo2MByte(void)
{
	WORD 		wKernel32SectorCount;
	WORD 		wTotalKernelSectorCount;
	WORD		wIA32eSectorCount;
	DWORD* 	pSourceAddress;
	DWORD* 	pDestinationAddress;

	//	0x7C05�� �� Ŀ�� ���� ��, 0x7C08�� ��ȣ��� Ŀ�� ���� ���� ��� ����
	wTotalKernelSectorCount = *((WORD *) 0x7C05);
	wKernel32SectorCount = *((WORD *) 0x7C07);

	pSourceAddress = (DWORD *)(0x10000 + (wKernel32SectorCount * 512));
	pDestinationAddress = (DWORD *)0x200000;

	//	IA-32e ��� Ŀ�� ���� ũ�⸸ŭ ����
	wIA32eSectorCount = wTotalKernelSectorCount - wKernel32SectorCount;
	for (int nIdx=0; nIdx < 512 * wIA32eSectorCount / 4; nIdx++)
	{
		*pDestinationAddress = *pSourceAddress;
		pDestinationAddress++;
		pSourceAddress++;
	}
}
