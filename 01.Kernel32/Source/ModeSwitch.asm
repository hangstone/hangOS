[BITS 32]       ;;  이하의 코드는 32-bit 코드로 설정

;;  C언어에서 호출할 수 있도록 이름을 노출함(Export)
global kReadCPUID, kSwitchAndExecute64BitKernel

SECTION .text   ;;  text 섹션(segment)을 정의

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;  CPUID를 반환
;;    PARAM: DWORD dwEAX, DWORD* pdwEAX, DWORD* pdwEBX, DWORD* pdwECX, DWORD* pdwEDX
kReadCPUID:
  push  ebp         ;;  EBP(base pointer register)를 stack에 삽입
  mov   ebp, esp    ;;  EBP에 ESP(stack pointer register)의 값을 설정
  push  eax         ;;  함수에서 임시로 사용하는 register로 함수의 마지막 부분에서 stack에 삽입된 값을 꺼내 원래 값으로 복원
  push  ebx
  push  ecx
  push  edx
  push  esi

  ;;  EAX register의 값으로 CPUID 명령어를 실행
  mov   eax, dword[ebp+8]   ;;  첫번쨰 paramter(dwEAX)를 EAX register에 저장
  cpuid                     ;;  cpuid 명령어를 수행

  ;;  반환된 값을 parameter에 저장
  ;;  *pdwEAX
  mov   esi, dword[ebp+12]  ;;  두번째 parameter(dwEAX)를 ESI register에 저장
  mov   dword[esi], eax     ;;  pdwEAX가 pointer이므로 pointer가 가리키는 주소에 EAX register를 저장

  ;;  *pdwEBX
  mov   esi, dword[ebp+16]  ;;  세번째 parameter(dwEBX)를 ESI register에 저장
  mov   dword[esi], ebx     ;;  pdwEAX가 pointer이므로 pointer가 가리키는 주소에 EBX register를 저장

  ;;  *pdwECX
  mov   esi, dword[ebp+20]  ;;  네번째 parameter(dwECX)를 ESI register에 저장
  mov   dword[esi], ecx     ;;  pdwECX가 pointer이므로 pointer가 가리키는 주소에 ECX register를 저장

  ;;  *pdwEDX
  mov   esi, dword[ebp+24]  ;;  다섯번째 parameter(dwEDX)를 ESI register에 저장
  mov   dword[esi], edx     ;;  pdwEDX가 pointer이므로 pointer가 가리키는 주소에 EDX register를 저장

  pop   esi       ;;  삽입된 값을 이용해서 복원
  pop   edx
  pop   ecx
  pop   ebx
  pop   eax
  pop   ebp       ;;  EBP(base pointer) 레지스터 복원
  ret             ;;  함수를 호출한 다음 코드의 위치로 복귀

;;  IA-32e 모드로 전환하고 64비트 커널을 수행
;;    PARAM: 없음
kSwitchAndExecute64BitKernel:
  ;;  CR4 컨트롤 레지스터의 PAE 비트를 '1'로 설정
  mov   eax, cr4        ;;  CR4 컨트롤 레지스터의 값을 EAX 레지스터에 저장
  or    eax, 0x20       ;;  PAE 비트(비트 5)를 '1'로 설정
  mov   cr4, eax        ;;  PAE 비트가 '1'로 설정된 값을 CR4 컨트롤 레지스터에 저장

  ;;  CR3 컨트롤 레지스터에 PML4 테이블의 address와 cache 활성화
  mov   eax, 0x100000   ;;  EAX 레지스터에 PML4 테이블이 존재하는 0x100000(1MB)를 저장
  mov   cr3, eax        ;;  CR3 컨트롤 레지스터에 0x100000(1MB)를 저장

  ;;  IA32_EFER.LME를 '1'로 설정하여 IA-32e 모드를 활성화
  mov   ecx, 0xC0000080 ;;  IA32_EFER MSR 레지스터의 어드레스를 저장
  rdmsr                 ;;  MSR 레지스터를 읽기
  or    eax, 0x0100     ;;  EAX 레지스터에 저장된 IA32_EFER MSR의 하위 32bit에서 LME 비트(비트 8)을 1로 설정
  wrmsr                 ;;  MSR 레지스터에 쓰기

  ;;  CR0 컨트롤 레지스터를 NW 비트(비트 29)=0, CD 비트(비트 30)=0, PG 비트(비트 31)=1로 설정하여
  ;;  캐시 기능과 페이징 기능을 활성화
  mov   eax, cr0        ;;  EAX 레지스터에 CR0 컨트롤 레지스터를 저장
  or    eax, 0xE0000000 ;;  NW 비트(비트 29), CD 비트(비트 30), PG 비트(비트 31)을 모두 '1'로 설정
  xor   eax, 0x60000000 ;;  NW 비트(비트 29), CD 비트(비트 30)을 XOR하여 '0'으로 설정
  mov   CR0, eax        ;;  NW 비트 = 0, CD 비트 = 0, PG 비트 = 1로 설정한 값을 다시 CR0 컨트롤 레지스터에 저장
  jmp   0x08:0x200000   ;;  CS 세그먼트 셀렉터를 IA-32e 모드용 코드 세그먼트 디스크립터로 교체하고
                        ;;  0x200000(2MB) 어드레스로 이동

  ;;  do not reach here
  jmp $
