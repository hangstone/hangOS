[ORG 0x00]      ;;  �ڵ��� ����  address�� 0x00���� ����
[BITS 16]       ;;  ������ �ڵ�� 16-bit �ڵ�� ����

SECTION .text   ;;  text ����(segment)�� ����

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  �ڵ� ����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
  mov   ax, 0x1000      ;;  ��ȣ ��� ��Ʈ�� ����Ʈ�� ���� address(0x1000)�� ���׸�Ʈ�� ���������� ������ ��ȯ
  mov   ds, ax          ;;  AX ���������� ���� DS ���׸�Ʈ �������Ϳ� ����
  mov   es, ax          ;;  ES ���׸�Ʈ �������Ϳ� ����

  ;;  A20 ����Ʈ�� Ȱ��ȭ
  ;;  BIOS�� �̿��� ��ȯ�� �������� �� �ý��� ��Ʈ�� ��Ʈ�� ��ȯ �õ�
  ;;  enable gate 'A20' using BIOS service
  mov   ax, 0x2401      ;;  A20 ����Ʈ Ȱ��ȭ ���� ����
  int   0x15            ;;  BIOS ���ͷ�Ʈ ���� ȣ��

  jc    .A20GATEERROR   ;;  A20 ����Ʈ Ȱ��ȭ�� �����ߴ��� Ȯ��
  jmp   .A20GATESUCCESS

.A20GATEERROR:
  ;;  ���� �߻� ��, �ý��� ��Ʈ�� ��Ʈ�� ��ȯ �õ�
  in    al, 0x92        ;;  �ý��� ��Ʈ�� ��Ʈ(0x92)���� 1-byte�� �о� AL �������Ϳ� ����
  or    al, 0x02        ;;  ���� ���� A20 ����Ʈ ��Ʈ(��Ʈ 1)�� 1�� ����
  and   al, 0xFE        ;;  �ý��� ���� ������ ���� 0xFE�� AND �����Ͽ� ��Ʈ 0�� '0'���� ����
  out   0x92, al        ;;  �ý��� ��Ʈ�� ��Ʈ(0x92)�� ����� ���� 1����Ʈ ����

.A20GATESUCCESS:
  cli                   ;;  interrupt�� �߻����� ���ϵ��� ����
                        ;;  �ֳ��ϸ�, interrupt ���� ������ interrupt�� �߻��ϰ� �Ǹ�,
                        ;;  ���μ����� interrupt ó�� �Լ��� ã�� �� �����Ƿ� ������ �߻��� �� �ִ�
                        ;;  (p. 178 ����)
  lgdt [GDTR]           ;;  GDTR �ڷᱸ���� ���μ����� �����Ͽ� GDT ���̺��� �ε�

  ;;  ��ȣ ���� ����
  ;;  p. 179 ����
  mov   eax, 0x4000003B ;;  PG=0; ����¡ ��� ������� ����
                        ;;  CD=1; cache�� ������� ����
                        ;;  NW=0; write through ��å�� ���
                        ;;  AM=0; address ���� �˻� ����� ������� ����
                        ;;  WP=0; page �Ӽ��� ������� write�� �� �� �ֵ��� ��
                        ;;  NE=1; FPU ���� ó���� interrupt�� �����ϱ� ����
                        ;;  ET=1; 386, 486 ���μ������� FPU�� �����ϱ� ����
                        ;;  TS=1; FPU ���� ��� ���� ��, device-not-available ���� �߻�
                        ;;        EM �ʵ�� MP �ʵ�� �����Ͽ� FPU�� ���¸� �����ϰ� �����ϴ� �뵵�� ���
                        ;;  EM=0; processor�� FPU�� ����Ǿ�����  ���θ� ����
                        ;;        FPU ���� ��� ���� ��, ���ܰ� �߻����� �ʰ�, ���������� ����
                        ;;        FPU�� ������, �Ǽ� ���� ����� ����Ʈ���������� ó���� �������� ���
                        ;;  MP=1; wait �Ǵ� fwait ��� ���� ��, ���ܰ� �߻�
                        ;;  PE=1; protection enable, �� ��ȣ���� ����
  mov   cr0, eax        ;;  CR0 ��Ʈ�� �������Ϳ� ������ ������ flag�� �����Ͽ� ��ȣ ���� ��ȯ

  ;;  Ŀ�� �ڵ� ���׸�Ʈ�� 0x00�� �������� �ϴ� ������ ��ü�ϰ� EIP�� ���� 0x00�� �������� �缳��
  ;;  CS ���׸�Ʈ ������: EIP
  jmp   dword 0x18: (PROTECTEDMODE - $$ + 0x10000)

