#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "LCD.h"

// --- [4. 메인 로직] ---
int main(void) {
    unsigned char rxData;
    char buffer[17]; // 숫자를 임시 저장할 배열
    int i = 0;

    // 초기화 과정
    I2C_Init();
    LCD_Init();
    UART0_Init(9600);

    // [확인용] 부팅 시 메시지 출력
    LCD_Write_4bit(0x80, 0); // 1행 이동
    LCD_WriteString("System Ready");
    _delay_ms(1000); // 1초간 대기
    
    LCD_Write_4bit(0x01, 0); // 화면 지우기
    LCD_Write_4bit(0x80, 0);
    LCD_WriteString("Wait Input...");

    while (1) {
        // UART 데이터 수신 확인
        if (UCSR0A & (1 << RXC0)) {
            rxData = UDR0;

            // 엔터(\r 또는 \n)가 들어오면 화면을 갱신합니다.
            if (rxData == '\r' || rxData == '\n') {
                if (i > 0) { // 입력된 값이 있을 때만
                    buffer[i] = '\0'; // 문자열 끝 표시

                    LCD_Write_4bit(0x01, 0); // 1. 화면을 완전히 지움 (이전 숫자 제거)
                    _delay_ms(2);
                    
                    LCD_Write_4bit(0x80, 0); // 2. 첫 줄 이동
                    LCD_WriteString("Temperature:");
                    LCD_WriteString(buffer); // 2-1. 보관된 숫자(10 이상 포함) 출력
                    LCD_WriteString("C");
                    LCD_Write_4bit(0xC0, 0); // 3. 두 번째 줄 이동
                    LCD_WriteString("Humidity:");
                    LCD_WriteString(buffer); //3-1. 보관된 숫자(10 이상 포함) 출력
                    LCD_WriteString("%");
                    i = 0; // 5. 다음 입력을 위해 버퍼 초기화
                }
            } 
            // 출력 가능한 일반 문자(숫자 등)만 버퍼에 담기
            else if (rxData >= 32 && rxData <= 126) {
                if (i < 16) {
                    buffer[i++] = rxData;
                }
            }
        }
    }
}


