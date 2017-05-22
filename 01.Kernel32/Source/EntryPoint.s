[ORG 0x00]      ;;  코드의 시작  address를 0x00으로 설정
[BITS 16]       ;;  이하의 코드는 16-bit 코드로 설정

SECTION .text   ;;  text 섹션(segment)을 정의

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
  mov   ax, 0x1000      ;;  보호 모드 엔트리 포인트의 시작 address(0x1000)를 세그먼트의 레지스터의 값으로 변환
  mov   ds, ax          ;;  AX 레지스터의 값을 DS 세그먼트 레지스터에 설정
  mov   es, ax          ;;  ES 세그먼트 레지스터에 설정

  ;;  A20 게이트를 활성화
  ;;  BIOS를 이용한 전환이 실패했을 떄 시스템 컨트롤 포트로 전환 시도
  ;;  enable gate 'A20' using BIOS service
  mov   ax, 0x2401      ;;  A20 게이트 활성화 서비스 결정
  int   0x15            ;;  BIOS 인터럽트 서비스 호출

  jc    .A20GATEERROR   ;;  A20 게이트 활성화가 성공했는지 확인
  jmp   .A20GATESUCCESS

.A20GATEERROR:
  ;;  에러 발생 시, 시스템 컨트롤 포트로 전환 시도
  in    al, 0x92        ;;  시스템 컨트롤 포트(0x92)에서 1-byte를 읽어 AL 레지스터에 저장
  or    al, 0x02        ;;  읽은 값에 A20 게이트 비트(비트 1)를 1로 설정
  and   al, 0xFE        ;;  시스템 리셋 방지를 위해 0xFE와 AND 연산하여 비트 0을 '0'으로 설정
  out   0x92, al        ;;  시스템 컨트롤 포트(0x92)에 변경된 값을 1바이트 설정

.A20GATESUCCESS:
  cli                   ;;  interrupt가 발생하지 못하도록 설정
                        ;;  왜냐하면, interrupt 설정 이전에 interrupt가 발생하게 되면,
                        ;;  프로세서는 interrupt 처리 함수를 찾을 수 없으므로 문제가 발생할 수 있다
                        ;;  (p. 178 참조)
  lgdt [GDTR]           ;;  GDTR 자료구조를 프로세서에 설정하여 GDT 테이블을 로드

  ;;  보호 모드로 진입
  ;;  p. 179 참조
  mov   eax, 0x4000003B ;;  PG=0; 페이징 기능 사용하지 않음
                        ;;  CD=1; cache를 사용하지 않음
                        ;;  NW=0; write through 정책을 사용
                        ;;  AM=0; address 정렬 검사 기능을 사용하지 않음
                        ;;  WP=0; page 속성에 관계없이 write를 할 수 있도록 함
                        ;;  NE=1; FPU 에러 처리를 interrupt로 연결하기 위함
                        ;;  ET=1; 386, 486 프로세서에서 FPU를 지원하기 위함
                        ;;  TS=1; FPU 관련 명령 실행 시, device-not-available 예외 발생
                        ;;        EM 필드와 MP 필드와 조합하여 FPU의 상태를 저장하고 복구하는 용도로 사용
                        ;;  EM=0; processor에 FPU가 내장되었는지  여부를 결정
                        ;;        FPU 관련 명령 실행 시, 예외가 발생하지 않고, 정상적으로 실행
                        ;;        FPU가 없으면, 실수 연산 명령을 소프트웨어적으로 처리할 목적으로 사용
                        ;;  MP=1; wait 또는 fwait 명령 실행 시, 예외가 발생
                        ;;  PE=1; protection enable, 즉 보호모드로 진입
  mov   cr0, eax        ;;  CR0 컨트롤 레지스터에 위에서 저장한 flag를 설정하여 보호 모드로 전환

  ;;  커널 코드 세그먼트를 0x00을 기준으로 하는 것으로 교체하고 EIP의 값을 0x00을 기준으로 재설정
  ;;  CS 세그먼트 셀렉터: EIP
  jmp   dword 0x18: (PROTECTEDMODE - $$ + 0x10000)

