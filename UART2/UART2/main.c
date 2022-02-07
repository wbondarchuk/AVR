#define F_CPU 8000000UL
#define BAUD 9600
#define MY_UBRR F_CPU/16/BAUD-1	 // ���������� ����������� ��������������� ����� ������
#define MAX_SIZE 125 // ����. ����� ������

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

uint8_t temp = 0;
char buffer[MAX_SIZE]; // �����, � ������� �������� ������
char buffer_res[MAX_SIZE]; // �����, � ������� �������� ���������
int size = 0; //������� ������ ������
bool complete = false; //���� ���������� ������
bool result = false;//���� ����������
bool error = false;//���� ������


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ISR(USART_RXC_vect) {
	char c = UDR;
	switch(c){
		case(char)10://(/n)
		break;
		case(char)13://(/r) ��������� �� ����� ������ � ���������� ����
		buffer[size] = 0;
		complete = true;
		break;
		default://����������� ���������� ������ �� 1
		buffer[size] = c;
		size++;
		break;
	}
	//�������� �� ������������� �������
	if (size == MAX_SIZE) {
		buffer[size] = 0;
		complete = true;
	}
}

void Init(unsigned int ubrr) {

	//  ��������� ��������
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)(ubrr);
	
	//		RXC			-	���������� �����
	//		|TXC		-	���������� ��������
	//		||UDRE 		-	���������� ������ ��� ��������
	//		|||FE		-	������ �����
	//		||||DOR		-	������ ������������ ������
	//		|||||PE		-	������ ��������
	//		||||||U2X	-	������� ��������
	//		|||||||MPCM	-	����������������� �����
	//		76543210
	UCSRA=0b00100000;

	
	//		RXCIE		-	���������� ��� ����� ������
	//		|TXCIE		-	���������� ��� ���������� ��������
	//		||UDRIE		-	���������� ���������� ������ ��� ��������
	//		|||RXEN		-	���������� �����
	//		||||TXEN	-	���������� ��������
	//		|||||UCSZ2	-	UCSZ0:2 ������ ����� ������
	//		||||||RXB8	-	9 ��� �������� ������
	//		|||||||TXB8	-	9 ��� ���������� ������
	//		76543210
	UCSRB=0b10011000;//  �������� ���� � �������� ������ �� UART
	
	//		URSEL		-	������ 1
	//		|UMSEL		-	�����:1-���������� 0-�����������
	//		||UPM1		-	UPM0:1 ��������
	//		|||UPM0		-	UPM0:1 ��������
	//		||||USBS	-	���� ����: 0-1, 1-2
	//		|||||UCSZ1	-	UCSZ1:2 ������ ����� ������
	//		||||||UCSZ0	-	UCSZ0:2 ������ ����� ������
	//		|||||||UCPOL-	� ���������� ������ - ������������
	//		76543210
	UCSRC=0b10000110;//  8-������� �������, 1 ���� ���
}


//�������� �����
void USART_Transmit( unsigned char data )
{
	// �������� ������������ ������ �����������  
	while ( !( UCSRA & (1<<UDRE)) );//���� �������� ����� UDRE �������� UCSRA(����� ����� �������� ������, ������� �������� �����������, � � ���� ������������ ������
	// ��������� ������ � �����, �������� ������ 
	UDR = data;
}

//�������� ���������
void USART_Message(const char str[]) {
	int i = 0;
	while(str[i]) {
		USART_Transmit(str[i++]);
	}
}

//�������� ������ � ������� �� ����� �������
void USART_NewLine(const char str[]) {
	USART_Message(str);
	USART_Transmit('\n');
	USART_Transmit('\r');
}

//  ��������� �����
char USARTReceiveChar(void) {
	//  ���������������, ����� ������� ��������
	while(!(UCSRA & (1<<RXC)));
	return UDR;
}

//������� ������
void clean() {
	buffer[0] = 0;
	size = 0;
	complete = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void calculator(){
	clean_res();
	if(buffer[0]=='+')
	{
		int r1, r2;
		r1 = buffer[1] & 0x0F;
		r2 = buffer[2] & 0x0F;
		temp = r1 + r2;
		if (temp < 0x0A)
		{
			buffer_res[0] = (temp) | 0x30;
			buffer_res[1] = ' ';
		}
		else {
			buffer_res[0] = (temp / 10) | 0x30;
			buffer_res[1] = (temp % 10) | 0x30;
		}
		result = true;
		error = false;		
	}
	else if(buffer[0]=='-')
	{
		temp = buffer[1] - buffer[2];
		buffer_res[0] = (temp & 0x0F) | 0x30;
		result = true;
		error = false;
	}
	else if(buffer[0]=='*')
		{
			int r1, r2;
			r1 = buffer[1] & 0x0F;
			r2 = buffer[2] & 0x0F;
			temp = r1 * r2;
			if (temp < 0x0A)
			{
				buffer_res[0] = (temp) | 0x30;
				buffer_res[1] = ' ';
			}
			else {
				buffer_res[0] = (temp / 10) | 0x30;
				buffer_res[1] = (temp % 10) | 0x30;
			}
			result = true;
			error = false;		
		}
	else
	{
		error = true;
		result = false;
	}	
		
}

void analyze(){
	if (buffer[0]=='e')
	{
		for(int i = 0; i< size; i++){
			buffer_res[i] = buffer[i+1];
		}
		result = true;
		error = false;
	} else if (buffer[0]=='r')
	{
		for(int i = 0; i < size - 1; i++){
			buffer_res[i] = buffer[size-i-1];
		}
		result = true;
		error = false;
	}
	else
	{
		error = true;
		result = false;
	}
}

// ������� ������ ����������
void clean_res() {
	for(int i = 0; i< size; i++){
		buffer_res[i] = 0;
	}
}

int main(void) {
	sei();
	Init(MY_UBRR);
	USART_NewLine("Hello there!");
	while(1) {
		if(complete) {
			calculator();
			USART_Message("You print: ");
			USART_NewLine(buffer);
			
			if(result) {
				USART_Message("Your result: ");
				USART_NewLine(buffer_res);
				clean();
				clean_res();
			}
			 
			else if(error) {
				USART_NewLine("You got error");
				clean();
				clean_res();
			}
			
			clean();
			clean_res();
		}
	}
}