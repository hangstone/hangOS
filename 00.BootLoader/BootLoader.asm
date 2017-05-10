[ORG 0x00]		;;	코드의 시작  address를 0x00으로 설정
[BITS 16]			;;	이하의 코드는 16-bit 코드로 설정

SECTION .text	;;	text 섹션(segment)을 정의

;;	ex 4-4. 화면 최상단에 문자를 출력
mov ax, 0xB800				;;	AX 레지스터에 0xB800 복사
mov ds, ax						;;	DS 세그먼트에 AX 레지스터의 값(0xB800)을 복사

mov byte [0x00], 'M'	;;	DS 세그먼트: offset 0xB800:0x0000에 'M'을 복사
mov byte [0x01], 0x4A	;;	DS 세그먼트: offset 0xB800:0x0001에 0x4A(red 배경, 밝은 green 속성)를 복사

mov byte [0x02], 'o'
mov byte [0x03], 0x42

jmp $					;;	현재 위치에서 infinitt loop 수행

times 510 - ($ - $$) db 0x00	;;	$: 현재 line의 address
															;;	$$: 현재 섹션(.text)의 시작 address
															;;	$ - $$: 현재 섹션을 기준으로 하는 offset
															;;	510 - ($ - $$): 현재부터 address 510까지
															;;	db 0x00: 1-byte를 선언하고 값은 0x00
															;;	time: 반복 수행
															;;	현재위치에서 address 510까지 0x00으로 채움

db 0x55				;;	1-byte를 선언하고 값은 0x55
db 0xAA				;;	1-byte를 선언하고 값음 0xAA
							;;	address 511, 512에 0x55, 0xAA를 써서 boot sector로 표기함