;;  enter protection mode
[BITS 32]               ;;  ������ �ڵ�� 32bit �ڵ�� ����
PROTECTEDMODE:
  mov   ax, 0x20        ;;  ��ȣ��� Ŀ�ο� ������ ���׸�Ʈ ��ũ���͸� AX �������Ϳ� ����
  mov   ds, ax          ;;  DS ���׸�Ʈ �����Ϳ� ����
  mov   es, ax          ;;  ES ���׸�Ʈ �����Ϳ� ����
  mov   fs, ax          ;;  FS ���׸�Ʈ �����Ϳ� ����
  mov   gs, ax          ;;  GS ���׸�Ʈ �����Ϳ� ����

  ;;  ������ 0x00000000 ~ 0x0000FFFF ������ 64KB ũ��� ����
  mov   ss, ax          ;;  SS ���׸�Ʈ �����Ϳ� ����
  mov   esp, 0xFFFE     ;;  ESP ���������� address�� 0xFFFE�� ����
  mov   ebp, 0xFFFE     ;;  EBP ���������� address�� 0xFFFE�� ����

  ;;  ȭ�鿡 ��ȣ���� ��ȯ�Ǿ��ٴ� �޽����� ��´�
  push  (SWITCHSUCCESSMESSAGE - $$ + 0x10000)    ;;  ����� �޽����� address�� ���ÿ� ����
  push  2
  push  0
  call  PRINTMESSAGE
  add   esp, 12

  jmp   dword 0x18: 0x10200     ;;  C��� Ŀ���� �����ϴ� 0x10200 address�� �̵��Ͽ� C��� Ŀ�� ����





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  �Լ� �ڵ� ����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;  �޽����� ����ϴ� �Լ� (stdcall ���)
;;    param: x��ǥ, y��ǥ, ���ڿ�
PRINTMESSAGE:
  push  ebp                 ;;  ���̽� ������ ��������(BP)�� stack�� ����
  mov   ebp, esp            ;;  BP�� ���� ������ ��������(SP)�� ���� ����
                            ;;  �ֳ��ϸ�, BP�� �̿��ؼ� parameter�� �����ϱ� ���� ����
  push  esi                 ;;  �Լ����� �ӽ÷� ����ϴ� �������ͷμ� �Լ��� ������ �κп���
  push  edi                 ;;  stack�� ���Ե� ���� ���� ���� ������ ������ ���̴�
  push  eax
  push  ecx
  push  edx

  ;;  X, Y ��ǥ�� ���� �޸��� address�� �����
  ;;  Y ��ǥ�� �̿��ؼ� ���� ���� address�� ����
  mov   eax, dword[ebp+12]  ;;  �ι��� parameter(Y��ǥ)�� EAX �������Ϳ� ����
  mov   esi, 160            ;;  �� ������ byte ��(2*80 col)�� ESI �������Ϳ� ����
  mul   esi                 ;;  EAX �������Ϳ� ESI �������͸� ���Ͽ� ȭ�� Y�� address ���
  mov   edi, eax            ;;  ���� ȭ�� Y address�� EDI �������Ϳ� ����
  ;;  X ��ǥ�� �̿��ؼ� 2�� ���� ��, ���� address�� ����
  mov   eax, dword[ebp+8]   ;;  ù��° parameter(X��ǥ)�� EAX �������Ϳ� ����
  mov   esi, 2              ;;  �� ���ڸ� ��Ÿ���� byte ��(2)�� ESI �������Ϳ� ����
  mul   esi                 ;;  EAX �������Ϳ� ESI �������͸� ���Ͽ� ȭ�� X address�� ���
  add   edi, eax            ;;  ���� ���� �޸� address�� ���
  ;;  ����� ���ڿ��� address
  mov   esi, dword[ebp+16]   ;;  ����° parameter(����� ���ڿ��� address)

.MESSAGELOOP:                 ;;  �޽����� ����ϴ� loop
  mov   cl, byte[esi]         ;;  ESI �������Ͱ� ����Ű�� ���ڿ� ��ġ���� �� ���ڸ� CL register�� ����
                              ;;  CL register�� CX register�� ���� 1-byte�� �ǹ�
                              ;;  ���ڿ��� 1-byte�� ����ϹǷ� CX ���������� ���� 1-byte�� ���
  cmp   cl, 0                 ;;  ����� ���ڿ� '0'�� ��
  je    .MESSAGEEND           ;;  ����� ������ ���� '0'�̸� ���ڿ��� ����Ǿ����� �ǹ��ϹǷ�
                              ;;  .MESSAGEEND�� �̵��Ͽ� ���� ��� ����
  mov   byte [edi+0xB8000], cl;;  '0'�� �ƴϸ�, ���� �޸� address 0xB800+EDI�� ���ڸ� ���
  add   esi, 1                ;;  ESI register�� '1'�� ���Ͽ� ���� ���ڿ��� �̵�
  add   edi, 2                ;;  EDI register�� '2'�� ���Ͽ� ���� �޸��� ���� ���� ��ġ�� �̵�
                              ;;  ���� �޸𸮴� (����, �Ӽ�)�� ������ �����ǹǷ� ���ڸ� ����Ϸ���
                              ;;  '2'�� ���ؾ� ��
  jmp   .MESSAGELOOP          ;;  �޽��� ��� loop�� �̵��Ͽ�, ���� ���ڿ��� ���

