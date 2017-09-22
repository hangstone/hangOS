/*
 * main.c
 *
 *  Created on: 2017. 5. 22.
 *      Author: hangstone
 */

#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
#include "AssemblyUtility.h"
#include "Console.h"
#include "ConsoleShell.h"


void Main(void)
{
/*
  BOOL  bRet = FALSE;
  char  pszTypedString[2] = {0,};
  BYTE  bFlags;
  BYTE  bScanedCode;
  int   nPositionX = 0;
  KEYDATA stData;
*/
  int nCursorX, nCursorY;

  //  콘솔을 먼저 초기화한 후, 다음 작업을 진행
  kInitializeConsole(0, 10);
  kPrintf("Switch To IA-32e Mode Success!!\n");
  kPrintf("IA-32e C Language Kernel Start...................[Pass]\n");
  kPrintf("Initialize Console...............................[Pass]\n");

  //  부팅 상황을 화면에 출력
  kGetCursor(&nCursorX, &nCursorY);
  kPrintf("GDT Initialize And Switch For IA-32e Mode........[    ]");
  kInitializeGDTTableAndTSS();
  kLoadGDTR(GDTR_STARTADDRESS);
  kSetCursor(50, nCursorY++);
  kPrintf("Pass\n");

  kPrintf("TSS Segment Load.................................[    ]");
  kLoadTR(GDT_TSSSEGMENT);
  kSetCursor(50, nCursorY++);
  kPrintf("Pass\n");

  kPrintf("IDT Initialize...................................[    ]");
  kInitializeIDTTables();
  kLoadIDTR(IDTR_STARTADDRESS);
  kSetCursor(50, nCursorY++);
  kPrintf("Pass\n");

  kPrintf("Total RAM Size Check.............................[    ]");
  kCheckTotalRAMSize();
  kSetCursor(50, nCursorY++);
  kPrintf("Pass], Size = %d MB\n", kGetTotalRAMSize());

  kPrintf("Keyboard Activate And Queue Initialize...........[    ]");

  //  activate keyboard
  if (TRUE == kInitializeKeyboard())
  {
    kSetCursor(50, nCursorY++);
    kPrintf("Pass\n");
    kChangeKeyboardLED(FALSE, FALSE, FALSE);
  }
  else
  {
    kSetCursor(50, nCursorY++);
    kPrintf("Fail\n");
    while (1);
  }

  kPrintf("PIC Controller And Interrupt Initialize..........[    ]");
  //  PIC 컨트롤러 초기화 및 모든 인터럽트 활성화
  kInitializePIC();
  kMaskPICInterrupt(0);
  kEnableInterrupt();
  kSetCursor(50, nCursorY++);
  kPrintf("Pass\n");
}

/*
void kPrintString(int nX, int nY, const char* pszString)
{
  CHARACTER* pstScreen = (CHARACTER *)0xB8000;

  pstScreen += (nY * 80) + nX;
  for (int nIdx = 0; pszString[nIdx] != 0; ++nIdx)
  {
    pstScreen[nIdx].bCharacter = pszString[nIdx];
  }
}
*/