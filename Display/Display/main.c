#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

#define RS 0
#define RW 1
#define E 2

// PORT A = data port
// PORT B = E RW RS

//Стробирующий вход
//Дрыгом напряжения на этой линии мы даем понять дисплею
// что нужно забирать/отдавать данные с/на шину данных.
void strob() {
	PORTB |= (1 << E); //установили Е=1
	_delay_ms(1); 
	PORTB &= ~(1 << E); //установили Е=0
}

//Очистка экрана.
void clear_cmd() {
	PORTA = (1 << 0); 
	strob();
	_delay_ms(100);
}


//сброс сдвигов, Счетчик адреса на 0
void reset_cursor_cmd() {
	PORTA = (1 << 1); 
	strob();
}



// 2- Настройка сдвига экрана и курсора
void shift_screen_setting_cmd(uint8_t id, uint8_t shift) {
	uint8_t ID = 1; // инкремент/декремент счетчика адреса (0 - n-1 ячейка, 1 - n+1) 
	uint8_t S = 0; 	// сдвиг экрана
	PORTA = (1 << 2) | (id << ID) | (shift << S);
	strob();
}

//3 - Настройка режима отображения
void display_setting_cmd(uint8_t onDisp, uint8_t onCursor, uint8_t sqCursor) {
	uint8_t D = 2; // 1 - включить дисплей
	uint8_t C = 1; // 1 - включить курсор в виде прочерка
	uint8_t B = 0; // 1 - сделать курсор в виде мигающего черного квадрата
	PORTA = (1 << 3) | (onDisp << D) | (onCursor << C) | (sqCursor << B);
	strob();
}

//4 - Сдвиг курсора или экрана, в зависимости от битов
void move_cursor_cmd(uint8_t dir) {
	PORTB &= ~(1 << E);
	uint8_t SC = 3; // сдвиг курсора или экрана. Если стоит 0, то сдвигается курсор. Если 1, то экран.
	uint8_t RL = 2; // направление сдвига курсора и экрана. 0 — влево, 1 — вправо
	PORTA = 0;
	
	PORTA |= (1 << 4) | (0 << SC) | (dir << RL);
	strob();
}


//5-инициализация экрана
void LCD_init() {
	//выбираем число линий/ ширину 4/8 бит и размер картинки
	uint8_t DL = 4; // ширина шины данных. 1-8 бит, 0-4 бита
	uint8_t N = 3;  // число строк. 0 — одна строка, 1 — две строки
	uint8_t F = 2;  // размер символа 0 — 5х8 точек. 1 — 5х10 точек 
	
	PORTB &= ~(1 << E);
	PORTA = (1 << 5) | (1 << DL) | (1 << N) | (0 << F);
	strob();

	reset_cursor_cmd();
}

void print_ASCII(char sym) {
	PORTB = (1 << RS);
	PORTA = sym;
	strob();
	PORTB &= ~(1 << RS);
}

void print_string(const char* str) {
	unsigned char i = 0;
	while (str[i] != '\0')
		print_ASCII(str[i++]);
}


//добавляем знак калькулятора
void add_new_symbol() {
	PORTB = (0 << RS);
	// Выбираем в CGRAM адрес 0х08 — как раз начало второго символа
	// что на один символ уходит 8 байт
	PORTA = 0b01001000; 
	strob();
	PORTB = (1 << RS);

	PORTA = 0b00011111;	strob();
	PORTA = 0b00010001;	strob();
	PORTA = 0b00010001;	strob();
	PORTA = 0b00011111;	strob();
	PORTA = 0b00010101;	strob();
	PORTA = 0b00011111;	strob();
	PORTA = 0b00010101;	strob();
	PORTA = 0b00011111;	strob();

	PORTB = (0 << RS);
	//переключение адреса на DDRAM 
	PORTA = 0b10000000; 
	strob();
}

void print_ASCII_by_num(uint8_t num, uint8_t coord) {
	reset_cursor_cmd();
	for (uint8_t i = 0; i < coord; i++)
		move_cursor_cmd(1);
	PORTB = (1 << RS);
	PORTA = num;
	strob();
	PORTB &= ~(1 << RS);
}

int main() {
	DDRB = 0xff;
	DDRA = 0xff;
	PORTB = 0x00;
	
	LCD_init();
	clear_cmd();
	shift_screen_setting_cmd(1, 0);
	display_setting_cmd(1, 0, 0);
	
	print_string("Stopwatch: ");
	add_new_symbol();
	//данные (RS=1), код 01 — символ.
	print_ASCII_by_num(1, 15);
	
	for (int i = 1 ; ; i++) {
		unsigned int a = i;
		int digits = 0;
		_delay_ms(1000);
		while(a) {
			print_ASCII_by_num(a % 10 + '0', 14 - digits++);
			a /= 10;
		}
	}
	return 0;
}