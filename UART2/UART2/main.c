#define F_CPU 8000000UL
#define BAUD 9600
#define MY_UBRR F_CPU/16/BAUD-1	 // нормальный асинхронный двунаправленный режим работы
#define MAX_SIZE 125 // макс. длина строки

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

uint8_t temp = 0;
char buffer[MAX_SIZE]; // буфер, в котором хранится строка
char buffer_res[MAX_SIZE]; // буфер, в котором хранится результат
int size = 0; //текущий размер буфера
bool complete = false; //флаг заполнения буфера
bool result = false;//флаг результата
bool error = false;//флаг ошибки


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ISR(USART_RXC_vect) {
	char c = UDR;
	switch(c){
		case(char)10://(/n)
		break;
		case(char)13://(/r) проверяем на конец строки и выставляем флаг
		buffer[size] = 0;
		complete = true;
		break;
		default://увеличиваем полученный символ на 1
		buffer[size] = c;
		size++;
		break;
	}
	//проверка на заполненность массива
	if (size == MAX_SIZE) {
		buffer[size] = 0;
		complete = true;
	}
}

void Init(unsigned int ubrr) {

	//  Установка скорости
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)(ubrr);
	
	//		RXC			-	завершение приёма
	//		|TXC		-	завершение передачи
	//		||UDRE 		-	отсутствие данных для отправки
	//		|||FE		-	ошибка кадра
	//		||||DOR		-	ошибка переполнение буфера
	//		|||||PE		-	ошибка чётности
	//		||||||U2X	-	Двойная скорость
	//		|||||||MPCM	-	Многопроцессорный режим
	//		76543210
	UCSRA=0b00100000;

	
	//		RXCIE		-	прерывание при приёме данных
	//		|TXCIE		-	прерывание при завершение передачи
	//		||UDRIE		-	прерывание отсутствие данных для отправки
	//		|||RXEN		-	разрешение приёма
	//		||||TXEN	-	разрешение передачи
	//		|||||UCSZ2	-	UCSZ0:2 размер кадра данных
	//		||||||RXB8	-	9 бит принятых данных
	//		|||||||TXB8	-	9 бит переданных данных
	//		76543210
	UCSRB=0b10011000;//  разрешен приём и передача данных по UART
	
	//		URSEL		-	всегда 1
	//		|UMSEL		-	режим:1-синхронный 0-асинхронный
	//		||UPM1		-	UPM0:1 чётность
	//		|||UPM0		-	UPM0:1 чётность
	//		||||USBS	-	стоп биты: 0-1, 1-2
	//		|||||UCSZ1	-	UCSZ1:2 размер кадра данных
	//		||||||UCSZ0	-	UCSZ0:2 размер кадра данных
	//		|||||||UCPOL-	в синхронном режиме - тактирование
	//		76543210
	UCSRC=0b10000110;//  8-битовая посылка, 1 стоп бит
}


//Отправка байта
void USART_Transmit( unsigned char data )
{
	// Ожидание освобождения буфера передатчика  
	while ( !( UCSRA & (1<<UDRE)) );//цикл проверки флага UDRE регистра UCSRA(когда буфер окажется пустым, процесс ожидания прерывается, и в него записываются данные
	// Помещение данных в буфер, отправка данных 
	UDR = data;
}

//Отправка сообщения
void USART_Message(const char str[]) {
	int i = 0;
	while(str[i]) {
		USART_Transmit(str[i++]);
	}
}

//Отправка строки и перевод на новую строчку
void USART_NewLine(const char str[]) {
	USART_Message(str);
	USART_Transmit('\n');
	USART_Transmit('\r');
}

//  Получение байта
char USARTReceiveChar(void) {
	//  Устанавливается, когда регистр свободен
	while(!(UCSRA & (1<<RXC)));
	return UDR;
}

//Очистка буфера
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

// очистка буфера результата
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