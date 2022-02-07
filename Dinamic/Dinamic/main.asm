.def temp	=	r16 
.def razr1	=	r17
.def razr2	=	r18
.def razr3	=	r19
.def sys	=	r20
.def num	=	r21

.equ ch0	=	0b11101011
.equ ch1	=	0b00101000
.equ ch2	=	0b10110011
.equ ch3	=	0b10111010
.equ ch4	=	0b01111000
.equ ch5	=	0b11011010
.equ ch6	=	0b11011011
.equ ch7	=	0b10101000
.equ ch8	=	0b11111011
.equ ch9	=	0b11111010
.equ ch10	=	0b00001111






.cseg // Программный сегмент
.org 0 
jmp start // Переходим на start
.org 0x12 // Адрес вектора таймера 0 по переполнению
jmp TIM0_OVF  // Переходим на TIM0_OVF

start: 
	ldi temp, high(RAMEND) // Инициализируем стек
	out sph, temp
	ldi temp, low(RAMEND)
	out spl, temp


	ldi temp, 0x0F // Порты D и A на выход, на нем висит наш индикатор 
	out DDRB, temp
	ldi temp, 0xFF
	out DDRA, temp

 // сохраняем в ОЗУ, 1 ячейка
	ldi num, ch8
	sts Digit+1, num
	ldi num, ch8
	sts Digit+2, num
	ldi num, ch8
	sts Digit+3, num

	ldi ZH,High(CH*2)
	ldi ZL,Low(CH*2) 	

	ldi temp, 0b00000011// Ставим предделитель 11
	out TCCR0, temp 

	ldi temp, 0b00000001 // TOV0 = 1 бит прерывания по переполнению таймера 0 
	out TIFR, temp
	out TIMSK, temp

	ldi temp, 0xFE 
	out TCNT0, temp // А тут в тикающий регистр засовываем FE, чтобы сразу влетать в прерывания после sei

	ldi sys, 0b00000001 // ставим начальное положение sys
	sei // Разрешаем прерывания

//через косвенную адрессацию
Loop:
	lpm num, Z+
	sts Digit, num
	call Delay
	/*ldi num, ch9 // Пихаем 9 в регистр
	sts Digit, num // сохраняем в ОЗУ, 1 ячейка
	call Delay // Вызываем подпрограмму задержки
	ldi num, ch8 // Дальше по аналогии, только меняем выводимые цифры
	sts Digit, num
	call Delay
	ldi num, ch7
	sts Digit, num
	call Delay
	ldi num, ch6
	sts Digit, num
	call Delay
	ldi num, ch5
	sts Digit, num
	call Delay
	ldi num, ch4
	sts Digit, num
	call Delay
	ldi num, ch3
	sts Digit, num
	call Delay
	ldi num, ch2
	sts Digit, num
	call Delay
	ldi num, ch1
	sts Digit, num
	call Delay
	ldi num, ch0
	sts Digit, num
	call Delay*/
	rjmp Loop 



Delay:
	ldi razr1, 0xFF
	ldi razr2, 0xFF
	ldi razr3, 0x2F


Delay_loop :
	dec razr1
	brne Delay_loop 
	dec razr2
	brne Delay_loop 
	dec razr3
	brne Delay_loop 
	ret


TIM0_OVF: // Прерывание по переполнению
	cli // Запрещаем прерывания
	lsl sys // Производим логический сдвиг влево
	cpi sys, 0b00010000 // Проверяем, не ушли ли за пределы сегментов отображения на индикаторе
	breq reset // Если ушли, переходим на обнуление т.е. метка reset
	out PORTB, sys // Если нет, выводим значение sys в порт
	ld temp, X+ // Увеличиваем адрес регистра косвенной адресации
	ld temp, X // Загружаем по данному адресу 
	out PORTA, temp // И выводим в порт A


rebuild: 
	ldi temp, 0x00 // Взводим тикающий регистр
	out TCNT0, temp
	sei // Разрешаем прерывания
	reti // Выходим из прерывания


reset:
	ldi sys, 0b00000001 // В sys значение
	out PORTB, sys // Выводим на порт
	ldi XH, high(Digit) // Тут произведем зарядку 1 адреса нашего Digit в регистр косвенной
	ldi XL, low(Digit) // адресации, сперва старший, потом младший
	ld temp, X // загрузим значение из X(Digit) в temp 
	out PORTA, temp // И выкинем на порт  
	rjmp rebuild 

CH :.db 0b11101011, 0b00101000, 0b10110011, 0b10111010, 0b01111000, 0b11011010, 0b11011011, 0b10101000, 0b11111011, 0b11111010
.dseg // сегмент ОЗУ
Digit : .byte	4
 

/*
.cseg
.org 0x00
rjmp start

CH :.db 0b11101011, 0b00101000, 0b10110011, 0b10111010, 0b01111000, 0b11011010, 0b11011011, 0b10101000, 0b11111011, 0b11111010

start: 
	ldi r16, Low(RAMEND) ; stack pointer
	out SPL, r16
	ldi r16, High(RAMEND)
	out SPH, r16
	//ldi ZH,high(CH*2) 
	ldi ZL,Low(CH*2) 	


	ldi r16, 0x0F
	out DDRb,r16
	ldi r16, 0xFF
	out ddra,r16



Loop:
	lpm r17,Z+
	ldi r18, 0b11111010
	cp r17, r18
	breq equal
	out porta,r17
	sbi portb, 0
	call Delay
	rjmp Loop

equal:
	//clr ZL 
	LDI ZL, Low(CH*2) 
	rjmp Loop

Delay:
	ldi razr1, 0xFF
	ldi razr2, 0xFF
	ldi razr3, 0x2F


Delay_loop :
	dec razr1
	brne Delay_loop 
	dec razr2
	brne Delay_loop 
	dec razr3
	brne Delay_loop 
	ret
*/	