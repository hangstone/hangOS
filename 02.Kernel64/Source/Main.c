/*
 * main.c
 *
 *  Created on: 2017. 5. 22.
 *      Author: hangstone
 */

#include "Types.h"

//  문자열 출력 함수
void kPrintString(int nX, int nY, const char* pszString);

void Main(void)
{
	kPrintString(0, 10, "Switch To IA-32e Mode Success!!");
	kPrintString(0, 11, "IA-32e C Language Kernel Start...................[Pass]");
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
