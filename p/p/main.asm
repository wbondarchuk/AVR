	.def temp	=	r16 
	.def razr1	=	r17
	.def razr2	=	r18
	.def bright	=	r19
	
	.org	0x00
	rjmp	start

	.org	0x0C  
	jmp		On 

	.org	0x100

start:
	ldi		temp, Low(RAMEND)
	out		SPL, temp
	ldi		temp, High(RAMEND)
	out		SPH, temp

	sbi		DDRB,3	
	ldi		temp, (1<<WGM01)|(1<<WGM00)|(1<<COM01)|(1<<CS00)

	out		TCCR0, temp

	ldi		bright, 0
	out		OCR0, bright

On:
	inc		bright
	out		OCR0, bright
	call	Delay
	cpi		bright, 128
	brne	On
	rjmp	Off

Off:
	dec		bright
	out		OCR0, bright
	call	Delay
	cpi		bright, 0
	brne	Off
	rjmp	On

Delay:
	ldi		razr1, 0xFF 
	ldi		razr2, 0x5F

Delay_loop:
	subi	razr1,1 
	sbci	razr2,0
	brne	Delay_loop 
	ret