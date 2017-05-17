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

  //	최소 메모리 크기를 만족하는 지 검사
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

  //	IA-32e 모드의 커널 영역을 초기화
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

//  문자열 출력 함수
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

	//	초기화를 시작할 address인 0x100000(1MB)를 설정
	pCurrentAddress = (DWORD *)0x100000;

	//	마지막 address인 0x600000(6MB)까지 루프를 돌면서 4-byte씩 '0'으로 채움
	while (0x600000 > (DWORD)pCurrentAddress)
	{
		*pCurrentAddress = 0x00;

		//	'0'으로 저장한 후, 다시 읽었을 때 '0'이 나오지 않으면 해당 address를 사용하는 데
		//	문제가 생긴 것이므로 더이상 진행하지 않고 종료
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

	//	0x100000(1MB)부터 검사 시작
	pCurrentAddress = (DWORD *)0x100000;

	//	0x4000000(64MB)까지 루프를 돌면서 확인
	while ((DWORD)pCurrentAddress < 0x4000000)
	{
		*pCurrentAddress = 0x12345678;

		//	0x12345678로 저장한 후 다시 읽었을 때 0x12345678이 나오지 않으면
		//	해당 address를 사용하는 데 문제가 생긴 것이므로 더이상 진행하지 않고 종료한다
		if (*pCurrentAddress != 0x12345678)
		{
			return FALSE;
		}

		//	1MB씩 이동하면서 확인
		pCurrentAddress += (0x100000 / 4);
	}

	return TRUE;
}
