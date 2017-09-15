[BITS 64]       ;;  이하의 코드는 64-bit 코드로 설정

SECTION .text   ;;  text 섹션(segment)을 정의

;;  외부에서 정의된 함수를 쓸 수 있도록 선언 함(Import)
extern kCommonExceptionHandler, kCommonInterruptHandler, kKeyboardHandler

;;  C언어에서 호출할 수 있도록 이름을 노출 함(Export)
;;  예외(Exception) 처리를 위한 ISR
global kISRDivideError
global kISRDebug
global kISRNMI
global kISRBreakPoint
global kISROverflow
global kISRBoundRangeExceeded
global kISRInvaliedOpcode
global kISRDeviceNotAvailable
global kISRDoubleFault
global kISRCoprocessorSegmentOverrun
global kISRInvalidTSS
global kISRSegmentNotPresent
global kISRStackSegmentFault
global kISRGeneralProtection
global kISRPageFault
global kISR15
global kISRFPUError
global kISRAlignmentCheck
global kISRMachineCheck
global kISRSMDError
global kISRException
;; 인터럽트(Interrupt) 처리를 위한 ISR
global kISRTimer
global kISRKeyboard
global kISRSlavePIC
global kISRSerialPort2
global kISRSerialPort1
global kISRParallelPort2
global kISRFloppyDiskController
global kISRParallelPort1
global kISRRTC
global kISRReserved
global kISRNotUsed1
global kISRNotUsed2
global kISRMouse
global kISRCoProcessor
global kISRHardDisk1
global kISRHardDisk2
global kISREtcInterrupts


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;  context를 저장하고 셀렉터를 교체하는 macro
%macro KSAVECONTEXT 0   ;;  parameter를 전달받지 않는 KSAVECONTEXT 매크로 정의
  ;;  RBP 레지스터부터 GS 세그먼트 셀렉터까지 모두 stack에 삽입
  ;;    'SS' ~ '에러코드(옵션)' 부분은 processor가 처리하는 부분이므로 실제 코드에는 넣을 필요 없음
  push  rbp
  mov   rbp, rsp    ;;  에러코드를 포함하는 예외가 발생했을 때, 현재 스택의 위치를 
                    ;;  RBP 레지스터에 저장하여 스택에 삽입된 에러코드에 'RSP + 8'로 
                    ;;  접근할 수 있도록 함
  push  rax
  push  rbx
  push  rcx
  push  rdx
  push  rdi
  push  rsi
  push  r8
  push  r9
  push  r10
  push  r11
  push  r12
  push  r13
  push  r14
  push  r15
  
  mov   ax, ds      ;;  DS 세그먼트 셀렉터와 ES 세그먼트 셀렉터는 stack에 직접  
  push  rax         ;;  삽입할 수 없으므로, RAX 레지스터에 저장한 후, stack에 삽입
  mov   ax, es
  push  rax
  push  fs
  push  gs

  ;;  세그먼트 셀렉터 교체
  mov   ax, 0x10    ;;  AX 레지스터에 커널 데이터 세그먼트 디스크립터에 저장
  mov   ds, ax      ;;  DS 세그먼트 셀렉터부터 FS 세그먼트 셀렉터까지 모두 
  mov   es, ax      ;;  커널 데이터 세그먼트 교체
  mov   gs, ax
  mov   fs, ax
%endmacro

;;  context를 복원하는 매크로
%macro KLOADCONTEXT 0   ;;  parameter를 전달받지 않는 KSAVECONTEXT 메크로 정의
  ;;  context 복원
  ;;  GS 세그먼트 셀렉터부터 RBP 레지스터까지 모두 stack에서 꺼내 복원
  pop   gs
  pop   fs
  pop   rax
  mov   es, ax      ;;  ES 세그먼트 셀렉터와 DS 세그먼트 셀렉터는 stack에서 직접 꺼내
  pop   rax         ;;  복원할 수 없으므로, RAX 레지스터에 저장한 뒤에 복원
  mov   ds, ax
  
  pop   r15
  pop   r14
  pop   r13
  pop   r12
  pop   r11
  pop   r10
  pop   r9
  pop   r8
  pop   rsi
  pop   rdi
  pop   rdx
  pop   rcx
  pop   rbx
  pop   rax
  pop   rbp
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  예외 핸들러
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

kISRDivideError:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 0
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRDebug:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 1
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRNMI:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 2
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRBreakPoint:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 3
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISROverflow:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 4
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRBoundRangeExceeded:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 5
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRInvaliedOpcode:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 6
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRDeviceNotAvailable:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 7
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRDoubleFault:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 8
  mov   rsi, qword[rbp+8]
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  add   rsp, 8
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRCoprocessorSegmentOverrun:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 9
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRInvalidTSS:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 10
  mov   rsi, qword[rbp+8]
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  add   rsp, 8
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRSegmentNotPresent:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 11
  mov   rsi, qword[rbp+8]
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  add   rsp, 8
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRStackSegmentFault:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 12
  mov   rsi, qword[rbp+8]
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  add   rsp, 8
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRGeneralProtection:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 13
  mov   rsi, qword[rbp+8]
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  add   rsp, 8
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRPageFault:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 14
  mov   rsi, qword[rbp+8]
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  add   rsp, 8
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISR15:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 15
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원
  
kISRFPUError:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 16
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRAlignmentCheck:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 17
  mov   rsi, qword[rbp+8]
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  add   rsp, 8
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원
  
kISRMachineCheck:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 18
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRSMDError:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 19
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRException:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 20
  call  kCommonExceptionHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  예외 핸들러
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
kISRTimer:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 32
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRKeyboard:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 33
  call  kKeyboardHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRSlavePIC:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 34
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRSerialPort2:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 35
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRSerialPort1:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 36
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRParallelPort2:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 37
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRFloppyDiskController:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 38
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRParallelPort1:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 39
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRRTC:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 40
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRReserved:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 41
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRNotUsed1:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 42
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRNotUsed2:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 43
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRMouse:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 44
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRCoProcessor:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 45
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRHardDisk1:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 46
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISRHardDisk2:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 47
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원

kISREtcInterrupts:
  KSAVECONTEXT    ;;  context를 저장한 뒤 셀렉터를 커널 데이터 스크립트로 교체

  ;; 핸들러에 예외 번호를 삽입하고 핸들러 호출
  mov   rdi, 48
  call  kCommonInterruptHandler

  KLOADCONTEXT    ;;  context를 복원
  iretq           ;;  인터럽트 처리를 완료하고, 이전에 수행하던 코드로 복원
