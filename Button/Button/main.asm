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
	
	cbi DDRD, 2 //вывод работает как ВХОД
	sbi portd, 2 //подтягивающий 

	sbi DDRD, 3
	cbi portd, 3

	sbi DDRB, 3
	sbi PORTB,3 

	ldi last, 0b00000100//начальное значение чтобы корректно отработать первый клик
	ldi status, 0b00001000//начальное значение чтобы корректно отработать первый клик

loop:
	in click, PIND //грузим значение из порта

	call Delay

	mov comp2, click //копируем в регистр для сравнения
	ldi comp, 0b00000100//единица с пул апа
	and comp2, comp//0
	eor comp2, comp//00000100

	mov comp, last//копируем в регистр последнее состояние
	mov last, click//переписываем последнее 
	
	and comp, comp2 //00000100 
	ldi comp2, 0b00000100
	eor comp, comp2//xor
	cpi comp, 0b00000100
	brne lamp
	rjmp loop

lamp:
	ldi comp, 0b00001000//выход на светодиод
	eor status, comp//меняем статус( если 00001000, то горит, если еще раз нажимаем, то статус становится 0)
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