[ORG 0x00]		;;	�ڵ��� ����  address�� 0x00���� ����
[BITS 16]			;;	������ �ڵ�� 16-bit �ڵ�� ����

SECTION .text	;;	text ����(segment)�� ����

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	chapter 4.3.4

;;	initialize segment register
	jmp 0x07C0:START			;;	CS ���׸�Ʈ�� �������Ϳ� 0x07C0�� �����ϸ鼭 START ���̺�� �̵�

START:
	mov ax, 0x07C0			;;	boot loader�� ���� address(0x07C0)�� ���׸�Ʈ ���������� ������ ��ȯ
	mov ds, ax					;;	DS ���׸�Ʈ �������Ϳ� ����
	mov ax, 0xB800			;;	���� �޸��� ���� ��巹��(0xB800)�� ���׸�Ʈ �������� ������ ��ȯ
	mov es, ax					;; 	es ���׸�Ʈ �������Ϳ� ����

;;	ȭ���� ����� �ڵ�
	mov si, 0										;;	SI ��������(���ڿ� ���� �ε��� ��������)�� �ʱ�ȭ

.SCREENCLEARLOOP:						;;	ȭ���� ����� loop
	mov byte [es:si], 0				;;	���� �޸��� ���ڰ� ��ġ�ϴ� address�� '0'�� �����Ͽ� ���ڸ� ����
	mov byte [es:si+1], 0x0A	;;	���� �޸��� �Ӽ��� ��ġ�ϴ� address�� 0x0A(���� ������ ���� ���)�� ����
	add si, 2									;;	���ڿ� �Ӽ��� ���������Ƿ� ���� ��ġ�� �̵�
	cmp si, 80*25*2						;;	ȭ���� ��ü ũ�Ⱑ 80����*25���� ��
	jl .SCREENCLEARLOOP				;;	SI reigster�� 80*25*2���� �۴ٸ�, ���� ������ ���� ������ �����Ƿ�
														;;	.SCREENCLEARLOOP ���̺�� �̵�

;;	�Ž����� ����ϴ� �ڵ�
	mov si, 0							;;	SI register(���ڿ� ���� �ε��� ��������)�� �ʱ�ȭ
	mov di, 0							;;	DI register(���ڿ� ��� �ε��� ��������)�� �ʱ�ȭ

.MESSAGELOOP:								;;	�޽����� ����ϴ� loop
	mov cl, byte[si+MESSAGE1]	;;	MESSAGE1�� address���� SI �������� ����ŭ ���� ��ġ�� ���ڸ�
														;;	 CL register�� ����
														;;	CL register�� CX register�� ���� 1-byte�� �ǹ�
														;;	���ڿ��� 1-byte�� ����ϹǷ� CX ���������� ���� 1-byte�� ���
	cmp cl, 0									;;	����� ���ڿ� '0'�� ��
  je .MESSAGEEND						;;	����� ������ ���� '0'�̸� ���ڿ��� ����Ǿ����� �ǹ��ϹǷ�
  													;;	.MESSAGEEND�� �̵��Ͽ� ���� ��� ����
  mov byte [es:di], cl			;;	'0'�� �ƴϸ�, ���� �޸� address 0xB800:di�� ���ڸ� ���
  add si, 1									;;	SI register�� '1'�� ���Ͽ� ���� ���ڿ��� �̵�
  add di, 2									;;	DI register�� '2'�� ���Ͽ� ���� �޸��� ���� ���� ��ġ�� �̵�
  													;;	���� �޸𸮴� (����, �Ӽ�)�� ������ �����ǹǷ� ���ڸ� ����Ϸ���
  													;;	'2'�� ���ؾ� ��
  jmp .MESSAGELOOP					;;	�޽��� ��� loop�� �̵��Ͽ�, ���� ���ڿ��� ���
.MESSAGEEND:

;;	����� �޽����� ����
;;	�������� '0'���� �����Ͽ�, .MESSAGELOOP���� ó���� �� �ֵ��� ��
MESSAGE1: db 'HANG64 OS Boot Loader was Started!', 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

jmp $					;;	���� ��ġ���� infinitt loop ����

times 510 - ($ - $$) db 0x00	;;	$: ���� line�� address
															;;	$$: ���� ����(.text)�� ���� address
															;;	$ - $$: ���� ������ �������� �ϴ� offset
															;;	510 - ($ - $$): ������� address 510����
															;;	db 0x00: 1-byte�� �����ϰ� ���� 0x00
															;;	time: �ݺ� ����
															;;	������ġ���� address 510���� 0x00���� ä��

db 0x55				;;	1-byte�� �����ϰ� ���� 0x55
db 0xAA				;;	1-byte�� �����ϰ� ���� 0xAA
							;;	address 511, 512�� 0x55, 0xAA�� �Ἥ boot sector�� ǥ����
