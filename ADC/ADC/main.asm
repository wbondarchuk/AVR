.def temp = r16
.def inter = r17

.org 0x00
rjmp start

.org 0x012
rjmp timer

.org 0x01C
rjmp adc_int

.org 0x100

start:
	ldi temp, low(RAMEND)
	out SPL, temp
	ldi temp, high(RAMEND)
	out SPH, temp
	sbi DDRB, 3


timer:
	ldi temp, ((1 << WGM00)|(1 << WGM01) | (1 << COM01) | (1 << CS01))
	ldi inter, (1 << TOIE0)
	out TCCR0, temp
	sts TIMSK, inter

adc_set:
//������� ADMUX ����� ������� ������� ����� A ��� ����������� ���, �������������� ���������� � ����� ������� �������.
//� ������, ���� REFS1=0 � REFS0=1, ������� ������� ������ �� AVCC � ������� �������������, ������������ � AREF.
// ���� ���������� ��� ADLAR, �� ��������� ����-������������.
	ldi temp, ((1 << ADLAR) | (1 << REFS0))
	out ADMUX, temp
//ADEN=1 �������� ������ ���.
//ADSC ��������� ���� ��������������.
//ADATE-����� ��������������� ����������� ADC
//ADIE - ���� � ���� ���� ����������� �������, � ���������� ��������� ���������, �� ��� ��������� �������������� ����� �������� ������� �� ������� ���������� �� ���.
//ADPS-������������ �������
	ldi temp, (1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)
	out ADCSRA, temp
	sei

adc_int:
	in temp, ADCH
	out OCR0, temp
	reti