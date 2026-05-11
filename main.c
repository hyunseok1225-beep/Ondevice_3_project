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
// // --- [1. I2C(TWI) 제어 함수] ---
// void I2C_Init() {
//     TWSR = 0x00; // 프리스케일러 1
//     TWBR = ((F_CPU / 100000L) - 16) / 2; // 100kHz 설정
// }

// void I2C_Start() {
//     TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
//     while (!(TWCR & (1 << TWINT))); // 시작 신호 완료 대기
// }

// void I2C_Stop() {
//     TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
// }

// void I2C_Write(unsigned char data) {
//     TWDR = data;
//     TWCR = (1 << TWINT) | (1 << TWEN);
//     while (!(TWCR & (1 << TWINT))); // 데이터 전송 완료 대기
// }

// // --- [2. I2C LCD 1602 제어 함수] ---
// // 주소가 0x27인 경우 0x4E, 0x3F인 경우 0x7E를 사용하세요.
// #define LCD_ADDR 0x4E 

// void LCD_Write_4bit(unsigned char data, unsigned char rs) {
//     unsigned char control = rs ? 0x01 : 0x00; // RS=1(Data), RS=0(Command)
    
//     I2C_Start();
//     I2C_Write(LCD_ADDR);
    
//     // 상위 4비트 전송 (Enable Pulse 포함)
//     I2C_Write((data & 0xF0) | control | 0x0C); // En=1, Backlight=1
//     _delay_us(2);
//     I2C_Write((data & 0xF0) | control | 0x08); // En=0, Backlight=1
    
//     // 하위 4비트 전송
//     I2C_Write(((data << 4) & 0xF0) | control | 0x0C);
//     _delay_us(2);
//     I2C_Write(((data << 4) & 0xF0) | control | 0x08);
    
//     I2C_Stop();
//     _delay_ms(2); // LCD 처리 시간 대기
// }

// void LCD_Init() {
//     _delay_ms(50);
//     LCD_Write_4bit(0x33, 0); // 초기화
//     LCD_Write_4bit(0x32, 0); // 4비트 모드 설정
//     LCD_Write_4bit(0x28, 0); // 2줄, 5x8 폰트
//     LCD_Write_4bit(0x0C, 0); // Display ON, Cursor OFF
//     LCD_Write_4bit(0x01, 0); // 화면 Clear
//     _delay_ms(2);
// }

// void LCD_WriteString(char *str) {
//     while(*str) LCD_Write_4bit(*str++, 1);
// }

// // --- [3. UART0 설정 함수] ---
// void UART0_Init(unsigned int baud) {
//     unsigned int ubrr = F_CPU / 16 / baud - 1;
//     UBRR0H = (unsigned char)(ubrr >> 8);
//     UBRR0L = (unsigned char)ubrr;
//     UCSR0B = (1 << RXEN0) | (1 << TXEN0); // 수신/송신 가능
//     UCSR0C = (3 << UCSZ00);               // 8비트 데이터
// }

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


// #define F_CPU 16000000UL
// #include <avr/io.h>
// #include <util/delay.h>

// // --- [UART 관련 설정 및 함수] ---
// void UART0_Init(unsigned int baud) {
//     // 보레이트 설정 (9600 기준)
//     unsigned int ubrr = F_CPU / 16 / baud - 1;
//     UBRR0H = (unsigned char)(ubrr >> 8);
//     UBRR0L = (unsigned char)ubrr;
    
//     // 송수신 가능 설정 (RXEN, TXEN)
//     UCSR0B = (1 << RXEN0) | (1 << TXEN0);
//     // 데이터 비트 8비트 설정
//     UCSR0C = (3 << UCSZ00);
// }

// unsigned char UART0_Receive(void) {
//     // 데이터가 수신될 때까지 대기
//     while (!(UCSR0A & (1 << RXC0)));
//     return UDR0; // 수신된 데이터 반환
// }

// // --- [I2C(TWI) 관련 함수] ---
// void I2C_Init() {
//     TWSR = 0x00;
//     TWBR = ((F_CPU / 100000L) - 16) / 2; // 100kHz
// }

