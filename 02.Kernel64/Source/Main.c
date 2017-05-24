/*
 * main.c
 *
 *  Created on: 2017. 5. 22.
 *      Author: hangstone
 */

#include "Types.h"
#include "Keyboard.h"

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
  kPrintString(0, 12, "Keyboard Activate................................[    ]");

  //  activate keyboard
  if (TRUE == kActivateKeyboard())
  {
    kPrintString(50, 12, "Pass");
    kChangeKeyboardLED(FALSE, FALSE, FALSE);
  }
  else
  {
    kPrintString(50, 12, "Fail");
    while (1);
  }

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
          kPrintString(nPositionX++, 13, pszTypedString);
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
