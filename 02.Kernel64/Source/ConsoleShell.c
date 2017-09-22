/*
 * ConsoleShell.c
 *
 *  Created on: 2017. 9. 25.
 *      Author: hangstone
 */

#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"


//  커맨드 테이블 정의
SHELLCOMMANDENTRY gs_vstCommandTable[] = {
  {"help", "Show Help", kCmdHelpMessage},
  {"cls", "Clear Screen", kCmdClearScreen},
  {"tatalram", "Show Total RAM Size", kCmdShowTotalRAMSize},
  {"strtod", "String To Decimal/Hex Convert", kCmdStringToDecimalHexTest},
  {"shutdown", "Shutdown And Reboot OS", kCmdShutdown},
};

/*
 *  functions (실제 shell code)
 */

void  kStartConsoleShell(void)
{
  char  vcCommandBuffer[CONSOLESHELL_MAXCOMMANDBUFFERCOUNT];
  int   nCommandBufferIndex = 0;
  BYTE  bKey;
  int   nCursorX, nCursorY;

  //  프롬프트 출력
  kPrintf(CONSOLESHELL_PROMPTMESSAGE);

  while (1)
  {
    //  키가 수신될때까지 대기
    bKey = kGetCh();

    if (KEY_BACKSPACE == bKey)    //  backspace키 처리
    {
      if (0 < nCommandBufferIndex)
      {
        //  현재 커서 위치를 얻어서 한 문자 앞으로 이동한 다음 공백을 출력하고, 
        //  커맨드 버퍼에서 마지막 문자 삭제
        kGetCursor(&nCursorX, &nCursorY);
        kPrintStringXY(nCursorX - 1, nCursorY, " ");
        kSetCursor(nCursorX - 1, nCursorY);
        nCommandBufferIndex--;
      }
    }
    else if (KEY_ENTER == bKey)
    {
      kPrintf("\n");

      if (0 < nCommandBufferIndex)
      {
        //  커맨드 버퍼에 있는 명령을 실행
        vcCommandBuffer[nCommandBufferIndex] = '\0';
        kExecuteCommand(vcCommandBuffer);
      }

      //  프롬프트 출력 및 커맨드 버퍼 초기화
      kPrintf("%s", CONSOLESHELL_PROMPTMESSAGE);
      kMemSet(vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT);
      nCommandBufferIndex = 0;
    }
    else if ((KEY_LSHIFT == bKey) ||
             (KEY_RSHIFT == bKey) ||
             (KEY_CAPSLOCK == bKey) ||
             (KEY_NUMLOCK == bKey) ||
             (KEY_SCROLLLOCK == bKey))
    {
      ;   //  do nothing
    }
    else
    {
      //  tab은 공백으로 전환
      if (KEY_TAB == bKey)
      {
        bKey = ' ';
      }

      //  버퍼에 공간이 남아 있을때만 가능
      if (CONSOLESHELL_MAXCOMMANDBUFFERCOUNT > nCommandBufferIndex)
      {
        vcCommandBuffer[nCommandBufferIndex++] = bKey;
        kPrintf("%c", bKey);
      }
    }
  }
}

void  kExecuteCommand(const char* pCommandBuffer)
{
  int nCommandBufferLength = 0;
  int nCommandLength = 0;
  int nCount = 0;
  int nIdx = 0, nSpaceIdx = 0;

  //  공백으로 구분된 커맨드를 추출
  nCommandBufferLength = kStrLen(pCommandBuffer);
  for (int nSpaceIdx=0; nSpaceIdx < nCommandBufferLength; nSpaceIdx++)
  {
    if (' ' == pCommandBuffer[nSpaceIdx])
      break;
  }

  //  커맨드 테이블을 검사해서 같은 이름의 커맨드가 있는지 확인
  nCount = sizeof(gs_vstCommandTable) / sizeof(SHELLCOMMANDENTRY);
  for (int nIdx=0; nIdx<nCount; nIdx++)
  {
    nCommandLength = kStrLen(gs_vstCommandTable[nIdx].pCommand);

    //  커맨드의 길이와 내용이 완전히 일치하는지 검사
    if ((nCommandLength == nSpaceIdx) &&
        (kMemCmp(gs_vstCommandTable[nIdx].pCommand, 
                 pCommandBuffer, 
                 nSpaceIdx) == 0))
    {
      gs_vstCommandTable[nIdx].pFunction(pCommandBuffer + nSpaceIdx + 1);
      break;
    }
  }

  //  리스트에서 찾을 수 없다면 에러 출력
  if (nIdx >= nCount)
    kPrintf("'%s' is not found.\n", pCommandBuffer);
}

