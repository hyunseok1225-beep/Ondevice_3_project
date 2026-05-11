#include "button.h"

void ButtonInit(BUTTON *button, volatile uint8_t *ddr, volatile uint8_t *pin, uint8_t pinNum)
{
    button->ddr = ddr;
    button->pin = pin;
    button->btnPin = pinNum;
    button->prevState = RELEASED; //초기화 상태로 아무것도 안누른 상태
    *button->ddr &= ~(1 << button->btnPin); //버튼에 대한 핀을 입력으로 설정
}

uint8_t ButtonGetState(BUTTON *button)
{
    uint8_t curState = *button->pin & (1 << button->btnPin);    // 현재 버튼의 상태를 읽어옴

    if((curState == PUSHED) && (button->prevState == RELEASED))     // 안누른 상태에서 누르면...
    {
        _delay_ms(50);                  // debounced code
        button->prevState = PUSHED;     // 버튼을 누른 상태로 변환
        return ACT_PUSH;                // 버튼을 눌렀다고 반환
    }
    else if((curState != PUSHED) && (button->prevState == PUSHED)) // 누른상태에서 떼면??
    {
        _delay_ms(50);
        button->prevState = RELEASED;   // 버튼을 뗀 상태로 변환
        return ACT_RELEASE;             // 버튼을 떼었다고 반환
    }
    return NO_ACT;                      // 아무것도 안했다고 반환
}