/*
 * Console.h
 *
 *  Created on: 2017. 9. 25.
 *      Author: hangstone
 */

#ifndef __CONSOLESHELL_H_
#define __CONSOLESHELL_H_

#include "Types.h"

/*
 *  macros
 */
#define CONSOLESHELL_MAXCOMMANDBUFFERCOUNT  300
#define CONSOLESHELL_PROMPTMESSAGE          "HANG64>"

//  문자열 포인터를 파라미터로 받는 함수 포인터 타입 정의
typedef void (* CommandFunction) (const char* pParameter);

/*
 *  structures
 */

//  1-byte로 정렬
#pragma pack (push, 1)

typedef struct kShellCommandEntryStruct
{
  char* pCommand;             //  커맨드 문자열
  char* pHelp;                //  커맨드의 도움말
  CommandFunction pFunction;  //  커맨드를 수행하는 함수의 포인터
} SHELLCOMMANDENTRY;

typedef struct kParameterListStruct
{
  const char* pBuffer;        //  파라미터의 버퍼의 어드레스
  int   nLength;               //  파라미터의 길이
  int   nCurrentPosition;     //  현재 처리할 파라미터가 시작하는 위치
} PARAMETERLIST;

#pragma pack (pop)


/*
 *  functions (실제 shell code)
 */

/*
 *  셸의 메인 루프
 */
void  kStartConsoleShell(void);

/*
 *  커맨드 버퍼에 있는 커맨드를 비교하여 해당 커맨드를 처리하는 함수를 수행
 */
void  kExecuteCommand(const char* pCommandBuffer);

/*
 *  parameter 자료 구조를 초기화
 */
void  kInitializeParamter(PARAMETERLIST* pParamList, const char* pParameter);

/*
 *  공백으로 구분된 파라미터의 내용과 길이를 반환
 */
int   kGetNextParameter(PARAMETERLIST* pParamList, char* pParameter);


/*
 *  functions (커맨드를 처리하는 함수)
 */

/*
 *  셸 도움말을 출력
 */
void  kCmdHelpMessage(const char* pParameterBuffer);

/*
 *  화면을 지움
 */
void  kCmdClearScreen(const char* pParameterBuffer);

/*
 *  총 메모리 크기를 출력
 */
void  kCmdShowTotalRAMSize(const char* pParameterBuffer);

/*
 *  문자열로 된 숫자를 숫자로 변환하여 화면에 출력
 */
void  kCmdStringToDecimalHexTest(const char* pParameterBuffer);

/*
 *  PC를 재시작
 */
void  kCmdShutdown(const char* pParameterBuffer);

#endif  /*__CONSOLESHELL_H_*/
