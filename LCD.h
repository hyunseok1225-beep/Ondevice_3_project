#include <avr/io.h>
#include <util/delay.h>


// --- [1. I2C(TWI) 제어 함수] ---

void I2C_Init();
void I2C_Start();
void I2C_Stop();
void I2C_Write(unsigned char data);

// --- [2. I2C LCD 1602 제어 함수] ---

void LCD_Write_4bit(unsigned char data, unsigned char rs);
void LCD_Init();
void LCD_WriteString(char *str);

// --- [3. UART0 설정 함수] ---
void UART0_Init(unsigned int baud);



