#include <avr/io.h>
#include <util/delay.h>

#define LED_DDR     DDRD
#define LED_PORT    PORTD

typedef struct
{
    volatile uint8_t *port; //LED가 연결된 포트
    uint8_t          pin;   //LED가 연결된 핀번호
}LED;

void ledInit(LED *led);
void ledOn(LED *led);
void ledOff(LED *led);