/*
 * main.c
 *
 *  Created on: 2017. 5. 12.
 *      Author: hangstone
 */

#include "Types.h"

void kPrintString(int nX, int nY, const char* pszString);
BOOL kInitializeKernel64Area(void);
BOOL kIsMemoryEnough(void);

void Main(void)
{
	BOOL bRet = FALSE;
  kPrintString(0, 3, "C Language Kernel Started........................[Pass]");

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



  while(1);
}

//  ���ڿ� ��� �Լ�
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
