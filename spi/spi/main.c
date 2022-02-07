#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>


#define SPI_SS 4
#define SPI_MOSI 5
#define SPI_MISO 6
#define SPI_SCK 7

char dg = 8;


//инициализация
void SPI_init(void)
{
	DDRB |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS)|(0<<SPI_MISO); //ножки SPI на выход
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); //включим шину, объ¤вим ведущим, делитель 16
}

//  Передача одного байта данных по SPI
void SPI_SendByte(char byte)
{
	SPDR = byte;
	while(!(SPSR & (1<<SPIF)));//Флаг прерывания от SPI, устанавливается в 1 по окончании передачи байта данных.
}


void Send_7219(char rg, char dt)
{
	PORTB &= ~(1<<SPI_SS);//опускаем ножку SS для выбора микросхемы, затем передаем байт с адресом регистра, затем байт данных
	SPI_SendByte(rg);
	SPI_SendByte(dt);
	PORTB |= (1<<SPI_SS);
}

void Clear_7219(void)
{
	char i = dg;
	do {
		Send_7219(i, 0xF); //символ пустоты
	} while (--i);
}



int main(void)
{
	SPI_init();
	//Send_7219(0x09, 0xFF); 
	Send_7219(0x0B, dg - 1); //сколько разрядов используем
	Send_7219(0x0A,  0x02); //яркость
	Send_7219(0x0C, 1); //включим индикатор
	
	Clear_7219();
	Send_7219(0x01, 0b01111111);
	Send_7219(0x02, 0b01110000);
	Send_7219(0x03, 0b01011111);
	Send_7219(0x04, 0b01011011);
	Send_7219(0x05, 0b00110011);
	Send_7219(0x06, 0b01111001);
	Send_7219(0x07, 0b01101101);
	Send_7219(0x08, 0b00110000);

	
	
}