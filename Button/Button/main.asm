.def temp=r16
.def comp=r17
.def comp2=r18
.def last=r19
.def click=r20
.def status=r21
.def razr1=r22
.def razr2=r23

.org 100
jmp start

start:
	ldi temp, Low(RAMEND)
	out SPL, temp
	ldi r16, High(RAMEND)
	out SPH, temp
	
	cbi DDRD, 2 //����� �������� ��� ����
	sbi portd, 2 //������������� 

	sbi DDRD, 3
	cbi portd, 3

	sbi DDRB, 3
	sbi PORTB,3 

	ldi last, 0b00000100//��������� �������� ����� ��������� ���������� ������ ����
	ldi status, 0b00001000//��������� �������� ����� ��������� ���������� ������ ����

loop:
	in click, PIND //������ �������� �� �����

	call Delay

	mov comp2, click //�������� � ������� ��� ���������
	ldi comp, 0b00000100//������� � ��� ���
	and comp2, comp//0
	eor comp2, comp//00000100

	mov comp, last//�������� � ������� ��������� ���������
	mov last, click//������������ ��������� 
	
	and comp, comp2 //00000100 
	ldi comp2, 0b00000100
	eor comp, comp2//xor
	cpi comp, 0b00000100
	brne lamp
	rjmp loop

lamp:
	ldi comp, 0b00001000//����� �� ���������
	eor status, comp//������ ������( ���� 00001000, �� �����, ���� ��� ��� ��������, �� ������ ���������� 0)
	out PORTB, status
	rjmp loop

Delay:
	ldi razr1, 0xFF 
	ldi razr2, 0xFF
	

Delay_loop:
	subi razr1, 1 
	sbci razr2,	 0
	brne Delay_loop 
	ret