/*
 * main.c
 *
 *  Created on: 2017. 5. 12.
 *      Author: hangstone
 */

#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

//  문자열을 X, Y 위치에 출력
void kPrintString(int nX, int nY, const char* pszString);
//	IA-32e 모드용 커널 영역을 '0'으로 초기화
//		1MB ~ 6MB 까지 영역을 초기화
BOOL kInitializeKernel64Area(void);
//	HANG64 OS를 실행하기에 충분한 메모리를 가지고 있는지 체크
//		64MB 이상의 메모리를 가지고 있는지 검사
BOOL kIsMemoryEnough(void);
//	IA-32e 모드 커널을 0x200000(2MB) 어드레스에 복사
void kCopyKernel64ImageTo2MByte(void);

void Main(void)
{
	BOOL bRet = FALSE;
	DWORD dwEAX, dwEBX, dwECX, dwEDX;
	char szVendorString[13] = {0,};		//	제조사 문자열을 담을 문자열 버퍼
																		//	kPrintString() 함수로 출력하기 위함

  kPrintString(0, 3, "Protected Mode C Language Kernel Start...........[Pass]");

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

  //	IA-32e 모드 커널을 위한 페이지 테이블 생성
  kPrintString(0, 6, "IA-32e Page Table Initialize.....................[    ]");
  kInitializePageTables();
  kPrintString(50, 6, "Pass");

  //	프로세서 제조사 정보 얻기
  kReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
  *(DWORD *)szVendorString = dwEBX;					//	문자가 저장된 순서가 하위바이트에서 상위바이트의
  *((DWORD *)szVendorString + 1) = dwEDX;		//	순서이므로 그대로 문자열 버퍼에 복사하면 정상적으로 출력가능
  *((DWORD *)szVendorString + 2) = dwECX;		//	4-byte씩 한번에 복사하려고 DWORD로 캐스팅 함
  kPrintString(0, 7, "64bit Mode Vendor String.........................[            ]");
  kPrintString(50, 7, szVendorString);

  //	64비트 지원 유무 확인
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

  //	IA-32e 모드 커널을 0x200000(2MB) 어드레스로 이동
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

void kCopyKernel64ImageTo2MByte(void)
{
	WORD 		wKernel32SectorCount;
	WORD 		wTotalKernelSectorCount;
	WORD		wIA32eSectorCount;
	DWORD* 	pSourceAddress;
	DWORD* 	pDestinationAddress;

	//	0x7C05에 총 커널 섹터 수, 0x7C08에 보호모드 커널 섹터 수가 들어 있음
	wTotalKernelSectorCount = *((WORD *) 0x7C05);
	wKernel32SectorCount = *((WORD *) 0x7C07);

	pSourceAddress = (DWORD *)(0x10000 + (wKernel32SectorCount * 512));
	pDestinationAddress = (DWORD *)0x200000;

	//	IA-32e 모드 커널 섹터 크기만큼 복사
	wIA32eSectorCount = wTotalKernelSectorCount - wKernel32SectorCount;
	for (int nIdx=0; nIdx < 512 * wIA32eSectorCount / 4; nIdx++)
	{
		*pDestinationAddress = *pSourceAddress;
		pDestinationAddress++;
		pSourceAddress++;
	}
}
