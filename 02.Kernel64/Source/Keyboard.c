/*
 * Keyboard.c
 *
 *  Created on: 2017. 5. 23.
 *      Author: hangstone
 */

#include "Types.h"
#include "AssemblyUtility.h"
#include "Keyboard.h"

//////////////////////////////////////////////////
//  키보드 컨트롤러 및 키보드 제어와 관련된 함수
//////////////////////////////////////////////////

BOOL  kIsOutputBufferFull(void)
{
  //  상태 레지스터(포트 0x64)에서 읽은 값에 출력 버퍼 상태 비트(비트 0)가 '1'로 설정되어 있으면
  //  출력 buffer에 키보드가 전송한 데이터가 존재함
  if (kInPortByte(0x64) & 0x01)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

BOOL  kIsInputBufferFull(void)
{
  //  상태 레지스터(포트 0x64)에서 읽은 값에 출력 버퍼 상태 비트(비트 1)가 '1'로 설정되어 있으면
  //  아직 키보드가 데이터를 가져가지 않은 것으로 판단한다
  if (kInPortByte(0x64) & 0x02)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

BOOL  kActivateKeyboard(void)
{
  BOOL bRet = FALSE;

  //  컨트롤 레지스터(포트 0x64)에 키보드 활성화 command(0xAE)를 전달하여 키보드 디바이스 활성화
  kOutPortByte(0x64, 0xAE);

  //  입력 buffer(포트 0x60)가 빌 때까지 기다렸다가 키보드에 활성화 command를 전송
  //  0xFFFF만큼 루프를 수행할 시간이면 충분히 command가 전송될 수 있음
  //  0xFFFF 루프를 수행한 이후에도 입력 buffer(포트  0x60)가 비어있지 않으면 무시하고 전송
  for (int nIdx=0; nIdx < 0xFFFF; nIdx++)
  {
    //  입력 buffer(포트 0x60)가 비어 있으면 키보드 command 전송 가능
    bRet = kIsInputBufferFull();
    if (FALSE == bRet)
      break;
  }

  //  입력 buffer(포트 0x60)로 키보드 활성화(0xF4) command를 전달하여 키보드로 전송
  kOutPortByte(0x60, 0xF4);

  //  ACK가 올 때까지 대기..
  //  ACK가 오기 전에 키보드 출력 buffer(포트 0x60)에 키 데이터가 저장될 수 있으므로
  //  키보드에서 전달된 데이터를 최대 100개까지 수신하여 ACK에 전달
  for (int nData=0; nData < 100; nData++)
  {
    //  0xFFFF만큼 루프를 수행할 시간이면 충분히 command의 응답이 올수 있음
    //  0xFFFF 루프를 수행한 이후에도 출력 buffer(포트 0x60)가 차 있지 않으면 무시하고 읽음
    for (int nIdx=0; nIdx < 0xFFFF; nIdx++)
    {
      //  출력 버퍼(포트 0x60)가 가득 차 있으면 데이터를 읽을 수 있음
      if (TRUE == kIsOutputBufferFull())
        break;
    }

    //  출력 버퍼(포트 0x60)에서 읽은 데이터가 ACK(0xFA)이면 성공
    if (0xFA == kInPortByte(0x60))
      return TRUE;
  }

  return FALSE;
}

BYTE  kGetKeyboardScanCode(void)
{
  //  output buffer(포트 0x60)에 데이터가 있을 때까지 대기
  while (FALSE == kIsOutputBufferFull())
  {
    //  do nothing
  }

  return kInPortByte(0x60);
}

void  kEnableA20Gate(void)
{
  BYTE bOutputPortData;

  //  컨트롤 레지스터(포트 0x64)에 키보드 컨트롤러의 출력 포트 값을 읽는 command(0xD0) 전송
  kOutPortByte(0x64, 0xD0);

  //  출력 포트의 데이터를 기다렸다가 읽음
  for (int nIdx=0; nIdx < 0xFFFF; nIdx++)
  {
    //  출력 buffer(포트 0x60)가 차 있으면 데이터를 읽을 수 있음
    if (TRUE == kIsOutputBufferFull())
      break;
  }

  //  출력 포트(포트 0x60)에 수신된 키보트 컨트롤러의 출력 포트 값을 읽음
  bOutputPortData = kInPortByte(0x60);

  //  A20 게이트 비트 설정
  //  A20 게이트를 활성화하기 위해 1st bit를 '1'로 설정
  bOutputPortData |= 0x01;

  //  입력 버퍼(포트 0x60)에 데이터가 비어있으면 출력 포트에 값을 쓰는 command와 출력 포트 데이터를 전송
  for (int nIdx=0; nIdx < 0xFFFF; nIdx++)
  {
    //  입력 버퍼(포트 0x60)가 비었으면 command 전송 가능
    if (TRUE == kIsInputBufferFull())
      break;
  }

  //  command 레지스터(0x64)에 출력 포트 설정 command(0xD1)을 전달
  kOutPortByte(0x64, 0xD1);

  //  입력 버퍼(0x60)에 A20 게이트 비트가 '1'로 설정된 값을 전달
  kOutPortByte(0x60, bOutputPortData);
}

BOOL  kChangeKeyboardLED(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn)
{
  //  키보드에 LED 변경 command 전송하고, command가 처리될 때까지 대기
  for (int nIdx=0; nIdx < 0xFFFF; nIdx++)
  {
    //  입력 버퍼(포트 0x60)가 비었으면 command 전송 가능
    if (FALSE == kIsInputBufferFull())
      break;
  }

  for (int j=0; j < 100; j++)
  {
    for (int i=0; i < 0xFFFF; i++)
    {
      //  출력 버퍼(포트 0x60)가 차 있으면 데이터를 읽을 수 있음
      if (TRUE == kIsOutputBufferFull())
        break;
    }

    //  출력 buffer(포트 0x60)에서 읽은 데이터가 ACK(0xFA)이면 성공
    if (0xFA == kInPortByte(0x60))
      break;
  }

//  if (100 <= j)
//    return FALSE;

  //  LED 변경 값을 키보드로 전송하고, 데이터가 처리가 될 떄까지 대기
  BYTE bStatusOfKeyboardLED = (bCapsLockOn << 2) |
                              (bNumLockOn << 1) |
                              (bScrollLockOn);
  kOutPortByte(0x60, bStatusOfKeyboardLED);
  for (int nIdx=0; nIdx < 0xFFFF; nIdx++)
  {
    //  입력 버퍼(포트 0x60)가 비어 있으면 키보드가 LED 데이터를 가져간 것임
    if (FALSE == kIsInputBufferFull())
        break;
  }

  //  키보드가 LED 값을 가져갔으므로 ACK가 올 때까지 대기
  for (int j=0; j < 100; j++)
  {
    for (int i=0; i < 0xFFFF; i++)
    {
      //  출력 버퍼(포트 0x60)가 차 있으면 데이터를 읽을 수 있음
      if (TRUE == kIsOutputBufferFull())
        break;
    }

    //  출력 버퍼(포트 0x60)에서 읽은 데이터가 ACK(0xFA)이면 성공
    if (0xFA == kInPortByte(0x60))
      break;
  }

//  if (100 <= j)
//    return FALSE;

  return TRUE;
}

BOOL  kReboot(void)
{
  //  입력 버퍼 (포트 0x60)에 데이터가 비어 있으면 출력 포트에 값을 쓰는 command와 출력 포트 데이터 전송
  for (int nIdx=0; nIdx < 0xFFFF; nIdx++)
  {
    //  입력 버퍼(포트 0x60)가 비었으면 command 전송 가능
    if (FALSE == kIsInputBufferFull())
    {
      break;
    }
  }

  //  command 레지스터 (0x64)에 출력 포트 설정 command(0xD1)를 저장
  kOutPortByte(0x64, 0xD1);

  //  입력 버퍼(0x60)에 '0'을 전달하여 프로세서를 리셋
  kOutPortByte(0x60, 0x00);

  while (1)
  {
    //  do nothing
  }
}

//////////////////////////////////////////////////
//  스캔 코드를 ASCII 코드로 변환하는 기능에 관련된 함수들
//////////////////////////////////////////////////

// 키보드 상태를 관리하는 키보드 매니저
static KEYBOARDMANAGER gs_stKeyboardManager = { 0, };

// 스캔 코드를 ASCII 코드로 변환하는 테이블
static KEYMAPPINGENTRY gs_vstKeyMappingTable[ KEY_MAPPINGTABLEMAXCOUNT ] =
{
    /*  0   */  {   KEY_NONE        ,   KEY_NONE        },
    /*  1   */  {   KEY_ESC         ,   KEY_ESC         },
    /*  2   */  {   '1'             ,   '!'             },
    /*  3   */  {   '2'             ,   '@'             },
    /*  4   */  {   '3'             ,   '#'             },
    /*  5   */  {   '4'             ,   '$'             },
    /*  6   */  {   '5'             ,   '%'             },
    /*  7   */  {   '6'             ,   '^'             },
    /*  8   */  {   '7'             ,   '&'             },
    /*  9   */  {   '8'             ,   '*'             },
    /*  10  */  {   '9'             ,   '('             },
    /*  11  */  {   '0'             ,   ')'             },
    /*  12  */  {   '-'             ,   '_'             },
    /*  13  */  {   '='             ,   '+'             },
    /*  14  */  {   KEY_BACKSPACE   ,   KEY_BACKSPACE   },
    /*  15  */  {   KEY_TAB         ,   KEY_TAB         },
    /*  16  */  {   'q'             ,   'Q'             },
    /*  17  */  {   'w'             ,   'W'             },
    /*  18  */  {   'e'             ,   'E'             },
    /*  19  */  {   'r'             ,   'R'             },
    /*  20  */  {   't'             ,   'T'             },
    /*  21  */  {   'y'             ,   'Y'             },
    /*  22  */  {   'u'             ,   'U'             },
    /*  23  */  {   'i'             ,   'I'             },
    /*  24  */  {   'o'             ,   'O'             },
    /*  25  */  {   'p'             ,   'P'             },
    /*  26  */  {   '['             ,   '{'             },
    /*  27  */  {   ']'             ,   '}'             },
    /*  28  */  {   '\n'            ,   '\n'            },
    /*  29  */  {   KEY_CTRL        ,   KEY_CTRL        },
    /*  30  */  {   'a'             ,   'A'             },
    /*  31  */  {   's'             ,   'S'             },
    /*  32  */  {   'd'             ,   'D'             },
    /*  33  */  {   'f'             ,   'F'             },
    /*  34  */  {   'g'             ,   'G'             },
    /*  35  */  {   'h'             ,   'H'             },
    /*  36  */  {   'j'             ,   'J'             },
    /*  37  */  {   'k'             ,   'K'             },
    /*  38  */  {   'l'             ,   'L'             },
    /*  39  */  {   ';'             ,   ':'             },
    /*  40  */  {   '\''            ,   '\"'            },
    /*  41  */  {   '`'             ,   '~'             },
    /*  42  */  {   KEY_LSHIFT      ,   KEY_LSHIFT      },
    /*  43  */  {   '\\'            ,   '|'             },
    /*  44  */  {   'z'             ,   'Z'             },
    /*  45  */  {   'x'             ,   'X'             },
    /*  46  */  {   'c'             ,   'C'             },
    /*  47  */  {   'v'             ,   'V'             },
    /*  48  */  {   'b'             ,   'B'             },
    /*  49  */  {   'n'             ,   'N'             },
    /*  50  */  {   'm'             ,   'M'             },
    /*  51  */  {   ','             ,   '<'             },
    /*  52  */  {   '.'             ,   '>'             },
    /*  53  */  {   '/'             ,   '?'             },
    /*  54  */  {   KEY_RSHIFT      ,   KEY_RSHIFT      },
    /*  55  */  {   '*'             ,   '*'             },
    /*  56  */  {   KEY_LALT        ,   KEY_LALT        },
    /*  57  */  {   ' '             ,   ' '             },
    /*  58  */  {   KEY_CAPSLOCK    ,   KEY_CAPSLOCK    },
    /*  59  */  {   KEY_F1          ,   KEY_F1          },
    /*  60  */  {   KEY_F2          ,   KEY_F2          },
    /*  61  */  {   KEY_F3          ,   KEY_F3          },
    /*  62  */  {   KEY_F4          ,   KEY_F4          },
    /*  63  */  {   KEY_F5          ,   KEY_F5          },
    /*  64  */  {   KEY_F6          ,   KEY_F6          },
    /*  65  */  {   KEY_F7          ,   KEY_F7          },
    /*  66  */  {   KEY_F8          ,   KEY_F8          },
    /*  67  */  {   KEY_F9          ,   KEY_F9          },
    /*  68  */  {   KEY_F10         ,   KEY_F10         },
    /*  69  */  {   KEY_NUMLOCK     ,   KEY_NUMLOCK     },
    /*  70  */  {   KEY_SCROLLLOCK  ,   KEY_SCROLLLOCK  },

    /*  71  */  {   KEY_HOME        ,   '7'             },
    /*  72  */  {   KEY_UP          ,   '8'             },
    /*  73  */  {   KEY_PAGEUP      ,   '9'             },
    /*  74  */  {   '-'             ,   '-'             },
    /*  75  */  {   KEY_LEFT        ,   '4'             },
    /*  76  */  {   KEY_CENTER      ,   '5'             },
    /*  77  */  {   KEY_RIGHT       ,   '6'             },
    /*  78  */  {   '+'             ,   '+'             },
    /*  79  */  {   KEY_END         ,   '1'             },
    /*  80  */  {   KEY_DOWN        ,   '2'             },
    /*  81  */  {   KEY_PAGEDOWN    ,   '3'             },
    /*  82  */  {   KEY_INS         ,   '0'             },
    /*  83  */  {   KEY_DEL         ,   '.'             },
    /*  84  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  85  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  86  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  87  */  {   KEY_F11         ,   KEY_F11         },
    /*  88  */  {   KEY_F12         ,   KEY_F12         }
};

BOOL  kIsAlphabetScanCode(BYTE bScanCode)
{
  //  변환 table을 값을 직접 읽어서 alphabet 범위인지 확인
  BOOL bIsUpperThan_a = 'a' <= gs_vstKeyMappingTable[bScanCode].bNormalCode ? TRUE : FALSE;
  BOOL bIsLowerThan_z = 'z' >= gs_vstKeyMappingTable[bScanCode].bNormalCode ? TRUE : FALSE;

  if (TRUE == bIsUpperThan_a && TRUE == bIsLowerThan_z)
    return TRUE;
  else
    return FALSE;
}

BOOL  kIsNumberOrSymbolScanCode(BYTE bScanCode)
{
  //  숫자 패드나 확장 키 범위를 제외한 범위(scan code 2~53)에서 영문자가 아니면 숫자 or 기호 이다
  if ( (2 <= bScanCode) &&
       (bScanCode <= 53) &&
       FALSE == (kIsAlphabetScanCode(bScanCode)) )
    return TRUE;
  else
    return FALSE;
}

BOOL  kIsNumberPadScanCode(BYTE bScanCode)
{
  //  숫자 패드는 scan code의 71~83 범위에 있음
  if ( (71 <= bScanCode) && (bScanCode <= 83) )
    return TRUE;
  else
    return FALSE;
}

BOOL  kIsUseCombinedCode(BYTE bScanCode)
{
  BYTE bDownScanCode = bScanCode & 0x7F;  //  최상위 비트는 up & down을 구분하는 것이므로
                                          //  최상위 비트를 제외한 나머지 값에 &연산을 하면 초기화가 될 것이다
  BOOL bUseCombinedKey = FALSE;

  //  알파벳 키라면 shift 키와 caps lock의 영향을 받음
  if (TRUE == kIsAlphabetScanCode(bScanCode))
  {
    //  만약 shift 키와 caps lock 키 중에 하나만 눌러져 있으면 조합된 키를 되돌려 줌
    if (gs_stKeyboardManager.bShiftDown ^ gs_stKeyboardManager.bCapsLockOn)
      bUseCombinedKey = TRUE;
    else
      bUseCombinedKey = FALSE;
  }
  //  숫자와 기호 키인 경우에는 shift 키의 영향을 받음
  else if (TRUE == kIsNumberOrSymbolScanCode(bDownScanCode))
  {
    //  shift key가 눌러져 있으면 조합된 키를 되돌려 줌
    if (TRUE == gs_stKeyboardManager.bShiftDown)
      bUseCombinedKey = TRUE;
    else
      bUseCombinedKey = FALSE;
  }
  //  숫자 패드 키라면 num lock 키의 영향을 받음
  //  0xE0만 제외하면 확장 키 코드와 숫자 패드의 코드가 겹치므로,
  //  확장 키 코드가 수신되지 않았을 때만 조합된 코드 사용
  else if ( (TRUE == kIsNumberPadScanCode(bDownScanCode)) &&
            (FALSE == gs_stKeyboardManager.bExtensionCodeIn))
  {
    if (TRUE == gs_stKeyboardManager.bNumLockOn)
      bUseCombinedKey = TRUE;
    else
      bUseCombinedKey = FALSE;
  }

  return bUseCombinedKey;
}

BOOL  UpdateCombinationKeyStatusAndLED(BYTE bScanCode)
{
  BOOL  bDown = FALSE;
  BYTE  bDownScanCode;
  BOOL  bLEDStatusChanged = FALSE;

  //  on or off 상태 처리
  //  최상위 비트가 '1'이면 off, '0'이면 on
  if (bScanCode & 0x80)
  {
    bDown = FALSE;
    bDownScanCode = bScanCode & 0x7F;
  }
  else
  {
    bDown = TRUE;
    bDownScanCode = bScanCode;
  }

  //  check combined key
  //  shift 키의 scan code(42 or 54)이면, shift key의 상태 갱신
  if ( (42 == bDownScanCode) || (54 == bDownScanCode) )
  {
    gs_stKeyboardManager.bShiftDown = bDown;
  }
  //  caps lock 키의 scan code(58)이면, caps lock의 상태를 갱신하고 LED 상태 변경
  else if ( (58 == bDownScanCode) && (TRUE == bDown) )
  {
    gs_stKeyboardManager.bCapsLockOn ^= TRUE;
    bLEDStatusChanged = TRUE;
  }
  //  num lock 키의 scan code(69)이면, num lock의 상태를 갱신하고 LED 상태 변경
  else if ( (69 == bDownScanCode) && (TRUE == bDown) )
  {
    gs_stKeyboardManager.bNumLockOn ^= TRUE;
    bLEDStatusChanged = TRUE;
  }
  //  scroll lock 키의 scan code(70)이면, scroll lock의 상태를 갱신하고 LED 상태 변경
  else if ( (70 == bDownScanCode) && (TRUE == bDown) )
  {
    gs_stKeyboardManager.bScrollLockOn ^= TRUE;
    bLEDStatusChanged = TRUE;
  }
  else
  {
    bLEDStatusChanged = FALSE;
  }

  //  LED 상태가 변했으면, 키보드로 command를 전송하여 LED를 변경
  if (TRUE == bLEDStatusChanged)
  {
    kChangeKeyboardLED(gs_stKeyboardManager.bCapsLockOn,
                       gs_stKeyboardManager.bNumLockOn,
                       gs_stKeyboardManager.bScrollLockOn);

  }
}

BOOL  kConvertScanCodeToASCIICode(BYTE bScanCode,
                                  BYTE* pASCIICode,
                                  BOOL* pFlags)
{
  BOOL  bUseCombinedKey = FALSE;

  //  이전에 pause 키가 수신되었다면, pause의 남은 scan code를 무시
  if (0 < gs_stKeyboardManager.nSkipCountForPause)
  {
    gs_stKeyboardManager.nSkipCountForPause--;
    return FALSE;
  }

  //  pause 키는 따로 처리
  if (0xE1 == bScanCode)
  {
    *pASCIICode = KEY_PAUSE;
    *pFlags = KEY_FLAGS_DOWN;
    gs_stKeyboardManager.nSkipCountForPause = KEY_SKIPCOUNTFORPAUSE;

    return TRUE;
  }
  //  확장 키 코드가 들어왔을 때, 실제 키 값은 다음에 들어오므로 flag 설정만 하고 종료
  else if (0xE0 == bScanCode)
  {
    gs_stKeyboardManager.bExtensionCodeIn = TRUE;
    return FALSE;
  }

  //  조합된 키 값을 반환해야 하는가?
  bUseCombinedKey = kIsUseCombinedCode(bScanCode);

  //  키 값을 설정
  if (TRUE == bUseCombinedKey)
  {
    *pASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bCombinedCode;
  }
  else
  {
    *pASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bNormalCode;
  }

  //  확장 키 유무 설정
  if (TRUE == gs_stKeyboardManager.bExtensionCodeIn)
  {
    *pFlags = KEY_FLAGS_EXTENDEDKEY;
    gs_stKeyboardManager.bExtensionCodeIn = FALSE;
  }
  else
  {
    *pFlags = 0;
  }

  //  눌러짐 또는 떨어짐 유무 설정
  if (0 == (bScanCode & 0x80))
  {
    *pFlags |= KEY_FLAGS_DOWN;
  }

  //  조합 키 눌림 또는 떨어짐 상태를 갱신
  UpdateCombinationKeyStatusAndLED(bScanCode);

  return TRUE;
}
