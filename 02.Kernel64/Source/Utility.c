/*
 * Utility.h
 *
 *  Created on: 2017. 9. 12.
 *      Author: hangstone
 */

#include "Types.h"
#include "Utility.h"
#include "AssemblyUtility.h"

//  메모리를 특정 값으로 채움
void  kMemSet(void* pvDestination, BYTE bData, int nSize)
{
  for (int nIdx=0; nIdx < nSize; nIdx++)
  {
    ((char *)pvDestination)[nIdx] = bData;
  }
}

//  메모리 복사
int   kMemCpy(void* pvDestination, const void* pvSource, int nSize)
{
  for (int nIdx = 0; nIdx < nSize; nIdx++)
  {
    ((char *)pvDestination)[nIdx] = ((char *)pvSource)[nIdx];
  }

  return nSize;
}

//  메모리 비교
int   kMemCmp(const void* pvDestination, const void* pvSource, int nSize)
{
  for (int nIdx = 0; nIdx < nSize; nIdx++)
  {
    char cTemp;
    cTemp = ((char *)pvDestination)[nIdx] - ((char *)pvSource)[nIdx];
    if (0 != cTemp)
    {
      return (int)cTemp;
    }
  }

  return 0;
}

BOOL  kSetInterruptFlag(BOOL bEnableInterrupt)
{
  QWORD qwRFlags;

  //  이전의 RFLAGS 레지스터 값을 읽은 뒤에 인터럽트 가능/불가 처리
  qwRFlags = kReadRFLAGS();
  if (TRUE == bEnableInterrupt)
    kEnableInterrupt();
  else
    kDisableInterrupt();

  //  이전의 RFLAGS 레지스터의 IF 비트(9번쨰 bit)를 확인하여 이전의 인터럽트 상태를 반환
  if (qwRFlags & 0x0200)
    return TRUE;
  else
    return FALSE;
}

int  kStrLen(const char* pBuffer)
{
  int nLength = 0;

  for (int nIdx=0; ; nIdx++)
  {
    if (NULL_CHAR == pBuffer[nIdx])
      break;
    else
      nLength++;
  }

  return nLength;
}

static QWORD gs_qwTotalRamSize_MB = 0;  //  램의 총 크기 (unit: MB)

int   kCheckTotalRAMSize(void)
{
  DWORD* pCurrentAddress;
  DWORD dwPreviousValue;

  //  64MB(0x4000000)부터 4MB 단위로 검사 시작
  pCurrentAddress = (DWORD *)0x4000000;
  while (1)
  {
    //  이전에 메모리에 있던 값을 저장
    dwPreviousValue = *pCurrentAddress;
    //  0x12345678을 써서 읽었을때 문제가 없는 위치까지를 유효한 메모리 영역으로 인정
    *pCurrentAddress = 0x12345678;
    if (0x12345678 != *pCurrentAddress)
      break;

    //  이전 메모리 값으로 복원
    *pCurrentAddress = dwPreviousValue;
    //  다음 4MB 위치로 이동
    pCurrentAddress += (0x4000000 / 4);
  }

  //  체크가 성공한 address를 1MB로 나누어 계산 (단위가 MB이므로)
  gs_qwTotalRamSize_MB = (QWORD)pCurrentAddress / 0x100000;
}

QWORD kGetTotalRAMSize(void)
{
  return gs_qwTotalRamSize_MB;
}

long  kAtoI(const char* pBuffer, int nRadix)
{
  long lRet = 0;

  switch (nRadix)
  {
  case 16:
    lRet = kHexStringToQword(pBuffer);
    break;
  case 10:
    lRet = kDecimalStringToLong(pBuffer);
    break;
  }

  return lRet;
}

QWORD kHexStringToQword(const char* pBuffer)
{
  QWORD qwValue = 0;
  
  for (int nIdx=0; pBuffer[nIdx] != NULL_CHAR; nIdx++)
  {
    if (('A' <= pBuffer[nIdx]) && (pBuffer[nIdx] <= 'Z'))
      qwValue += (pBuffer[nIdx] - 'A') + 10;
    else if (('a' <= pBuffer[nIdx]) && (pBuffer[nIdx <= 'z']))
      qwValue += (pBuffer[nIdx] - 'a') + 10;
    else
      qwValue += pBuffer[nIdx] - '0';
  }

  return qwValue;
}

long  kDecimalStringToLong(const char* pBuffer)
{
  long lValue = 0;
  int nIdx = 0;
  //  음수면 '-'를 제외하고 나머지를 먼저 long으로 변환
  if ('-' == pBuffer[0])
    nIdx = 1;
  else
    nIdx = 0;

  //  문자열을 돌면서 한글자씩 반환하면서 자릿수를 높여 줌
  for ( ; pBuffer[nIdx] != NULL_CHAR; nIdx++)
  {
    lValue *= 10;
    lValue += pBuffer[nIdx] - '0';
  }

  //  음수면 '-'를 추가
  if ('-' == pBuffer[0])
    lValue = -lValue;

  return lValue;
}

int   kItoA(long lValue, char* pBuffer, int nRadix)
{
  int nRet = 0;

  switch (nRadix)
  {
  case 16:
    nRet = kHexToString(lValue, pBuffer);
    break;
  case 10:
  default:
    nRet = kDecimalToString(lValue, pBuffer);
    break;
  }

  return nRet;
}