;;  enter protection mode
[BITS 32]               ;;  이하의 코드는 32bit 코드로 설정
PROTECTEDMODE:
  mov   ax, 0x20        ;;  보호모드 커널용 데이터 세그먼트 디스크립터를 AX 레지스터에 저장
  mov   ds, ax          ;;  DS 세그먼트 셀렉터에 설정
  mov   es, ax          ;;  ES 세그먼트 셀렉터에 설정
  mov   fs, ax          ;;  FS 세그먼트 셀렉터에 설정
  mov   gs, ax          ;;  GS 세그먼트 셀렉터에 설정

  ;;  스택을 0x00000000 ~ 0x0000FFFF 영역에 64KB 크기로 설정
  mov   ss, ax          ;;  SS 세그먼트 셀렉터에 설정
  mov   esp, 0xFFFE     ;;  ESP 레지스터의 address를 0xFFFE로 설정
  mov   ebp, 0xFFFE     ;;  EBP 레지스터의 address를 0xFFFE로 설정

  ;;  화면에 보호모드로 전환되었다는 메시지를 찍는다
  push  (SWITCHSUCCESSMESSAGE - $$ + 0x10000)    ;;  출력할 메시지의 address를 스택에 삽입
  push  2
  push  0
  call  PRINTMESSAGE
  add   esp, 12

  jmp   dword 0x18: 0x10200     ;;  C언어 커널이 존재하는 0x10200 address로 이동하여 C언어 커널 수행





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  함수 코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;  메시지를 출력하는 함수 (stdcall 방식)
;;    param: x좌표, y좌표, 문자열
PRINTMESSAGE:
  push  ebp                 ;;  베이스 포인터 레지스터(BP)를 stack에 삽입
  mov   ebp, esp            ;;  BP에 스택 포인터 레지스터(SP)의 값을 설정
                            ;;  왜냐하면, BP를 이용해서 parameter에 접근하기 위한 목적
  push  esi                 ;;  함수에서 임시로 사용하는 레지스터로서 함수의 마지막 부분에서
  push  edi                 ;;  stack에 삽입된 값을 꺼내 원래 값으로 변경할 것이다
  push  eax
  push  ecx
  push  edx

  ;;  X, Y 좌표로 비디오 메모리의 address를 계산함
  ;;  Y 좌표를 이용해서 먼저 라인 address를 구함
  mov   eax, dword[ebp+12]  ;;  두번쨰 parameter(Y좌표)를 EAX 레지스터에 설정
  mov   esi, 160            ;;  한 라인의 byte 수(2*80 col)를 ESI 레지스터에 설정
  mul   esi                 ;;  EAX 레지스터와 ESI 레지스터를 곱하여 화면 Y의 address 계산
  mov   edi, eax            ;;  계산된 화면 Y address를 EDI 레지스터에 설정
  ;;  X 좌표를 이용해서 2를 곱한 후, 최종 address를 구함
  mov   eax, dword[ebp+8]   ;;  첫번째 parameter(X좌표)를 EAX 레지스터에 설정
  mov   esi, 2              ;;  한 문자를 나타내는 byte 수(2)를 ESI 레지스터에 설정
  mul   esi                 ;;  EAX 레지스터와 ESI 레지스터를 곱하여 화면 X address를 계산
  add   edi, eax            ;;  실제 비디오 메모리 address를 계산
  ;;  출력할 문자열의 address
  mov   esi, dword[ebp+16]   ;;  세번째 parameter(출력할 문자열의 address)

.MESSAGELOOP:                 ;;  메시지를 출력하는 loop
  mov   cl, byte[esi]         ;;  ESI 레지스터가 가리키는 문자열 위치에서 한 문자를 CL register에 복사
                              ;;  CL register는 CX register의 하위 1-byte를 의미
                              ;;  문자열은 1-byte면 충분하므로 CX 레지스터의 하위 1-byte만 사용
  cmp   cl, 0                 ;;  복사된 문자와 '0'을 비교
  je    .MESSAGEEND           ;;  복사된 문자의 값이 '0'이면 문자열이 종료되었음을 의미하므로
                              ;;  .MESSAGEEND로 이동하여 문자 출력 종료
  mov   byte [edi+0xB8000], cl;;  '0'이 아니면, 비디오 메모리 address 0xB800+EDI에 문자를 출력
  add   esi, 1                ;;  ESI register에 '1'을 더하여 다음 문자열로 이동
  add   edi, 2                ;;  EDI register에 '2'를 더하여 비디오 메모리의 다음 문자 위치로 이동
                              ;;  비디오 메모리는 (문자, 속성)의 쌍으로 구성되므로 문자만 출력하려면
                              ;;  '2'를 더해야 함
  jmp   .MESSAGELOOP          ;;  메시지 출력 loop로 이동하여, 다음 문자열을 출력

