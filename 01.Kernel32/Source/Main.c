/*
 * main.c
 *
 *  Created on: 2017. 5. 12.
 *      Author: hangstone
 */

#include "Types.h"

void kPrintString(int nX, int nY, const char* pszString);

void Main(void)
{
  kPrintString(0, 3, "C Language Kernel Started!!");

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