// void I2C_Start() {
//     TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
//     while (!(TWCR & (1 << TWINT)));
// }

// void I2C_Stop() {
//     TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
// }

// void I2C_Write(unsigned char data) {
//     TWDR = data;
//     TWCR = (1 << TWINT) | (1 << TWEN);
//     while (!(TWCR & (1 << TWINT)));
// }

// // --- [LCD 제어 함수] ---
// #define LCD_ADDR 0x4E // 0x27 << 1

// void LCD_Write_4bit(unsigned char data, unsigned char rs) {
//     unsigned char control = rs ? 0x01 : 0x00; // RS=1(Data), RS=0(Command)
    
//     I2C_Start();
//     I2C_Write(LCD_ADDR);
    
//     // High Nibble
//     I2C_Write((data & 0xF0) | control | 0x0C); // En=1, Backlight=1
//     _delay_us(2);
//     I2C_Write((data & 0xF0) | control | 0x08); // En=0, Backlight=1
    
//     // Low Nibble
//     I2C_Write(((data << 4) & 0xF0) | control | 0x0C);
//     _delay_us(2);
//     I2C_Write(((data << 4) & 0xF0) | control | 0x08);
    
//     I2C_Stop();
//     _delay_ms(2);
// }

// void LCD_Init() {
//     _delay_ms(50);
//     LCD_Write_4bit(0x33, 0); // 초기화 시퀀스
//     LCD_Write_4bit(0x32, 0); // 4비트 모드 설정
//     LCD_Write_4bit(0x28, 0); // 2줄, 5x8
//     LCD_Write_4bit(0x0C, 0); // Display ON
//     LCD_Write_4bit(0x01, 0); // Clear Display
//     _delay_ms(2);
// }

// // --- [메인 함수] ---
// int main(void) {
//     unsigned char rxData;

//     UART0_Init(9600); // UART 초기화 (9600bps)
//     I2C_Init();       // I2C 초기화
//     LCD_Init();       // LCD 초기화

   
//     LCD_Write_4bit(0x80, 0); // 1행 1열
//     char *msg = "UART Input:";
//     while(*msg) LCD_Write_4bit(*msg++, 1);

//     LCD_Write_4bit(0xC0, 0); // 2행 1열 (입력값이 찍힐 곳)

  

//     while (1) {
//     rxData = UART0_Receive(); // UART 데이터 한 글자 수신 대기

//     LCD_Write_4bit(0x01, 0); 
//     _delay_ms(2); // 지우는 데는 시간이 조금 걸리므로 대기

//     LCD_Write_4bit(0x80, 0); 

//     char *prefix = "Input: ";
//     while(*prefix) LCD_Write_4bit(*prefix++, 1);

//     LCD_Write_4bit(rxData, 1);
//   }
// }

// int main(void) {
//     unsigned char rxData;
//     char buffer[17]; // 숫자를 임시 저장할 버퍼
//     int i = 0;
//     int final_number = 0; // 변환된 정수 값이 저장될 변수

//     I2C_Init();
//     LCD_Init();
//     UART0_Init(9600);

//     while (1) {
//         rxData = UART0_Receive();

//         // 엔터가 들어오면 지금까지 모은 문자를 숫자로 변환
//         if (rxData == '\r' || rxData == '\n') {
//             buffer[i] = '\0'; // 문자열 끝 표시
//             final_number = atoi(buffer); // "123" -> 123 (정수) 변환

//             // LCD 출력 (확인용)
//             LCD_Write_4bit(0x01, 0); // 화면 지우기
//             _delay_ms(2);
            
//             // 숫자가 10 이상인지 판별하는 로직 예시
//             if (final_number >= 10) {
//                 char *msg = "Num >= 10";
//                 LCD_Write_4bit(0x80, 0); // 1행
//                 while(*msg) LCD_Write_4bit(*msg++, 1);
//             }

//             // 받은 숫자 그대로 출력
//             LCD_Write_4bit(0xC0, 0); // 2행
//             char *p = buffer;
//             while(*p) LCD_Write_4bit(*p++, 1);

