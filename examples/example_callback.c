#define LITE 1
#define PRO  0

#if LITE
#include "MultiButtonLite.h"
#elif PRO
#include "MultiButtonPro.h"
#endif

enum Button_IDs
{
    btn1_id,
    btn2_id,
};

struct Button btn1;
struct Button btn2;

/**
 * @brief 示例绑定的按钮电平获得函数(实际上是虚拟电平，只要是0或1即可)
 * @param button_id 触发的按钮IO
 * @return 电平0或1
 */
uint8_t read_button_GPIO(uint8_t button_id)
{
    // you can share the GPIO read function with multiple Buttons
    switch(button_id)
    {
    case btn1_id:
        return HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
    case btn2_id:
        return HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin);
    default:
        return 0;
    }
}

int main( )
{
    MTButtonInit(&btn1, read_button_GPIO, 0, btn1_id);
    MTButtonInit(&btn2, read_button_GPIO, 0, btn2_id);
#if LITE
    /* Lite版本示例 */
    MTButtonInit(&btn1,            /* 按钮对象指针 */
                 read_button_GPIO, /* 按钮电平获得函数 */
                 0,                /* 按下有效电平 */
                 btn1_id);         /* 按钮对象ID */
    MTButtonInit(&btn2,            /* 按钮对象指针 */
                 read_button_GPIO, /* 按钮电平获得函数 */
                 0,                /* 按下有效电平 */
                 btn2_id);         /* 按钮对象ID */
#elif PRO
    /* Pro版本示例 */
    MTButtonInit(&btn1,            /* 按钮对象指针 */
                 read_button_GPIO, /* 按钮电平获得函数 */
                 0,                /* 按下有效电平 */
                 btn1_id           /* 按钮对象ID */
                 5,                /* 电平建立有效周期，用于消抖*/
                 80,               /* 短按生效时间(Ms) */
                 1200);            /* 长按生效时间(Ms)*/
    MTButtonInit(&btn2,            /* 按钮对象指针 */
                 read_button_GPIO, /* 按钮电平获得函数 */
                 0,                /* 按下有效电平 */
                 btn2_id           /* 按钮对象ID */
                 5,                /* 电平建立有效周期，用于消抖*/
                 400,              /* 短按生效时间(Ms) */
                 1200);            /* 长按生效时间(Ms)*/
#endif

    MTButtonAttach(&btn1, PRESS_DOWN, BTN1_PRESS_DOWN_Handler);
    MTButtonAttach(&btn1, PRESS_UP, BTN1_PRESS_UP_Handler);
    MTButtonAttach(&btn1, PRESS_REPEAT, BTN1_PRESS_REPEAT_Handler);
    MTButtonAttach(&btn1, SINGLE_CLICK, BTN1_SINGLE_Click_Handler);
    MTButtonAttach(&btn1, DOUBLE_CLICK, BTN1_DOUBLE_Click_Handler);
    MTButtonAttach(&btn1, LONG_PRESS_START, BTN1_LONG_PRESS_START_Handler);
    MTButtonAttach(&btn1, LONG_PRESS_HOLD, BTN1_LONG_PRESS_HOLD_Handler);

    MTButtonAttach(&btn2, PRESS_DOWN, BTN2_PRESS_DOWN_Handler);
    MTButtonAttach(&btn2, PRESS_UP, BTN2_PRESS_UP_Handler);
    MTButtonAttach(&btn2, PRESS_REPEAT, BTN2_PRESS_REPEAT_Handler);
    MTButtonAttach(&btn2, SINGLE_CLICK, BTN2_SINGLE_Click_Handler);
    MTButtonAttach(&btn2, DOUBLE_CLICK, BTN2_DOUBLE_Click_Handler);
    MTButtonAttach(&btn2, LONG_PRESS_START, BTN2_LONG_PRESS_START_Handler);
    MTButtonAttach(&btn2, LONG_PRESS_HOLD, BTN2_LONG_PRESS_HOLD_Handler);

    MTButtonStart(&btn1);
    MTButtonStart(&btn2);

    while(1)
    {
        MTButtonTicks(5);
        delay(5ms);
    }
}

void BTN1_PRESS_DOWN_Handler(void *btn)
{
    //do something...
}

void BTN1_PRESS_UP_Handler(void *btn)
{
    //do something...
}
