[ORG 0x00]		;;	�ڵ��� ����  address�� 0x00���� ����
[BITS 16]			;;	������ �ڵ�� 16-bit �ڵ�� ����

SECTION .text	;;	text ����(segment)�� ����

;;	ex 4-4. ȭ�� �ֻ�ܿ� ���ڸ� ���
mov ax, 0xB800				;;	AX �������Ϳ� 0xB800 ����
mov ds, ax						;;	DS ���׸�Ʈ�� AX ���������� ��(0xB800)�� ����

mov byte [0x00], 'M'	;;	DS ���׸�Ʈ: offset 0xB800:0x0000�� 'M'�� ����
mov byte [0x01], 0x4A	;;	DS ���׸�Ʈ: offset 0xB800:0x0001�� 0x4A(red ���, ���� green �Ӽ�)�� ����

mov byte [0x02], 'o'
mov byte [0x03], 0x42

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
