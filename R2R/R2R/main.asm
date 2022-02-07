.def temp=r16
.def razr1=r17
.def razr2=r18
.def lamp=r19

.org 0
jmp start

.org 0x1C
jmp ADC_switcher

.org 0x100


start:
	ldi temp, LOW(RAMEND)
	out SPL, temp
	ldi temp, HIGH(RAMEND)
	out SPH, temp 
	//LED pushpull
	sbi DDRB, 3 
	sbi DDRD, 7 
	
	ldi temp, ((1 << WGM00)|(1 << WGM01) | (1 << COM01) | (1 << CS01)) 
	out TCCR0, temp

	ldi temp, ((1 << WGM20)|(1 << WGM21) | (1 << COM21) | (1 << CS21)) 
	out TCCR2, temp
//������� ADMUX ����� ������� ������� ����� A ��� ����������� ���, �������������� ���������� � ����� ������� �������.
//� ������, ���� REFS1=0 � REFS0=1, ������� ���������� ������ �� AVCC � ������� �������������, ������������ � AREF.
// ���� ���������� ��� ADLAR, �� ��������� ����-������������.
	ldi temp, (1 << REFS0) | (1 << ADLAR) //���������� - � 0-�� ���� ��������� �������
	out ADMUX, temp
//ADEN=1 �������� ������ ���.
//ADSC ��������� ���� ��������������.
//ADATE-����� ��������������� ����������� ADC
//ADIE - ���� � ���� ���� ����������� �������, � ���������� ��������� ���������, �� ��� ��������� �������������� ����� �������� ������� �� ������� ���������� �� ���.
//ADPS-������������ �������
	ldi temp, (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
	out ADCSRA, temp

	ldi lamp, 0 
	sei 

loop:
	rjmp loop

//��� �� ���� ��������? ��� �������. ����������

ADC_switcher:
	cpi lamp, 0
	brne lamp1
	
lamp0:
	ldi temp, (1 << REFS0)|(1 << ADLAR)|(1 << MUX0)
	out ADMUX, temp
	in razr1, ADCH
	out OCR0, razr1
	ldi lamp, 1
	reti

lamp1:
	ldi temp, (1 << REFS0)|(1 << ADLAR) 
	out ADMUX, temp
	in razr2, ADCH
	out OCR2, razr2
	ldi lamp, 0
	reti