//             i = 0; // 버퍼 초기화
//         } 
//         else if (rxData >= '0' && rxData <= '9') {
//             if (i < 16) {
//                 buffer[i++] = rxData; // 숫자 문자열 모으기
//             }
//         }
//     }
// }










// void uart0_init()
// {
//   UCSR0A |= (1<<U2X0); // 2배속 모드 활성화
//   UCSR0B |= (1<<RXEN0) | (1<<TXEN0); // 송수신 활성화
//   UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00); // 8비트 데이터 전송, 패리티비트 없음, 스톱비트 1비트
//   UBRR0H = 0x00;
//   UBRR0L = 207; //9600 bps 설정 (16MHz 기준)

// }

// void uart0_transmit(char data)
// {

//   while(!(UCSR0A & (1<<UDRE0))); // 송신 버퍼가 비어질 때까지 대기
//   UDR0 = data; // 데이터 전송

// }

// unsigned uart0_receive()
// {
//   while(!(UCSR0A & (1<<RXC0))); // 수신 버퍼에 데이터가 들어올 때까지 대기
//   return UDR0; // 수신된 데이터 반환
// }

// int main()
// {
//   uart0_init(); // UART 초기화

//   while(1)
//   {
//         uart0_transmit(uart0_receive()); // 수신된 데이터를 다시 UART로 전송 (에코)
//   }
// }



// int main()
// {
//   uint8_t fndNumber[]=
//   {
//     0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x67
//   };

//   int count = 0;

//   DDRA = 0xff; // FND의 a,b,c,d,e,f,g,dp 핀을 출력으로 설정

//   while(1)
//   {
//     PORTA = fndNumber[count]; // FND에 숫자 출력
//     count = (count + 1) % 10; // 0부터 9까지 반복
//     _delay_ms(500); // 1초 지연
//   }
// }


// int main()
// {
//   DDRB |= (1<<PB5);// B5를 출력으로 설정
//   TCCR1A |= (1<<COM1A1)|(1<<WGM11); // Clear OCnA/OCnB/OCnC on compare match (set output to low level).,PWM, Phase Correct, 9-bit
//   TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //    CTC  , clkI/O/64 (From prescaler)
//   TCCR1C = 0;

//  ICR1 = 4999;// TOP, 50 Hz
//    while(1)
//    {
//       OCR1A = 4500;//10%
//      _delay_ms(500);
    

//    }

// }

// int main()
// {
//   //DDRB = 0b00010000;
//   DDRB |= (1<<PB4); // PB4를 출력으로 설정
//   TCCR0 |= (1<<WGM00)|(1<<WGM01)|(1<<COM01)|(1<<CS02); // Fast PWM 모드, OC0 핀 토글, 분주비 64

//   //OCR0 = 64;

//   while(1)
//   {
//     for(uint8_t i = 0; i <= 255; i++)
//     {
//       OCR0 = i; // PWM 듀티 사이클을 증가시킴
//       _delay_ms(10); // 10ms 지연
//     }
//   }
// }



// int main()
// {
//   DDRB = 0XFF; // PORTB를 출력으로 설정

//   PORTB = 0;
//   TCCR0 |= (1<<CS02) | (1<<CS00);
//   TCNT0 = 6;
//   while(1)
//   {
//     while((TIFR & 0x01)==0);
//     PORTB = ~PORTB;
//     TCNT0 = 6;
//     TIFR =0x01;
//   }

// }

//CTC
// int main()
// {
//   DDRB =0x10; // 0b 00010000 -> PB4  를 출력으로 설정
//   //TCCR0 = 0b00011100; // 0x1C
//   TCCR0 |= (1<<COM00) | (1<<WGM01) | (1<<CS02);

  
//   OCR0 = 124;

//   while(1)
//   {
//     while((TIFR & 0x02) == 0);
    
//     TIFR = 0x02;
//     OCR0 = 124;

//   }

// }





// #define LED_DDR DDRD
// #define LED_PORT PORTD
// #define BUTTON_DDR DDRG
// #define BUTTON_PIN PING

