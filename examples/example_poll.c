#define LITE 1
#define PRO  0

#if LITE
#include "MultiButtonLite.h"
#elif PRO
#include "MultiButtonPro.h"
#endif

unit8_t       btn1_id = 0;
struct Button btn1;

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
    default:
        return 0;
    }
}

int main( )
{
    static PressEvent btn1_event_val;

#if LITE
    /* Lite版本示例 */
    MTButtonInit(&btn1,            /* 按钮对象指针 */
                 read_button_GPIO, /* 按钮电平获得函数 */
                 0,                /* 按下有效电平 */
                 btn1_id);         /* 按钮对象ID */
#elif PRO
    /* Pro版本示例 */
    MTButtonInit(&btn1,            /* 按钮对象指针 */
                 read_button_GPIO, /* 按钮电平获得函数 */
                 0,                /* 按下有效电平 */
                 btn1_id           /* 按钮对象ID */
                 5,                /* 电平建立有效周期，用于消抖*/
                 80,               /* 短按生效时间(Ms) */
                 1200);            /* 长按生效时间(Ms)*/
#endif

    MTButtonStart(&btn1);

    while(1)
    {
        MTButtonTicks(5);
        delay(5ms);

        if(btn1_event_val != MTButtonEventGet(&btn1))
        {
            btn1_event_val = MTButtonEventGet(&btn1);

            if(btn1_event_val == PRESS_DOWN)
            {
                //do something
            }
            else if(btn1_event_val == PRESS_UP)
            {
                //do something
            }
            else if(btn1_event_val == LONG_PRESS_HOLD)
            {
                //do something
            }
        }
    }
}
