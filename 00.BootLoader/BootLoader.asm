[ORG 0x00]		;;	�ڵ��� ����  address�� 0x00���� ����
[BITS 16]			;;	������ �ڵ�� 16-bit �ڵ�� ����

SECTION .text	;;	text ����(segment)�� ����

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
