/*
 * Keyboard.h
 *
 *  Created on: 2017. 5. 23.
 *      Author: hangstone
 */

#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#include "Types.h"

/*
 * Macros
 */

//  pause 키를 처리하기 위해 무시해야 하는 나머지 scan code 수
#define KEY_SKIPCOUNTFORPAUSE   2

//  키 상태에 대한 flag
#define KEY_FLAGS_UP            0x00
#define KEY_FLAGS_DOWN          0x01
#define KEY_FLAGS_EXTENDEDKEY   0x02

//  스캔 코드 매핑 테이블에 대한 macro
#define KEY_MAPPINGTABLEMAXCOUNT  89

#define KEY_NONE        0x00
#define KEY_ENTER       '\n'
#define KEY_TAB         '\t'
#define KEY_ESC         0x1B
#define KEY_BACKSPACE   0x08

#define KEY_CTRL        0x81
#define KEY_LSHIFT      0x82
#define KEY_RSHIFT      0x83
#define KEY_PRINTSCREEN 0x84
#define KEY_LALT        0x85
#define KEY_CAPSLOCK    0x86
#define KEY_F1          0x87
#define KEY_F2          0x88
#define KEY_F3          0x89
#define KEY_F4          0x8A
#define KEY_F5          0x8B
#define KEY_F6          0x8C
#define KEY_F7          0x8D
#define KEY_F8          0x8E
#define KEY_F9          0x8F
#define KEY_F10         0x90
#define KEY_NUMLOCK     0x91
#define KEY_SCROLLLOCK  0x92
#define KEY_HOME        0x93
#define KEY_UP          0x94
#define KEY_PAGEUP      0x95
#define KEY_LEFT        0x96
#define KEY_CENTER      0x97
#define KEY_RIGHT       0x98
#define KEY_END         0x99
#define KEY_DOWN        0x9A
#define KEY_PAGEDOWN    0x9B
#define KEY_INS         0x9C
#define KEY_DEL         0x9D
#define KEY_F11         0x9E
#define KEY_F12         0x9F
#define KEY_PAUSE       0xA0

/*
 * Structs
 */
#pragma pack (push, 1)

typedef struct kKeyMappingEntry
{
  //  shift 키나 caps lock 키와 조합되지 않는 ASCII 코드
  BYTE  bNormalCode;

  //  shift 키나 caps lock 키와 조합된 ASCII 코드
  BYTE  bCombinedCode;
} KEYMAPPINGENTRY;

typedef struct kKeyboardManager
{
  //  조합 키 정보
  BOOL  bShiftDown;
  BOOL  bCapsLockOn;
  BOOL  bNumLockOn;
  BOOL  bScrollLockOn;        //  조합키로는 사용하지는 않지만, 키보드 LED 상태를 저장하기 위한 용도

  //  확장 키를 처리하기 위한 정보
  BOOL  bExtensionCodeIn;     //  확장키를 처리하기 위한 flag
  int   nSkipCountForPause;   //  'Pause' 키를 처리하기 위한 값
                              //  'Pause' 키는 세 개의 코드로 구성되므로 첫번째 키를 제외한
                              //  나머지 키를 무시하기 위해서 추가 함
} KEYBOARDMANAGER;

#pragma pack (pop)


/*
 * Functions
 */

/*
 * 출력 buffer(포트 0x60)에 수신된 데이터가 있는지 여부를 반환
 */
BOOL  kIsOutputBufferFull(void);

/*
 * 입력 buffer(포트 0x64)에 프로세서가 쓴 데이터가 있는지 여부를 반환
 */
BOOL  kIsInputBufferFull(void);

/*
 * 키보드를 활성화
 */
BOOL  kActivateKeyboard(void);

/*
 * 키보드 컨트롤에서 키 값(scan code)를 읽는 코드
 */
BYTE  kGetKeyboardScanCode(void);

/*
 * 키보드 컨트롤러를 통해 A20 게이트를 활성화하는 코드
 */
void  kEnableA20Gate(void);

/*
 * 키보드 LED 상태 제어
 */
BOOL  kChangeKeyboardLED(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn);

/*
 * reset processor
 */
BOOL  kReboot(void);

/*
 * scan code가 알파벳 범위인지 여부를 판단
 */
BOOL  kIsAlphabetScanCode(BYTE bScanCode);

/*
 * 숫자 or 기호 범위인지 여부를 판단
 */
BOOL  kIsNumberOrSymbolScanCode(BYTE bScanCode);

/*
 * 숫자 패드 범위인지 여부를 판단
 */
BOOL  kIsNumberPadScanCode(BYTE bScanCode);

/*
 * 조합된 키 값을 사용해야 하는지 여부를 반환
 */
BOOL  kIsUseCombinedCode(BYTE bScanCode);

/*
 * 조합 키의 상태를 갱신하는 함수
 */
BOOL  UpdateCombinationKeyStatusAndLED(BYTE bScanCode);

/*
 * scan code를 ASCII 코드로 변환
 */
BOOL  kConvertScanCodeToASCIICode(BYTE bScanCode,
                                  BYTE* pASCIICode,
                                  BOOL* pFlags);

#endif /* __KEYBOARD_H_ */
