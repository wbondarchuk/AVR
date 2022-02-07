#define F_CPU 8000000UL
#define BAUD 9600
#define MY_UBRR F_CPU/16/BAUD-1	 // нормальный асинхронный двунаправленный режим работы
#define MAX_SIZE 125 // макс. длина строки

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>


char buffer[MAX_SIZE]; // буфер, в котором хранится строка
int size = 0; //текущий размер буфера
bool complete = false; //флаг заполнения буфера

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
			buffer[size] = c+1; 
			size++;
			break;
	}
	//проверка на заполненность массива
	if (size == MAX_SIZE) {
		buffer[size] = 0;
		complete = true;
	}
}

//Очистка буфера
void clean() {
	buffer[0] = 0;
	size = 0;
	complete = false;
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
	/* Ожидание освобождения буфера передатчика  */
	while ( !( UCSRA & (1<<UDRE)) );//цикл проверки флага UDRE регистра UCSRA(когда буфер окажется пустым, процесс ожидания прерывается, и в него записываются данные
	/* Помещение данных в буфер, отправка данных */
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



int main(void) {
	sei();
	Init(MY_UBRR);
	USART_NewLine("Hello there!");
	while (1) {
		if(complete){
			USART_NewLine(buffer);
			clean();
		}
	}
}