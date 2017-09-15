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

//  문자열 출력 함수
void kPrintString(int nX, int nY, const char* pszString);

void Main(void)
{
  BOOL  bRet = FALSE;
  char  pszTypedString[2] = {0,};
  BYTE  bFlags;
  BYTE  bScanedCode;
  int   nPositionX = 0;

  kPrintString(0, 10, "Switch To IA-32e Mode Success!!");
  kPrintString(0, 11, "IA-32e C Language Kernel Start...................[Pass]");

  kPrintString(0, 12, "GDT Initialize And Switch For IA-32e Mode........[    ]");
  kInitializeGDTTableAndTSS();
  kLoadGDTR(GDTR_STARTADDRESS);
  kPrintString(50, 12, "Pass");

  kPrintString(0, 13, "TSS Segment Load.................................[    ]");
  kLoadTR(GDT_TSSSEGMENT);
  kPrintString(50, 13, "Pass");

  kPrintString(0, 14, "IDT Initialize...................................[    ]");
  kInitializeIDTTables();
  kLoadIDTR(IDTR_STARTADDRESS);
  kPrintString(50, 14, "Pass");

  kPrintString(0, 15, "Keyboard Activate................................[    ]");

  //  activate keyboard
  if (TRUE == kActivateKeyboard())
  {
    kPrintString(50, 15, "Pass");
    kChangeKeyboardLED(FALSE, FALSE, FALSE);
  }
  else
  {
    kPrintString(50, 15, "Fail");
    while (1);
  }

  kPrintString(0, 16, "PIC Controller And Interrupt Initialize..........[    ]");
  //  PIC 컨트롤러 초기화 및 모든 인터럽트 활성화
  kInitializePIC();
  kMaskPICInterrupt(0);
  kEnableInterrupt();
  kPrintString(50, 16, "Pass");

  while (1)
  {
    //  출력 버퍼(포트 0x60)가 차 있으면 scan code를 읽을 수 있음
    if (TRUE == kIsOutputBufferFull())
    {
      //  출력 버퍼(포트 0x60)에서 scan code를 읽어서 저장
      bScanedCode = kGetKeyboardScanCode();

      //  scan code를 ASCII code로 변환하는 함수를 호출하여 ASCII 코드와 눌림 또는 떨어짐 정보를 반환
      bRet = kConvertScanCodeToASCIICode(bScanedCode,
                                         &(pszTypedString[0]),
                                         &bFlags);
      if (TRUE == bRet)
      {
        //  키가 눌러졌으면 키의 ASCII code 값을 화면에 출력
        if (bFlags & KEY_FLAGS_DOWN)
        {
          kPrintString(nPositionX++, 17, pszTypedString);
          
          //  '0'이 입력되면 변수를 '0'으로 나누어 Devide Error Exception(벡터 0번)를 발생시킴
          if('0' == pszTypedString[0])
          {
            //  아래 코드를 수행하면, Devide Error 예외가 발생하여 커널의 임시 핸들러가 수행 됨
            BYTE bTemp = 0;
            bTemp = bTemp / 0;
          }
        }        
      }
    }
  }
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