// #define BUTTON_ON 2
// #define BUTTON_OFF 3
// #define BUTTON_TOGGLE 4

// enum {PUSHED, RELEASED};
// enum 
// {
//   NO_ACT, 
//   ACT_PUSH, 
//   ACT_RELEASE

// };

// typedef struct 

// {
//     volatile uint8_t *ddr;
//     volatile uint8_t *pin;
//     uint8_t           btnPin;
//     uint8_t           prevState;

// }BUTTON;

// void ButtonInit(BUTTON *button, volatile uint8_t *ddr, volatile uint8_t *pin, uint8_t pinNum)
// {
//   button -> ddr = ddr;
//   button -> pin = pin;
//   button -> btnPin = pinNum;
//   button -> prevState = RELEASED; //초기화 상태로 아무것도 안누른 상태
//   *button -> ddr &= ~(1 << button -> btnPin); //버튼에 대한 핀을 입력으로 설정
// }

// uint8_t ButtonGetState(BUTTON *button)
// {
//   uint8_t curState = *button->pin & (1 << button->btnPin); // 현재 버튼의 상태를 읽어옴

//   if((curState ==PUSHED) && (button->prevState == RELEASED)) // 버튼이 눌렸을 때
//   {
//    _delay_ms(50); // 디바운싱을 위해 50ms 지연
//    button->prevState = PUSHED; // 이전 상태를 눌린 상태로 업데이트
//   return ACT_PUSH;
  
//   }

//   else if((curState != PUSHED) && (button->prevState == PUSHED)) // 버튼이 떼졌을 때
//   {
//     _delay_ms(20); // 디바운싱을 위해 50ms 지연
//     button->prevState = RELEASED; // 이전 상태를 떼어진 상태로 업데이트
//     return ACT_RELEASE;
//   }
//   return NO_ACT;

// }

// int main()
// {
//   LED_DDR = 0xFF; // LED 포트를 출력으로 설정

//   BUTTON btnOn;
//   BUTTON btnOff;
//   BUTTON btnTog;

//   ButtonInit(&btnOn, &BUTTON_DDR, &BUTTON_PIN, BUTTON_ON );
//   ButtonInit(&btnOff, &BUTTON_DDR, &BUTTON_PIN, BUTTON_OFF );
//   ButtonInit(&btnTog, &BUTTON_DDR, &BUTTON_PIN, BUTTON_TOGGLE);

//   while(1)
//   {
//     if(ButtonGetState(&btnOn) == ACT_RELEASE)
//     {
//       LED_PORT = 0xff;
//     }
//     if(ButtonGetState(&btnOff) ==ACT_RELEASE)
//     {
//       LED_PORT = 0x00;
//     }
//     if(ButtonGetState(&btnTog) == ACT_RELEASE)
//     {
//       LED_PORT ^= 0xff;
//     }
  
//   }
// }




// int main()
// {
//   DDRD = 0xFF;
//   DDRG = 0x00;

//   uint8_t ledData = 0x01;
  
//   uint8_t buttonData;


//   PORTD = 0x00;

//   while(1)
//   {
//     buttonData = PING;

//     if((buttonData & (1<<2)) == 0 )
//     {
//       PORTD = ledData;
//       ledData = (ledData >> 7) | (ledData<<1);
//       _delay_ms(200);
    
//     }

//     if((buttonData & (1<<3)) == 0 )
//     {
//       PORTD = ledData;
//       ledData = (ledData >> 1) | (ledData<<7);
//       _delay_ms(200);
    
//     }

//     if((buttonData & (1<<4)) == 0 )
//     {
//       PORTD = 0x00;
      
//     }

//   }
  
// }




// int main(){
    
//     DDRD = 0xff;    // LED bar(output port)

//     DDRG  &= ~(1<<4); // DDRG's 4th port is input

    
//     while (1)
//     {
//        if(PING & (1<<4))// if the 4th port of PING is high, then turn on the LED bar
//          {
//             PORTD = 0xff; // turn on the LED bar
//          }
//        else
//         {
//             PORTD = 0x00; // turn off the LED bar
//          }
//        {

//        }
//         /* code */
//     }
    
// }
