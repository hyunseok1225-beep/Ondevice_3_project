#include "LCD.h"

// --- [1. I2C(TWI) 제어 함수] ---
void I2C_Init() {
    TWSR = 0x00; // 프리스케일러 1
    TWBR = ((F_CPU / 100000L) - 16) / 2; // 100kHz 설정
}

void I2C_Start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))); // 시작 신호 완료 대기
}

void I2C_Stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void I2C_Write(unsigned char data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))); // 데이터 전송 완료 대기
}

// --- [2. I2C LCD 1602 제어 함수] ---
// 주소가 0x27인 경우 0x4E, 0x3F인 경우 0x7E를 사용하세요.
#define LCD_ADDR 0x4E 

void LCD_Write_4bit(unsigned char data, unsigned char rs) {
    unsigned char control = rs ? 0x01 : 0x00; // RS=1(Data), RS=0(Command)
    
    I2C_Start();
    I2C_Write(LCD_ADDR);
    
    // 상위 4비트 전송 (Enable Pulse 포함)
    I2C_Write((data & 0xF0) | control | 0x0C); // En=1, Backlight=1
    _delay_us(2);
    I2C_Write((data & 0xF0) | control | 0x08); // En=0, Backlight=1
    
    // 하위 4비트 전송
    I2C_Write(((data << 4) & 0xF0) | control | 0x0C);
    _delay_us(2);
    I2C_Write(((data << 4) & 0xF0) | control | 0x08);
    
    I2C_Stop();
    _delay_ms(2); // LCD 처리 시간 대기
}

void LCD_Init() {
    _delay_ms(50);
    LCD_Write_4bit(0x33, 0); // 초기화
    LCD_Write_4bit(0x32, 0); // 4비트 모드 설정
    LCD_Write_4bit(0x28, 0); // 2줄, 5x8 폰트
    LCD_Write_4bit(0x0C, 0); // Display ON, Cursor OFF
    LCD_Write_4bit(0x01, 0); // 화면 Clear
    _delay_ms(2);
}

void LCD_WriteString(char *str) {
    while(*str) LCD_Write_4bit(*str++, 1);
}

// --- [3. UART0 설정 함수] ---
void UART0_Init(unsigned int baud) {
    unsigned int ubrr = F_CPU / 16 / baud - 1;
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0); // 수신/송신 가능
    UCSR0C = (3 << UCSZ00);               // 8비트 데이터
}

