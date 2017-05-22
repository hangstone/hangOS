[BITS 32]       ;;  ������ �ڵ�� 32-bit �ڵ�� ����

;;  C���� ȣ���� �� �ֵ��� �̸��� ������(Export)
global kReadCPUID, kSwitchAndExecute64BitKernel

SECTION .text   ;;  text ����(segment)�� ����

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  �ڵ� ����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;  CPUID�� ��ȯ
;;    PARAM: DWORD dwEAX, DWORD* pdwEAX, DWORD* pdwEBX, DWORD* pdwECX, DWORD* pdwEDX
kReadCPUID:
  push  ebp         ;;  EBP(base pointer register)�� stack�� ����
  mov   ebp, esp    ;;  EBP�� ESP(stack pointer register)�� ���� ����
  push  eax         ;;  �Լ����� �ӽ÷� ����ϴ� register�� �Լ��� ������ �κп��� stack�� ���Ե� ���� ���� ���� ������ ����
  push  ebx
  push  ecx
  push  edx
  push  esi

  ;;  EAX register�� ������ CPUID ��ɾ ����
  mov   eax, dword[ebp+8]   ;;  ù���� paramter(dwEAX)�� EAX register�� ����
  cpuid                     ;;  cpuid ��ɾ ����

  ;;  ��ȯ�� ���� parameter�� ����
  ;;  *pdwEAX
  mov   esi, dword[ebp+12]  ;;  �ι�° parameter(dwEAX)�� ESI register�� ����
  mov   dword[esi], eax     ;;  pdwEAX�� pointer�̹Ƿ� pointer�� ����Ű�� �ּҿ� EAX register�� ����

  ;;  *pdwEBX
  mov   esi, dword[ebp+16]  ;;  ����° parameter(dwEBX)�� ESI register�� ����
  mov   dword[esi], ebx     ;;  pdwEAX�� pointer�̹Ƿ� pointer�� ����Ű�� �ּҿ� EBX register�� ����

  ;;  *pdwECX
  mov   esi, dword[ebp+20]  ;;  �׹�° parameter(dwECX)�� ESI register�� ����
  mov   dword[esi], ecx     ;;  pdwECX�� pointer�̹Ƿ� pointer�� ����Ű�� �ּҿ� ECX register�� ����

  ;;  *pdwEDX
  mov   esi, dword[ebp+24]  ;;  �ټ���° parameter(dwEDX)�� ESI register�� ����
  mov   dword[esi], edx     ;;  pdwEDX�� pointer�̹Ƿ� pointer�� ����Ű�� �ּҿ� EDX register�� ����

  pop   esi       ;;  ���Ե� ���� �̿��ؼ� ����
  pop   edx
  pop   ecx
  pop   ebx
  pop   eax
  pop   ebp       ;;  EBP(base pointer) �������� ����
  ret             ;;  �Լ��� ȣ���� ���� �ڵ��� ��ġ�� ����

;;  IA-32e ���� ��ȯ�ϰ� 64��Ʈ Ŀ���� ����
;;    PARAM: ����
kSwitchAndExecute64BitKernel:
  ;;  CR4 ��Ʈ�� ���������� PAE ��Ʈ�� '1'�� ����
  mov   eax, cr4        ;;  CR4 ��Ʈ�� ���������� ���� EAX �������Ϳ� ����
  or    eax, 0x20       ;;  PAE ��Ʈ(��Ʈ 5)�� '1'�� ����
  mov   cr4, eax        ;;  PAE ��Ʈ�� '1'�� ������ ���� CR4 ��Ʈ�� �������Ϳ� ����

  ;;  CR3 ��Ʈ�� �������Ϳ� PML4 ���̺��� address�� cache Ȱ��ȭ
  mov   eax, 0x100000   ;;  EAX �������Ϳ� PML4 ���̺��� �����ϴ� 0x100000(1MB)�� ����
  mov   cr3, eax        ;;  CR3 ��Ʈ�� �������Ϳ� 0x100000(1MB)�� ����

  ;;  IA32_EFER.LME�� '1'�� �����Ͽ� IA-32e ��带 Ȱ��ȭ
  mov   ecx, 0xC0000080 ;;  IA32_EFER MSR ���������� ��巹���� ����
  rdmsr                 ;;  MSR �������͸� �б�
  or    eax, 0x0100     ;;  EAX �������Ϳ� ����� IA32_EFER MSR�� ���� 32bit���� LME ��Ʈ(��Ʈ 8)�� 1�� ����
  wrmsr                 ;;  MSR �������Ϳ� ����

  ;;  CR0 ��Ʈ�� �������͸� NW ��Ʈ(��Ʈ 29)=0, CD ��Ʈ(��Ʈ 30)=0, PG ��Ʈ(��Ʈ 31)=1�� �����Ͽ�
  ;;  ĳ�� ��ɰ� ����¡ ����� Ȱ��ȭ
  mov   eax, cr0        ;;  EAX �������Ϳ� CR0 ��Ʈ�� �������͸� ����
  or    eax, 0xE0000000 ;;  NW ��Ʈ(��Ʈ 29), CD ��Ʈ(��Ʈ 30), PG ��Ʈ(��Ʈ 31)�� ��� '1'�� ����
  xor   eax, 0x60000000 ;;  NW ��Ʈ(��Ʈ 29), CD ��Ʈ(��Ʈ 30)�� XOR�Ͽ� '0'���� ����
  mov   CR0, eax        ;;  NW ��Ʈ = 0, CD ��Ʈ = 0, PG ��Ʈ = 1�� ������ ���� �ٽ� CR0 ��Ʈ�� �������Ϳ� ����
  jmp   0x08:0x200000   ;;  CS ���׸�Ʈ �����͸� IA-32e ���� �ڵ� ���׸�Ʈ ��ũ���ͷ� ��ü�ϰ�
                        ;;  0x200000(2MB) ��巹���� �̵�

  ;;  do not reach here
  jmp $
