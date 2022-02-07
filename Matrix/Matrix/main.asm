.def temp = r16				
.def razr = r17		
.def value = r18
.def row = r19		
.def col = r20			

start:
	//������������� �����
    ldi temp, Low(RAMEND)	
	out SPL, temp			
	ldi temp, High(RAMEND)	
	out SPH, temp			
	//�������� 7-�� ����������� �� �
	ldi temp, 0xFF			
	out DDRA, temp		
	//������ 7-�� ����������� �� �	
	ldi temp, 0x01			
	out DDRB, temp
	//������ �� D			
	ldi temp, 0x0F			
	out PORTD, temp			
	//�������� �������
	ldi value, 0			
	//���������� ����� � SRAM
	clr r27					//������� ������� ����
	ldi r26, $60			//������������� $60 � ������� ����
	ldi temp, 0x3F			//���������� �������� ��� 0
	st X+, temp				//��������� �������� temp � SRAM �� ������ $60
	ldi temp, 0x06			//���������� �������� ��� 1
	st X+, temp				//��������� �������� temp � SRAM �� ������ $61
	ldi temp, 0x5B			//���������� �������� ��� 2
	st X+, temp				//��������� �������� temp � SRAM �� ������ $62
	ldi temp, 0x4F			//���������� �������� ��� 3
	st X+, temp				//��������� �������� temp � SRAM �� ������ $63
	ldi temp, 0x66			//���������� �������� ��� 4
	st X+, temp				//��������� �������� temp � SRAM �� ������ $64
	rjmp loop				

//������������ �������� �������
rewrite:			
	clr value				
	ldi temp, 2			//����� ������ �� �������	

//����������� �������� ������
rewrite_row:			
	add value, temp			//+2, �������� �� ��������� ������(1=3, 2=4)
	lsr row					//���������� ����� ������(���� �� 2)
	cpi row, 0b0000			//��������� ������ �� �������?
	brne rewrite_row		//����������

//����������� �������� �������
rewrite_column:			
	inc value				//+1, �������� �� ��������� �������(1=2, 3=4)
	lsr col					//���������� ����� ������(���� �� 2)
	cpi col, 0b0010			//������� �����������?
	brne rewrite_column		//���� ���, �� ����� ������
	sub value, temp			//�������� ������ 2��, �������� ������������ ��������

loop:						
	//������� �������� �� 7-����������
	mov temp, value				//������� ��������
	call Digit					//��������� � ������ � �������� ������ ��� ��� �����
	out PORTA, temp				//������� ������ ��� �� �
	ldi temp, 0x01				//�� ������ ������ ������� �����
	out PORTB, temp				
	
//�������������� ��������� �������� ������
init_row:
	call Delay					
	ldi row, 0b0001				//�������� � ������ ������

//������ �� ��������
row_run:						
	ldi temp, 0b1111		//������ 1
	eor temp, row			//1110
	out PORTD, temp			//���������� 1110
	out DDRD, row			//��������� 1 �� DDRD
	ldi col, 0b0100 		//�������� ��������� �������� �������

//������ �� ��������
column_run:					
	in temp, PIND			//��������� �������� � ����
	and temp, col			//������ �����, ����� ��������� �������(��� ����� 0)
	eor temp, col			//�������� � 1
	cp temp, col			//�������� � ��������
	breq rewrite			//���� ����� �� ������������
	lsl col					//���������� ����� �����(������� �� 2)
	cpi col, 0b10000		//��������� � ������� ���������(����� �� �������)
	breq incr_row		    //��������� � ��������� ������
	rjmp column_run			//��������� ������ �� ��������

//��������� � ��������� ������
incr_row:				
	lsl row					//���������� ����� �����(������� �� 2)
	cpi row, 0b0100			//��������� � ������� ���������(����� �� ������)
	breq init_row		    //������������ ������ �� �������
	rjmp row_run			//��������� ������ �� �������

Digit:
//�������� �� ������ ��������� ���������� �����						
	clr r27					
	ldi r26, $60			
	add r26, temp			
	ld temp, X				
	ret						

Delay:						
	ldi temp, 0xFF			
	ldi razr, 0x0F		

Delay_loop:					
	subi temp, 1			
	sbci razr, 0			
	brne Delay_loop			
	ret		