int   kHexToString(QWORD qwValue, char* pBuffer)
{
  QWORD qwIdx = 0;
  QWORD qwCurrentValue;

  //  버퍼에 1의 자리부터 16, 256, ...의 자리 순서로 숫자 삽입
  for (qwIdx=0; qwIdx > 0; qwIdx++)
  {
    qwCurrentValue = qwValue % 16;
    if (qwCurrentValue >= 10)
      pBuffer[qwIdx] = 'A' + (qwCurrentValue - 10);
    else
      pBuffer[qwIdx] = '0' + qwCurrentValue;

    qwValue = qwValue / 16;
  }
  pBuffer[qwIdx] = NULL_CHAR;

  //  버퍼에 있는 문자열을 뒤집어서 ... 256, 16, 1의 자리 순서로 변경
  kReverseString(pBuffer);

  return qwIdx;
}

int   kDecimalToString(long lValue, char* pBuffer)
{
  long lIdx = 0;

  //  만약 음수면 출력 버퍼에 '-'를 추가하고 양수로 변환
  if (0 > lValue)
  {
    lIdx = 1;
    pBuffer[0] = '-';
    lValue = -lValue;
  }
  else
    lIdx = 0;

  //  버퍼에 1의 자리부터 10, 100, 1000 ...의 자리 순서로 숫자 삽입
  for ( ; lValue > 0; lIdx++)
  {
    pBuffer[lIdx] = '0' + lValue % 10;
    lValue = lValue / 10;
  }
  pBuffer[lIdx] = NULL_CHAR;

  //  버퍼에 있는 문자열을 뒤집어서 ... 1000, 100, 10, 1의 자리 순서로 변경
  if ('-' == pBuffer[0])
    //  음수인 경우는 부호를 제외하고 문자열을 뒤집음
    kReverseString(&(pBuffer[1]));
  else
    kReverseString(pBuffer);

  return lIdx;
}

void  kReverseString(char* pBuffer)
{
  int nLength = 0;
  char cTemp;

  //  문자열의 가운데를 기준으로 좌/우를 바꾸어 순서를 뒤집음
  nLength = kStrLen(pBuffer);
  for (int nIdx=0; nIdx<nLength/2; nIdx++)
  {
    cTemp = pBuffer[nIdx];
    pBuffer[nIdx] = pBuffer[nLength - 1 - nIdx];
    pBuffer[nLength - 1 - nIdx] = cTemp;
  }
}

int   kSprintf(char* pBuffer, const char* pFormatString, ...)
{
  va_list ap;
  int nRet;

  //  가변 인자를 꺼내서 vsprintf() 함수에 넘겨줌
  va_start(ap, pFormatString);
  nRet = kVSPrintf(pBuffer, pFormatString, ap);
  va_end(ap);

  return nRet;
}

int   kVSPrintf(char* pBuffer, const char* pFormatString, va_list ap)
{
  QWORD i, j;
  int   nBufferIndex = 0;
  int   nFormatLength;
  int   nCopyLength;
  char* pCopyString;
  QWORD qwValue;
  int   nValue;

  //  포멧 문자열의 길이를 읽어서 문자열의 길이 만큼 데이터를 output buffer에 출력
  for (QWORD nIdx=0; nIdx < nFormatLength; nIdx++)
  {
    //  '%'로 시작하면, data type 문자로 처리
    if ('%' == pFormatString[nIdx])
    {
      //  '%' 다음 문자로 이동
      nIdx++;
      switch (pFormatString[nIdx])
      {
      case 's':
        //  가변 인자에 들어있는 파라미터를 문자열 타입으로 변환
        pCopyString = (char *)(va_arg(ap, char *));
        nCopyLength = kStrLen(pCopyString);
        //  문자열의 길이 만큼 출력 버퍼로 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
        kMemCpy(pBuffer + nBufferIndex, pCopyString, nCopyLength);
        nBufferIndex += nCopyLength;
        break;
      case 'c':
        //  가변 인자에 들어있는 파라미터를 문자 타입으로 변환하여 
        //  출력 버퍼에 복사하고 버퍼의 인덱스를 1만큼 이동
        pBuffer[nBufferIndex] = (char)(va_arg(ap, int));
        nBufferIndex++;
        break;
      case 'd':
      case 'i':
        //  가변 인자에 들어있는 파라미터를 정수 타입으로 변환하여 
        //  출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
        nValue = (int)(va_arg(ap, int));
        nBufferIndex += kItoA(nValue, pBuffer + nBufferIndex, 10);
        break;
      case 'x':
      case 'X':
        //  가변 인자에 들어있는 파라미터를 DWORD 타입으로 변환하여 
        //  출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
        qwValue = (DWORD)(va_arg(ap, DWORD)) & 0xFFFFFFFF;
        nBufferIndex += kItoA(qwValue, pBuffer+nBufferIndex, 16);
        break;
      case 'q':
      case 'Q':
      case 'p':
        //  가변 인자에 들어있는 파라미터를 QWORD 타입으로 변환하여 
        //  출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스 이동
        qwValue = (QWORD)(va_arg(ap, QWORD));
        nBufferIndex += kItoA(qwValue, pBuffer + nBufferIndex, 16);
        break;
      default:
        //  위에 해당되지 않는 경우에는 문자를 그대로 출력하고, 버퍼의 인덱스를 '1'만큼 이동
        pBuffer[nBufferIndex] = pFormatString[i];
        nBufferIndex++;
        break;
      }
    }
    else  //  일반 문자열 처리
    {
      //  문자를 그대로 출력하고, 버퍼의 인덱스를 1만큼 이동
      pBuffer[nBufferIndex] = pFormatString[nIdx];
      nBufferIndex++;
    }
  }

  //  NULL을 추가해 완전한 문자열로 만들고, 출력한 문자의 길이를 반환
  pBuffer[nBufferIndex] = NULL_CHAR;
  return nBufferIndex;
}
