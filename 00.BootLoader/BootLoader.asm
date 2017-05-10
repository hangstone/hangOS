[ORG 0x00]		;;	코드의 시작  address를 0x00으로 설정
[BITS 16]			;;	이하의 코드는 16-bit 코드로 설정

SECTION .text	;;	text 섹션(segment)을 정의

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	chapter 4.3.4

;;	initialize segment register
	jmp 0x07C0:START			;;	CS 세그먼트의 레지스터에 0x07C0을 복사하면서 START 레이블로 이동

START:
	mov ax, 0x07C0			;;	boot loader의 시작 address(0x07C0)를 세그먼트 레지스터의 값으로 변환
	mov ds, ax					;;	DS 세그먼트 레지스터에 설정
	mov ax, 0xB800			;;	비디오 메모리의 시작 어드레스(0xB800)를 세그먼트 레지스터 값으로 변환
	mov es, ax					;; 	es 세그먼트 레지스터에 설정

;;	화면을 지우는 코드
	mov si, 0										;;	SI 레지스터(문자열 원본 인덱스 레지스터)를 초기화

.SCREENCLEARLOOP:						;;	화면을 지우는 loop
	mov byte [es:si], 0				;;	비디오 메모리의 문자가 위치하는 address에 '0'을 복사하여 문자를 삭제
	mov byte [es:si+1], 0x0A	;;	비디오 메모리의 속성이 위치하는 address에 0x0A(검은 바탕에 밝은 녹색)을 복사
	add si, 2									;;	문자와 속성을 선택했으므로 다음 위치로 이동
	cmp si, 80*25*2						;;	화면의 전체 크기가 80문자*25라인 임
	jl .SCREENCLEARLOOP				;;	SI reigster가 80*25*2보다 작다면, 아직 지우지 못한 영역이 있으므로
														;;	.SCREENCLEARLOOP 레이블로 이동

;;	매시지를 출력하는 코드
	mov si, 0							;;	SI register(문자열 원본 인덱스 레지스터)를 초기화
	mov di, 0							;;	DI register(문자열 대상 인덱스 레지스터)를 초기화

.MESSAGELOOP:								;;	메시지를 출력하는 loop
	mov cl, byte[si+MESSAGE1]	;;	MESSAGE1의 address에서 SI 레지스터 값만큼 더한 위치의 문자를
														;;	 CL register에 복사
														;;	CL register는 CX register의 하위 1-byte를 의미
														;;	문자열은 1-byte면 충분하므로 CX 레지스터의 하위 1-byte만 사용
	cmp cl, 0									;;	복사된 문자와 '0'을 비교
  je .MESSAGEEND						;;	복사된 문자의 값이 '0'이면 문자열이 종료되었음을 의미하므로
  													;;	.MESSAGEEND로 이동하여 문자 출력 종료
  mov byte [es:di], cl			;;	'0'이 아니면, 비디오 메모리 address 0xB800:di에 문자를 출력
  add si, 1									;;	SI register에 '1'을 더하여 다음 문자열로 이동
  add di, 2									;;	DI register에 '2'를 더하여 비디오 메모리의 다음 문자 위치로 이동
  													;;	비디오 메모리는 (문자, 속성)의 쌍으로 구성되므로 문자만 출력하려면
  													;;	'2'를 더해야 함
  jmp .MESSAGELOOP					;;	메시지 출력 loop로 이동하여, 다음 문자열을 출력
.MESSAGEEND:

;;	출력할 메시지를 정의
;;	마지막은 '0'으로 설정하여, .MESSAGELOOP에서 처리할 수 있도록 함
MESSAGE1: db 'HANG64 OS Boot Loader was Started!', 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	chapter 5.2.1

TOTALSECTORCOUNT:		dw		1024		;;	bootloader를 제외한  OS 이미지 크기
																	;;	최대 1152 sector(0x90000 byte)까지 가능
SECTORNUMBER:				db		0x02		;;	OS 이미지가 시작하는 섹터 번호를 저장하는 영역
HEADNUMBER:					db		0x00		;;	OS 이미지가 시작하는 헤드 번호를 저장하는 영역
TRACKNUMBER:				db		0x00		;;	OS 이미지가 시작하는 트랙 번호를 지정하는 영역

	;;	디스크 내용을 메모리로 복사할 address(ES:BX)를 0x10000으로 설정
	mov si, 0x1000									;;	OS image를 복사할 adress(0x10000)를 세그먼트 레지스터 값으로 변환
	mov es, si											;;	ES 세그먼트 레지스터 값으로 변환
	mov bx, 0x0000									;;	BX 레지스터에 0x0000을 설정하여 복사할 address를
																	;;	0x1000:0000(0x10000)으로 최종 설정
	mov di, word[TOTALSECTORCOUNT]	;;	복사할 OS 이미지의 섹터 수를 DI 레지스터에 설정

READDATA:			;;	디스크를 읽는 코드
	;;	모든 섹터를 다 읽었는지 확인
	cmp	di, 0				;;	복사할 OS 이미지의 섹터 수를 '0'과 비교
	je  READEND			;;	복사할 sector 수가 '0'이라면, 다 복사한 것이므로 READEND로 이동
	sub di, 0x01		;;	복사할 sector 수를 '1' 감소

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;	Call a BIOS Read Function
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov	ah, 0x02								;;	BIOS 서비스 #2 (read sector)
	mov al, 0x01								;;	읽을 섹터 수는 '1'
	mov ch, byte[TRACKNUMBER]		;;	읽을 트랙 번호 설정
	mov cl, byte[SECTORNUMBER]	;;	읽을 섹터 번호 설정
	mov dh, byte[HEADNUMBER]		;;	읽을 헤드 번호 설정
	mov dl, 0x00								;;	읽을 드라이브 번호(floppy = 0) 설정
	int 0x13										;;	interrupt service 수행
;;	jc HANDLEDISKERROR					;; 	에러가 발생 했다면, HANDLEDISKERROR로 이동

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;	Calculate address to copy, track, head and sector address
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	add si, 0x0020			;;	512-byte(0x200) 만큼 읽었으므로, 이를 세그먼트 레지스터 값으로 이동
	mov es, si					;;	ES 세그먼트 레지스터에 더해서, address를 한 섹터만큼 증가

	;;	한 섹터를 읽었으므로 섹터 번호를 증가시키고, 마지막 섹터(18)까지 읽었는지 판단
	;;	마지막 섹터가 아니면, 섹터 읽기로 이동해서 다시 섹터 읽기 수행
	mov al, byte[SECTORNUMBER]	;;	섹터 번호를 AL 레지스터에 설정
	add al, 0x01								;;	섹터 번호를 '1' 증가
	mov byte[SECTORNUMBER], al	;;	증가시킨 섹터 번호를 SECTORNUMBER에 다시 설정
	cmp al, 19									;;	증가시킨 섹터 번호를 '19'와 비교
	jl READDATA									;;	섹터 번호가 '19'미만이라면, READDATA로 이동

	;;	마지막 섹터까지 읽었으면(즉, 섹터 번호가 '19'이면), 헤드를 toggle(0->1, 1->0)하고,
	;;	섹터 번호를 '1'로 설정
	xor byte[HEADNUMBER], 0x01		;;	헤드 번호를 0x01과 XOR하여 toggle
	mov byte[SECTORNUMBER], 0x01	;;	섹터 번호를 다시 '1'로 설정

	;;	만약 헤드가 1->0 으로 바뀌었으면, 양쪽 헤드를 모두 읽은 것이므로 아래로 이동하여 track 번호를 '1' 증가
	cmp byte[HEADNUMBER], 0x00		;;	헤드 번호를 0x00과 비교
	jne READDATA									;;	헤드 번호가 '0'이 아니라면 READDATA로 이동

	;;	track을 '1' 증가시킨 후, 다시 섹터 읽기로 이동
	add byte[TRACKNUMBER], 0x01		;;	트랙 번호를 '1'증가
	jmp READDATA									;;	READDATA로 이동
READEND:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


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
