/*
 * Interrupt Handler.h
 *
 *  Created on: 2017. 9. 15.
 *      Author: hangstone
 */

#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"

void  kCommonExceptionHandler(int nVectorNumber, QWORD qwErrorCode)
{
  char vcBuffer[3] = {0, };

  //  인터럽트 벡터를 화면 오른쪽 위에 두자리 정수로 출력
  vcBuffer[0] = '0' + nVectorNumber / 10;
  vcBuffer[1] = '0' + nVectorNumber % 10;

  kPrintString(0, 11, "IA-32e C Language Kernel Start...................[Pass]");
  kPrintString(0, 0, "=======================================================");
  kPrintString(0, 1, "                  Exception Occur~!!!                  ");
  kPrintString(0, 2, "                    Vector:");
  kPrintString(27, 2 , vcBuffer);
  kPrintString(0, 3, "=======================================================");

  while(1);
}

void  kCommonInterruptHandler(int nVectorNumber)
{
  char vcBuffer[] = "[INT:  , ]";
  static int g_nCommonInterruptCount = 0;

  //======================================================================
  //  인터럽트가 발생했음을 알리기위해 메시지를 출력하는 부분
  //  인터럽트 벡터를 화면 오른쪽 위에 2자리 정수로 출력
  vcBuffer[5] = '0' + nVectorNumber / 10;
  vcBuffer[6] = '0' + nVectorNumber % 10;
  //  발생한 횟수 출력
  vcBuffer[8] = '0' + g_nCommonInterruptCount;
  g_nCommonInterruptCount = (g_nCommonInterruptCount + 1) % 10;
  kPrintString(70, 0, vcBuffer);
  //======================================================================

  //  EOI 전송
  kSendEOIToPIC(nVectorNumber - PIC_IRQSTARTVECTOR);
}

void  kKeyboardHandler(int nVectorNumber)
{
  char vcBuffer[] = "[INT:  , ]";
  static int g_nCommonInterruptCount = 0;
  BYTE  bTemp;

  //======================================================================
  //  인터럽트가 발생했음을 알리기위해 메시지를 출력하는 부분
  //  인터럽트 벡터를 화면 오른쪽 위에 2자리 정수로 출력
  vcBuffer[5] = '0' + nVectorNumber / 10;
  vcBuffer[6] = '0' + nVectorNumber % 10;
  //  발생한 횟수 출력
  vcBuffer[8] = '0' + g_nCommonInterruptCount;
  g_nCommonInterruptCount = (g_nCommonInterruptCount + 1) % 10;
  kPrintString(0, 0, vcBuffer);
  //======================================================================

  //  키보드 컨트롤러에서 데이터를 읽어서 ASCII로 변환하여 큐에 삽입
  if (TRUE == kIsOutputBufferFull())
  {
    bTemp = kGetKeyboardScanCode();
    kConvertScanCodeAndPutQueue(bTemp);
  }

  //  EOI 전송
  kSendEOIToPIC(nVectorNumber - PIC_IRQSTARTVECTOR);
}