.MESSAGEEND:
  ;;  stack ���� �۾� ����
  pop   edx                   ;;  �Լ����� ����� ���� DX~ES �������ͱ����� stack�� ���Ե� ���� �̿��ؼ� ����
  pop   ecx                   ;;  stack�� LIFO �����̹Ƿ� �������� �����ؾ� ��
  pop   eax
  pop   edi
  pop   esi
  pop   ebp                   ;;  ���̽� ������ ��������(BP) ����
  ret                         ;;  �Լ��� ȣ���� ���� �ڵ��� ��ġ�� ����





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  ������ ����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;  �Ʒ��� �����͵��� 8-byte�� ���� �����ϱ� ���� �߰�
align   8, db 0

;;  GDTR�� ���� 8-byte�� �����ϱ� ���� �߰�
dw  0x0000
;;  GDTR �ڷᱸ�� ����
GDTR:
  dw  GDTEND - GDT - 1        ;;  �Ʒ��� ��ġ�ϴ� GDT ���̺��� ��ü ũ��
  dd  (GDT - $$ + 0x10000)    ;;  �Ʒ��� ��ġ�ϴ� GDT ���̺��� ���� address

;;  GDT ���̺� ����
GDT:
  ;;  null descriptor, �ݵ�� '0'���� �ʱ�ȭ�ؾ� ��
  ;;  processor�� ���� ����� descriptor�� ��� �ʵ尡 '0'���� �ʱ�ȭ�� descriptor�̸�,
  ;;  �Ϲ������δ� �������� �ʴ´�
  NULLDESCRIPTOR:
    dw  0x0000
    dw  0x0000
    db  0x00
    db  0x00
    db  0x00
    db  0x00

  ;;  IA-32e ��� Ŀ�ο� �ڵ� ���׸�Ʈ descriptor
  IA_32eCODEDESCRIPTOR:
    dw  0xFFFF        ;;  limit [15:0]
    dw  0x0000        ;;  base [15:0]
    db  0x00          ;;  base [23:16]
    db  0x9A          ;;  P=1, DPL=0, code segment, execute/read
    db  0xAF          ;;  G=1, D=0, L=1, limit [19:16]
    db  0x00          ;;  base [31:24]

  ;;  IA-32e ��� Ŀ�ο� ������ ���׸�Ʈ descriptor
  IA_32eDATADESCRIPTOR:
    dw  0xFFFF        ;;  limit [15:0]
    dw  0x0000        ;;  base [15:0]
    db  0x00          ;;  base [23:16]
    db  0x92          ;;  P=1, DPL=0, data segment, execute/read
    db  0xAF          ;;  G=1, D=0, L=1, limit [19:16]
    db  0x00          ;;  base [31:24]

  ;;  ��ȣ ��� Ŀ�ο� �ڵ� ���׸�Ʈ descriptor
  CODEDESCRIPTOR:
    dw  0xFFFF        ;;  limit [15:0]
    dw  0x0000        ;;  base [15:0]
    db  0x00          ;;  base [23:16]
    db  0x9A          ;;  P=1, DPL=0, code segment, execute/read
    db  0xCF          ;;  G=1, D=1, L=0, limit [19:16]
    db  0x00          ;;  base [31:24]

  ;;  ��ȣ ��� Ŀ�ο� ������ ���׸�Ʈ descriptor
  DATADESCRIPTOR:
    dw  0xFFFF        ;;  limit [15:0]
    dw  0x0000        ;;  base [15:0]
    db  0x00          ;;  base [23:16]
    db  0x92          ;;  P=1, DPL=0, code segment, read/write
    db  0xCF          ;;  G=1, D=1, L=0, limit [19:16]
    db  0x00          ;;  base [31:24]
GDTEND:

;;  ��ȣ ���� ��ȯ�Ǿ��ٴ� �޽���
SWITCHSUCCESSMESSAGE: db 'Switching to Protect Mode Succeed!', 0

times 512 - ($ - $$)  db  0x00    ;;  512 byte�� ���߱� ���� ���� �κ��� '0'���� ä��



