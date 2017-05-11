[ORG 0x00]      ;;  �ڵ��� ����  address�� 0x00���� ����
[BITS 16]       ;;  ������ �ڵ�� 16-bit �ڵ�� ����

SECTION .text   ;;  text ����(segment)�� ����

;;  initialize segment register
jmp 0x07C0:START      ;;  CS ���׸�Ʈ�� �������Ϳ� 0x07C0�� �����ϸ鼭 START ���̺�� �̵�

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  �ڵ� ����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
START:
  mov ax, 0x07C0      ;;  boot loader�� ���� address(0x07C0)�� ���׸�Ʈ ���������� ������ ��ȯ
  mov ds, ax          ;;  DS ���׸�Ʈ �������Ϳ� ����
  mov ax, 0xB800      ;;  ���� �޸��� ���� ��巹��(0xB800)�� ���׸�Ʈ �������� ������ ��ȯ
  mov es, ax          ;;   es ���׸�Ʈ �������Ϳ� ����

  ;;  ȭ���� ����� �ڵ�
  mov si, 0           ;;  SI ��������(���ڿ� ���� �ε��� ��������)�� �ʱ�ȭ

.SCREENCLEARLOOP:           ;;  ȭ���� ����� loop
  mov byte [es:si], 0       ;;  ���� �޸��� ���ڰ� ��ġ�ϴ� address�� '0'�� �����Ͽ� ���ڸ� ����
  mov byte [es:si+1], 0x0A  ;;  ���� �޸��� �Ӽ��� ��ġ�ϴ� address�� 0x0A(���� ������ ���� ���)�� ����
  add si, 2                 ;;  ���ڿ� �Ӽ��� ���������Ƿ� ���� ��ġ�� �̵�
  cmp si, 80*25*2           ;;  ȭ���� ��ü ũ�Ⱑ 80����*25���� ��
  jl .SCREENCLEARLOOP       ;;  SI reigster�� 80*25*2���� �۴ٸ�, ���� ������ ���� ������ �����Ƿ�
                            ;;  .SCREENCLEARLOOP ���̺�� �̵�

  ;;  ȭ�� ��ܿ� ���� �޽��� ���
  push  MESSAGE1            ;;  ����� �޽����� address�� ���ÿ� ����
  push  0                   ;;  ȭ�� Y��ǥ(0)�� ���ÿ� ����
  push  0                   ;;  ȭ�� X��ǥ(0)�� ���ÿ� ����
  push  PRINTMESSAGE        ;;  PRINTMESSAGE �Լ� ȣ��
  add   sp, 6               ;;  ������ parameter ����

  ;;  OS �̹����� �ε��Ѵٴ� �޽��� ���
  push  IMAGELOADINGMESSAGE
  push  1
  push  0
  call  PRINTMESSAGE
  add   sp, 6

;;  disk���� image�� �ε�
;;  disk�� �б� ���� ���� �����Ѵ�
RESETDISK:
  ;;  call bios reset function
  ;;  ���� ��ȣ: 0, ����̺� ��ȣ: 0(floppy)
  mov   ax, 0
  mov   dl, 0
  int   0x13
  ;;  ������ �߻��ϸ� ����ó�� �ڵ�� �̵�
  jc    HANDLEDISKERROR

  ;;  ��ũ���� ���͸� ����
  ;;  ��ũ�� ������ �޸𸮷� ������ address(ES:BX)�� 0x10000���� ����
  mov   si, 0x1000                  ;;  OS �̹����� ������ address(0x1000)�� ���׸�Ʈ �������� ������ ��ȯ
  mov   es, si                      ;;  ES ���׸�Ʈ �������Ϳ� �� ����
  mov   bx, 0x0000                  ;;  BX �������Ϳ� 0x0000�� �����Ͽ� ������ address��
                                    ;;  0x1000:0000(0x1000)���� ���� ����
  mov   di, word[TOTALSECTORCOUNT]  ;;  ������ OS �̹����� ���� ���� DI �������Ϳ� ����

;;  ��ũ�� �д� �ڵ�
READDATA:
  ;;  ��� ���͸� �� �о����� Ȯ��
  cmp  di, 0      ;;  ������ OS �̹����� ���� ���� '0'�� ��
  je  READEND     ;;  ������ sector ���� '0'�̶��, �� ������ ���̹Ƿ� READEND�� �̵�
  sub di, 0x01    ;;  ������ sector ���� '1' ����

  ;;  Call a BIOS Read Function
  mov  ah, 0x02               ;;  BIOS ���� #2 (read sector)
  mov al, 0x01                ;;  ���� ���� ���� '1'
  mov ch, byte[TRACKNUMBER]   ;;  ���� Ʈ�� ��ȣ ����
  mov cl, byte[SECTORNUMBER]  ;;  ���� ���� ��ȣ ����
  mov dh, byte[HEADNUMBER]    ;;  ���� ��� ��ȣ ����
  mov dl, 0x00                ;;  ���� ����̺� ��ȣ(floppy = 0) ����
  int 0x13                    ;;  interrupt service ����
  jc HANDLEDISKERROR          ;;   ������ �߻� �ߴٸ�, HANDLEDISKERROR�� �̵�

  ;;  Calculate address to copy, track, head and sector address
  add si, 0x0020      ;;  512-byte(0x200) ��ŭ �о����Ƿ�, �̸� ���׸�Ʈ �������� ������ �̵�
  mov es, si          ;;  ES ���׸�Ʈ �������Ϳ� ���ؼ�, address�� �� ���͸�ŭ ����

  ;;  �� ���͸� �о����Ƿ� ���� ��ȣ�� ������Ű��, ������ ����(18)���� �о����� �Ǵ�
  ;;  ������ ���Ͱ� �ƴϸ�, ���� �б�� �̵��ؼ� �ٽ� ���� �б� ����
  mov al, byte[SECTORNUMBER]    ;;  ���� ��ȣ�� AL �������Ϳ� ����
  add al, 0x01                  ;;  ���� ��ȣ�� '1' ����
  mov byte[SECTORNUMBER], al    ;;  ������Ų ���� ��ȣ�� SECTORNUMBER�� �ٽ� ����
  cmp al, 19                    ;;  ������Ų ���� ��ȣ�� '19'�� ��
  jl READDATA                   ;;  ���� ��ȣ�� '19'�̸��̶��, READDATA�� �̵�

  ;;  ������ ���ͱ��� �о�����(��, ���� ��ȣ�� '19'�̸�), ��带 toggle(0->1, 1->0)�ϰ�,
  ;;  ���� ��ȣ�� '1'�� ����
  xor byte[HEADNUMBER], 0x01    ;;  ��� ��ȣ�� 0x01�� XOR�Ͽ� toggle
  mov byte[SECTORNUMBER], 0x01  ;;  ���� ��ȣ�� �ٽ� '1'�� ����

  ;;  ���� ��尡 1->0 ���� �ٲ������, ���� ��带 ��� ���� ���̹Ƿ� �Ʒ��� �̵��Ͽ� track ��ȣ�� '1' ����
  cmp byte[HEADNUMBER], 0x00    ;;  ��� ��ȣ�� 0x00�� ��
  jne READDATA                  ;;  ��� ��ȣ�� '0'�� �ƴ϶�� READDATA�� �̵�

  ;;  track�� '1' ������Ų ��, �ٽ� ���� �б�� �̵�
  add byte[TRACKNUMBER], 0x01   ;;  Ʈ�� ��ȣ�� '1'����
  jmp READDATA                  ;;  READDATA�� �̵�
READEND:

  ;;  OS �̹����� loading�� �Ϸ�Ǿ��ٴ� �޽����� ǥ��
  push  LOADINGCOMPLETEMESSAGE
  push  1
  push  20
  call  PRINTMESSAGE
  add   sp, 6

  ;;  loading�� ���� OS �̹��� ����
  jmp   0x1000:0x0000





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  �Լ� �ڵ� ����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;  ��ũ ������ ó���ϴ� �Լ�
HANDLEDISKERROR:
  push  DISKERRORMESSAGE
  push  1
  push  20
  call  PRINTMESSAGE

  jmp   $                   ;;  ���� ��ġ���� ���ѷ��� ����

;;  �޽����� ����ϴ� �Լ� (stdcall ���)
;;    param: x��ǥ, y��ǥ, ���ڿ�
PRINTMESSAGE:
  push  bp                  ;;  ���̽� ������ ��������(BP)�� stack�� ����
  mov   bp, sp              ;;  BP�� ���� ������ ��������(SP)�� ���� ����
                            ;;  because BP�� �̿��ؼ� parameter�� �����ϱ� ���� ����
  push  es                  ;;  es~dx �������͵��� stack�� ����
  push  si                  ;;  �Լ����� �ӽ÷� ����ϴ� �������ͷμ� �Լ��� ������ �κп���
  push  di                  ;;  stack�� ���Ե� ���� ���� ���� ������ ������ ���̴�
  push  ax
  push  cx
  push  dx

  ;;  ES ���׸�Ʈ �������Ϳ� ���� ��� ��巹�� ����
  mov   ax, 0xB800          ;;  ���� �޸� ���� address(0xB800)�� ��ũ��Ʈ �������� ������ ��ȯ
  mov   es, ax              ;;  ES ���׸�Ʈ �������Ϳ� ����

  ;;  X, Y ��ǥ�� ���� �޸��� address�� �����
  ;;  Y ��ǥ�� �̿��ؼ� ���� ���� address�� ����
  mov   ax, word[bp+6]      ;;  �ι��� parameter(Y��ǥ)�� AX �������Ϳ� ����
  mov   si, 160             ;;  �� ������ byte ��(2*80 col)�� SI �������Ϳ� ����
  mul   si                  ;;  AX �������Ϳ� SI �������͸� ���Ͽ� ȭ�� Y�� address ���
  mov   di, ax              ;;  ���� ȭ�� Y address�� DI �������Ϳ� ����
  ;;  X ��ǥ�� �̿��ؼ� 2�� ���� ��, ���� address�� ����
  mov   ax, word[bp+4]      ;;  ù��° parameter(X��ǥ)�� AX �������Ϳ� ����
  mov   si, 2               ;;  �� ���ڸ� ��Ÿ���� byte ��(2)�� SI �������Ϳ� ����
  mul   si                  ;;  AX �������Ϳ� SI �������͸� ���Ͽ� ȭ�� X address�� ���
  add   di, ax              ;;  ���� ���� �޸� address�� ���
  ;;  ����� ���ڿ��� address
  mov   si, word[bp+8]      ;;  ����° parameter(����� ���ڿ��� address)