void  kInitializeParamter(PARAMETERLIST* pParamList, const char* pParameter)
{
  pParamList->pBuffer = pParameter;
  pParamList->nLength = kStrLen(pParameter);
  pParamList->nCurrentPosition = 0;
}

int   kGetNextParameter(PARAMETERLIST* pParamList, char* pParameter)
{
  int nCurPos;
  int nLength;

  //  더 이상 파라미터가 없으면 나감
  if (pParamList->nLength <= pParamList->nCurrentPosition)
    return 0;

  for (nCurPos = pParamList->nCurrentPosition; nCurPos<pParamList->nLength; nCurPos++)
  {
    if (' ' == pParamList->pBuffer[nCurPos])
      break;
  }

  //  파라미터를 복사하고 길이를 반환
  kMemCpy(pParameter, pParamList->pBuffer + pParamList->nCurrentPosition, nCurPos);
  nLength = nCurPos - (pParamList->nCurrentPosition);
  pParameter[nLength] = '\0';

  //  parameter의 위치 업데이트
  pParamList->nCurrentPosition += nLength + 1;

  return nLength;
}


/*
 *  functions (커맨드를 처리하는 함수)
 */

void  kCmdHelpMessage(const char* pParameterBuffer)
{
  int nCount;
  int nCursorX, nCursorY = 0;
  int nLength, nMaxCommandLength = 0;

  kPrintf("============================================================\n");
  kPrintf("                    HANG64 Shell Help                       \n");
  kPrintf("============================================================\n");

  nCount = sizeof(gs_vstCommandTable) / sizeof(SHELLCOMMANDENTRY);

  //  가장 긴 커맨드 리스트의 길이를 계산
  for (int nIdx=0; nIdx<nCount; nIdx++)
  {
    nLength = kStrLen(gs_vstCommandTable[nIdx].pCommand);
    if (nLength > nMaxCommandLength)
      nMaxCommandLength = nLength;
  }

  //  도움말 출력
  for (int nIdx=0; nIdx<nCount; nIdx++)
  {
    kPrintf("%s", gs_vstCommandTable[nIdx].pCommand);
    kGetCursor(&nCursorX, &nCursorY);
    kSetCursor(nMaxCommandLength, nCursorY);
    kPrintf("  - %s\n", gs_vstCommandTable[nIdx].pHelp);
  }
}
 
void  kCmdClearScreen(const char* pParameterBuffer)
{
  //  맨 윗줄은 디버깅 용으로 사용하므로 화면을 지운 후 두번째 라인으로 커서 이동
  kClearScreen();
  kSetCursor(0, 1);
}

void  kCmdShowTotalRAMSize(const char* pParameterBuffer)
{
  kPrintf("Total RAM Size = %d MB\n", kGetTotalRAMSize());
}

void  kCmdStringToDecimalHexTest(const char* pParameterBuffer)
{
  PARAMETERLIST stList;

  //  파라미터 초기화
  kInitializeParamter(&stList, pParameterBuffer);

  while (1)
  {
    char  vcParameter[100];
    int   nLength = 0;
    long  lValue = 0;
    int   nCount = 0;

    //  다음 파라미터 구함, 파라미터의 길이가 '0'이면 파라미터가 없는 것이므로 종료
    nLength = kGetNextParameter(&stList, vcParameter);
    if (0 == nLength)
      break;

    //  파라미터에 대한 정보를 출력하고 16진수인지 10진수인지 판단하여 변환한 후,
    //  결과를 printf로 출력
    kPrintf("Param %d = '%s', Length = %d", nCount+1, vcParameter, nLength);

    //  0x로 시작하면 16진수, 그 외에는 10진수로 판단
    if (0 == kMemCmp(vcParameter, "0x", 2))
    {
      lValue = kAtoI(vcParameter+2, 16);
      kPrintf("HEX Value = %x\n", lValue);
    }
    else
    {
      lValue = kAtoI(vcParameter, 10);
      kPrintf("Decimal Value = %c\n", lValue);
    }

    nCount++;
  }
}

void  kCmdShutdown(const char* pParameterBuffer)
{
  kPrintf("System Shutdown Start...\n");

  //  키보드 컨트롤러를 통해 PC를 재시작
  kPrintf("Press Any Key To Reboot PC...");
  kGetCh();
  kReboot();
}
