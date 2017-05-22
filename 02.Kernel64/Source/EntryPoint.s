[BITS 64]       ;;  이하의 코드는 64-bit 코드로 설정

SECTION .text   ;;  text 섹션(segment)을 정의

;;  외부에서 정의된 함수를 쓸 수 있도록 선언함(Import)
extern Main

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
  mov   ax, 0x10        ;;  IA-32e 모드 커널용 데이터 세그먼트 디스크립터를  AX 레지스터에 저장
  mov   ds, ax          ;;  AX 레지스터의 값을 DS 세그먼트 레지스터에 설정
  mov   es, ax          ;;  ES 세그먼트 레지스터에 설정
  mov   fs, ax          ;;  ES 세그먼트 레지스터에 설정
  mov   gs, ax          ;;  ES 세그먼트 레지스터에 설정

  //  stack을 0x600000 ~ 0x6FFFFF 영역에 1MB 크기로 생성
  mov   ss, ax          ;;  SS 세그먼트 셀렉터에 설정
  mov   rsp, 0x6FFFF8   ;;  RSP 레지스터의 address를 0x6FFFF8로 설정
  mov   rbp, 0x6FFFF8   ;;  RBP 레지스터의 address를 0x6FFFF8로 설정

  call  Main            ;;  C언어 엔트리 포인트 함수(Main) 호출

  jmp   $               ;;  Main()에서 loop를 돌지 않도록 하기 위함