.MESSAGELOOP:                 ;;  �޽����� ����ϴ� loop
  mov   cl, byte[si+MESSAGE1] ;;  MESSAGE1�� address���� SI �������� ����ŭ ���� ��ġ�� ���ڸ�
                              ;;   CL register�� ����
                              ;;  CL register�� CX register�� ���� 1-byte�� �ǹ�
                              ;;  ���ڿ��� 1-byte�� ����ϹǷ� CX ���������� ���� 1-byte�� ���
  cmp   cl, 0                 ;;  ����� ���ڿ� '0'�� ��
  je    .MESSAGEEND           ;;  ����� ������ ���� '0'�̸� ���ڿ��� ����Ǿ����� �ǹ��ϹǷ�
                              ;;  .MESSAGEEND�� �̵��Ͽ� ���� ��� ����
  mov   byte [es:di], cl      ;;  '0'�� �ƴϸ�, ���� �޸� address 0xB800:di�� ���ڸ� ���
  add   si, 1                 ;;  SI register�� '1'�� ���Ͽ� ���� ���ڿ��� �̵�
  add   di, 2                 ;;  DI register�� '2'�� ���Ͽ� ���� �޸��� ���� ���� ��ġ�� �̵�
                              ;;  ���� �޸𸮴� (����, �Ӽ�)�� ������ �����ǹǷ� ���ڸ� ����Ϸ���
                              ;;  '2'�� ���ؾ� ��
  jmp   .MESSAGELOOP          ;;  �޽��� ��� loop�� �̵��Ͽ�, ���� ���ڿ��� ���

.MESSAGEEND:
  ;;  stack ���� �۾� ����
  pop   dx                    ;;  �Լ����� ����� ���� DX~ES �������ͱ����� stack�� ���Ե� ���� �̿��ؼ� ����
  pop   cx                    ;;  stack�� LIFO �����̹Ƿ� �������� �����ؾ� ��
  pop   ax
  pop   di
  pop   si
  pop   es
  pop   bp                    ;;  ���̽� ������ ��������(BP) ����
  ret                         ;;  �Լ��� ȣ���� ���� �ڵ��� ��ġ�� ����





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  ������ ����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

MESSAGE1: db 'HANG64 OS Boot Loader was Started!', 0  ;;  ����� �޽����� ����
                                                      ;;  �������� '0'���� �����Ͽ�,
                                                      ;;  .MESSAGELOOP���� ���ڿ���
                                                      ;;  ����Ǿ����� �� �� �ֵ��� ��
DISKERRORMESSAGE:         db  'DISK Error!', 0
IMAGELOADINGMESSAGE:      db  'Loading OS Image..'
LOADINGCOMPLETEMESSAGE:   db  'Complete!', 0

;;  disk �б�� ������ ����
TOTALSECTORCOUNT:   dw    1024    ;;  bootloader�� ������  OS �̹��� ũ��
                                  ;;  �ִ� 1152 sector(0x90000 byte)���� ����
SECTORNUMBER:       db    0x02    ;;  OS �̹����� �����ϴ� ���� ��ȣ�� �����ϴ� ����
HEADNUMBER:         db    0x00    ;;  OS �̹����� �����ϴ� ��� ��ȣ�� �����ϴ� ����
TRACKNUMBER:        db    0x00    ;;  OS �̹����� �����ϴ� Ʈ�� ��ȣ�� �����ϴ� ����





jmp $                           ;;  ���� ��ġ���� infinite loop ����

times 510 - ($ - $$) db 0x00    ;;  $: ���� line�� address
                                ;;  $$: ���� ����(.text)�� ���� address
                                ;;  $ - $$: ���� ������ �������� �ϴ� offset
                                ;;  510 - ($ - $$): ������� address 510����
                                ;;  db 0x00: 1-byte�� �����ϰ� ���� 0x00
                                ;;  time: �ݺ� ����
                                ;;  ������ġ���� address 510���� 0x00���� ä��

db 0x55       ;;  1-byte�� �����ϰ� ���� 0x55
db 0xAA       ;;  1-byte�� �����ϰ� ���� 0xAA
              ;;  address 511, 512�� 0x55, 0xAA�� �Ἥ boot sector�� ǥ����
