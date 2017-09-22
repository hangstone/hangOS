/*
 * Console.c
 *
 *  Created on: 2017. 9. 21.
 *      Author: hangstone
 */

#include <stdarg.h>
#include "Console.h"
#include "AssemblyUtility.h"
#include "Keyboard.h"

//  콘솔의 정보를 관리하는 자료구조
CONSOLEMANAGER gs_stConsoleManager = {0, };

void  kInitializeConsole(int nX, int nY)
{
  //  자료구조를 모두 '0'으로 초기화
  kMemSet(&gs_stConsoleManager, 0, sizeof(gs_stConsoleManager));

  //  커서 위치를 설정
  kSetCursor(nX, nY);
}

BOOL kSetCursor(int nX, int nY)
{
  int nLinearValue;

  //  커서의 위치를 계산
  nLinearValue = nY * CONSOLE_WIDTH + nX;

  //  CRTC contoller address register(port: 0x3D4)에 0x0E를 전송하여
  //  상위 커서 위치 레지스터를 선택
  kOutPortByte(VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR);
  //  CRTC contoller data register(port: 0x3D5)에 커서의 상위 바이트를 출력
  kOutPortByte(VGA_PORT_DATA, nLinearValue >> 8);

  //  CRTC contoller address register(port: 0x3D4)에 0x0F를 전송하여
  //  하위 커서 위치 레지스터를 선택
  kOutPortByte(VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR);
  //  CRTC contoller data register(port: 0x3D5)에 커서의 상위 바이트를 출력
  kOutPortByte(VGA_PORT_DATA, nLinearValue &  0xFF);

  //  문자를 출력할 위치 업데이트
  gs_stConsoleManager.nCurrentPrintOffset = nLinearValue;
}

BOOL kGetCursor(int* pX, int* pY)
{
  //  저장된 위치를 콘솔 화면의 너비로 나눈 나머지로 X좌표를 구할 수 있으며, 
  //  화면 너비로 나누면 Y 좌표를 구할 수 있음
  *pX = gs_stConsoleManager.nCurrentPrintOffset % CONSOLE_WIDTH;
  *pY = gs_stConsoleManager.nCurrentPrintOffset / CONSOLE_WIDTH;
}

BOOL kPrintf(const char* pFormatString, ...)
{
  va_list ap;
  char  vcBuffer[1024];
  int   nNextPrintOffset;

  //  가변인자 리스트를 사용해서 vsprintf()로 처리
  va_start(ap, pFormatString);
  kVSPrintf(vcBuffer, pFormatString, ap);
  va_end(ap);

  //  포멧 문자열을 화면에 출력
  nNextPrintOffset = kConsolePrintString(vcBuffer);

  //  커서의 위치를 업데이트
  kSetCursor(nNextPrintOffset % CONSOLE_WIDTH, nNextPrintOffset / CONSOLE_WIDTH);
}

BOOL kConsolePrintString(const char* pBuffer)
{
  CHARACTER* pScreen = (CHARACTER *)CONSOLE_VIDEOMEMORYADDRESS;
  int nLength = 0;
  int nPrintOffset = 0;

  //  문자열을 출력할 위치를 저장
  nPrintOffset = gs_stConsoleManager.nCurrentPrintOffset;

  //  문자열의 길이만큼 화면에 출력
  nLength = kStrLen(pBuffer);
  for (int nIdx=0; nIdx<nLength; nIdx++)
  {
    if ('\n' == pBuffer[nIdx])  //  줄바꿈 처리
    {
      //  출력할 위치를 80의 배수 컬럼으로 옮김
      //  현재 라인의 남은 문자열 수만큼 더래서 다음 라인으로 위치시킴
      nPrintOffset += (CONSOLE_WIDTH - (nPrintOffset % CONSOLE_WIDTH));
    }
    else if ('\t' == pBuffer[nIdx])
    {
      //  출력할 위치를 8의 배수 컬럼으로 옮김
      nPrintOffset += (8 - (nPrintOffset % 8));
    }
    else
    {
      //  비디오 메모리에 문자와 속성을 설정하여 문자를 출력하고, 출력할 위치를 다음으로 이동
      pScreen[nPrintOffset].bCharacter = pBuffer[nIdx];
      pScreen[nPrintOffset].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
      nPrintOffset++;
    }

    //  출력할 위치가 화면의 최대값(80 * 25)을 벗어난 경우에는 스크롤 처리
    if ((CONSOLE_HEIGHT * CONSOLE_WIDTH) <= nPrintOffset)
    {
      //  가장 윗줄을 제외한 나머지를 한 줄 위로 복사
      kMemCpy(CONSOLE_VIDEOMEMORYADDRESS,
              CONSOLE_VIDEOMEMORYADDRESS + CONSOLE_WIDTH * sizeof(CHARACTER),
              (CONSOLE_HEIGHT -1) * CONSOLE_WIDTH * sizeof(CHARACTER));

      //  가장 마지막 라인은 공백으로 채움
      for (int nLastLine = (CONSOLE_HEIGHT - 1) * (CONSOLE_WIDTH);
               nLastLine < (CONSOLE_HEIGHT * CONSOLE_WIDTH);
               nLastLine++)
      {
        //  공백 출력
        pScreen[nLastLine].bCharacter = ' ';
        pScreen[nLastLine].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
      }

      //  출력할 위치를 가장 아래쪽 라인의 처음으로 설정
      nPrintOffset = (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH;
    }
  }

  return nPrintOffset;
}

BOOL kClearScreen(void)
{
  CHARACTER* pScreen = (CHARACTER *)CONSOLE_VIDEOMEMORYADDRESS;
  
  //  화면 전체를 공백으로 채우고 커서의 위치를 (0, 0)으로 옮김
  for (int nIdx=0; nIdx<CONSOLE_WIDTH*CONSOLE_HEIGHT; nIdx++)
  {
    pScreen[nIdx].bCharacter = ' ';
    pScreen[nIdx].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
  }

  //  커서를 화면 상단으로 이동
  kSetCursor(0, 0);
}

BOOL kGetCh(void)
{
  KEYDATA stData;

  //  키가 눌러질때까지 대기
  while (1)
  {
    //  키 큐에 데이터가 수신될 때까지 대기
    while (FALSE == kGetKeyFromKeyQueue(&stData))
    {
      ;
    }

    //  키가 눌렸다는 데이터가 수신되면 ASCII 코드를 반환
    if (stData.bFlags & KEY_FLAGS_DOWN)
      return stData.bASCIICode;
  }
}

BOOL kPrintStringXY(int nX, int nY, const char* pString)
{
  CHARACTER* pScreen = (CHARACTER *)CONSOLE_VIDEOMEMORYADDRESS;

  //  비디오 메모리 어드레스에서 현재 출력할 위치를 계산
  for (int nIdx=0; pString[nIdx] != 0; nIdx++)
  {
    pScreen[nIdx].bCharacter = pString[nIdx];
    pScreen[nIdx].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
  }
}