.MESSAGEEND:
  ;;  stack 정리 작업 수행
  pop   edx                   ;;  함수에서 사용이 끝난 DX~ES 레지스터까지를 stack에 삽입된 값을 이용해서 복원
  pop   ecx                   ;;  stack은 LIFO 구조이므로 역순으로 제거해야 함
  pop   eax
  pop   edi
  pop   esi
  pop   ebp                   ;;  베이스 포인터 레지스터(BP) 복원
  ret                         ;;  함수를 호출한 다음 코드의 위치로 복귀





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  데이터 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;  아래의 데이터들을 8-byte에 맞춰 정렬하기 위해 추가
align   8, db 0

;;  GDTR의 끝을 8-byte로 정렬하기 위해 추가
dw  0x0000
;;  GDTR 자료구조 정의
GDTR:
  dw  GDTEND - GDT - 1        ;;  아래에 위치하는 GDT 테이블의 전체 크기
  dd  (GDT - $$ + 0x10000)    ;;  아래에 위치하는 GDT 테이블의 시작 address

;;  GDT 테이블 정의
GDT:
  ;;  null descriptor, 반드시 '0'으로 초기화해야 함
  ;;  processor에 의해 예약된 descriptor로 모든 필드가 '0'으로 초기화된 descriptor이며,
  ;;  일반적으로는 참조되지 않는다
  NULLDESCRIPTOR:
    dw  0x0000
    dw  0x0000
    db  0x00
    db  0x00
    db  0x00
    db  0x00

  ;;  IA-32e 모드 커널용 코드 세그먼트 descriptor
  IA_32eCODEDESCRIPTOR:
    dw  0xFFFF        ;;  limit [15:0]
    dw  0x0000        ;;  base [15:0]
    db  0x00          ;;  base [23:16]
    db  0x9A          ;;  P=1, DPL=0, code segment, execute/read
    db  0xAF          ;;  G=1, D=0, L=1, limit [19:16]
    db  0x00          ;;  base [31:24]

  ;;  IA-32e 모드 커널용 데이터 세그먼트 descriptor
  IA_32eDATADESCRIPTOR:
    dw  0xFFFF        ;;  limit [15:0]
    dw  0x0000        ;;  base [15:0]
    db  0x00          ;;  base [23:16]
    db  0x92          ;;  P=1, DPL=0, data segment, execute/read
    db  0xAF          ;;  G=1, D=0, L=1, limit [19:16]
    db  0x00          ;;  base [31:24]

  ;;  보호 모드 커널용 코드 세그먼트 descriptor
  CODEDESCRIPTOR:
    dw  0xFFFF        ;;  limit [15:0]
    dw  0x0000        ;;  base [15:0]
    db  0x00          ;;  base [23:16]
    db  0x9A          ;;  P=1, DPL=0, code segment, execute/read
    db  0xCF          ;;  G=1, D=1, L=0, limit [19:16]
    db  0x00          ;;  base [31:24]

  ;;  보호 모드 커널용 데이터 세그먼트 descriptor
  DATADESCRIPTOR:
    dw  0xFFFF        ;;  limit [15:0]
    dw  0x0000        ;;  base [15:0]
    db  0x00          ;;  base [23:16]
    db  0x92          ;;  P=1, DPL=0, code segment, read/write
    db  0xCF          ;;  G=1, D=1, L=0, limit [19:16]
    db  0x00          ;;  base [31:24]
GDTEND:

;;  보호 모드로 전환되었다는 메시지
SWITCHSUCCESSMESSAGE: db 'Switching to Protect Mode Succeed!', 0

times 512 - ($ - $$)  db  0x00    ;;  512 byte를 맞추기 위해 남은 부분을 '0'으로 채움



