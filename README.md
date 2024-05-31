# MultiButton

## 简介
#### MultiButton 是一个小巧简单易用的事件驱动型按键驱动模块，可无限量扩展按键，按键事件的回调异步处理方式可以简化你的程序结构，去除冗余的按键处理硬编码，让你的按键业务逻辑更清晰。


## 如何选择版本
### 经过二次开发，MultiButton库分为 Pro 和 Lite 两个版本
### Pro：带有全功能全事件；完整的软件消抖功能；每个按键对象独立管理短按、长按、双击触发阈值，便于针对不同功能、特性和手感的按键调参。引入MultiButtonPro文件夹下的文件即可使用
### Lite：仅有基础功能和事件；基础的借助连击机制消抖；各种阈值通过宏定义调整，最节省资源的版本。引入MultiButtonLite文件夹下的文件即可使用
对于资源受限的MCU，建议使用Lite版，随后参考Pro版进行功能拓展修改。
由于结构体和函数命名相同，目前 Pro 和 Lite 两个版本不能同时使用，未来也并无兼容同时使用的计划。

## 使用方法
1.先申请一个按键结构

```c
struct Button button1;
```
2.初始化按键对象，绑定按键的GPIO电平读取接口**read_button_pin()** ，后一个参数设置有效触发电平

```c
    /* Lite版本示例 */
    MTButtonInit(&btn1,            /* 按钮对象指针 */
                read_button_GPIO, /* 按钮电平获得函数 */
                0,                /* 按下有效电平 */
                0);               /* 按钮对象ID */
    /* Pro版本示例 */
    MTButtonInit(&btn1,            /* 按钮对象指针 */
                read_button_GPIO, /* 按钮电平获得函数 */
                0,                /* 按下有效电平 */
                0                 /* 按钮对象ID */
                5,                /* 电平建立有效周期，用于消抖*/
                80,               /* 短按生效时间(Ms) */
                1200);            /* 长按生效时间(Ms)*/
```
3.注册按键事件

```c
MTButtonAttach(&button1, SINGLE_CLICK, Callback_SINGLE_CLICK_Handler);
MTButtonAttach(&button1, DOUBLE_CLICK, Callback_DOUBLE_Click_Handler);
...
```
4.启动按键

```c
MTButtonStart(&button1);
```
5.设置一个5ms间隔的定时器循环调用后台处理函数

```c
while(1) 
{
    MTButtonTicks(5);
    delay(5ms);
}
```

## 特性

MultiButton 使用C语言实现，基于面向对象方式设计思路，每个按键对象单独用一份数据结构管理：

```c
struct Button 
{
	uint16_t ticks;
	uint8_t  repeat: 4;
	uint8_t  event : 4;
	uint8_t  state : 3;
	uint8_t  debounce_cnt : 3;
	uint8_t  active_level : 1;
	uint8_t  button_level : 1;
	uint8_t  button_id;
	uint8_t  (*hal_button_Level)(uint8_t  button_id_);
	BtnCallback  cb[MUTLTIB_EVENT_MAX];
	struct Button* next;
};
```
这样每个按键使用单向链表相连，依次进入 MTButtonHandler(struct Button* handle) 状态机处理，所以每个按键的状态彼此独立。


## 按键事件

事件 | 说明
---|---
PRESS_DOWN | 按键按下，每次按下都触发
PRESS_UP | 按键弹起，每次松开都触发
PRESS_REPEAT | 重复按下触发，变量repeat计数连击次数
SINGLE_CLICK | 单击按键事件
DOUBLE_CLICK | 双击按键事件
LONG_CLICK | 长按击键事件
SHORT_PRESS_START | 达到短按时间阈值时触发一次
LONG_PRESS_START | 达到长按时间阈值时触发一次
LONG_PRESS_HOLD | 长按期间一直触发


## Examples

```c
#include "MultiButtonLite.h"

uint8_t btn1_id = 0;

struct Button btn1;

uint8_t read_button_GPIO(uint8_t button_id)
{
	// you can share the GPIO read function with multiple Buttons
	switch(button_id)
	{
		case btn1_id:
			return HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
			break;

		default:
			return 0;
			break;
	}
}
void BTN1_PRESS_DOWN_Handler(void* btn)
{
	//do something...
}

void BTN1_PRESS_UP_Handler(void* btn)
{
	//do something...
}

...

int main()
{
	MTButtonInit(&btn1, read_button_GPIO, 0, btn1_id);
	MTButtonAttach(&btn1, PRESS_DOWN,       BTN1_PRESS_DOWN_Handler);
	MTButtonAttach(&btn1, PRESS_UP,         BTN1_PRESS_UP_Handler);
	MTButtonAttach(&btn1, PRESS_REPEAT,     BTN1_PRESS_REPEAT_Handler);
	MTButtonAttach(&btn1, SINGLE_CLICK,     BTN1_SINGLE_Click_Handler);
	MTButtonAttach(&btn1, DOUBLE_CLICK,     BTN1_DOUBLE_Click_Handler);
	MTButtonAttach(&btn1, LONG_PRESS_START, BTN1_LONG_PRESS_START_Handler);
	MTButtonAttach(&btn1, LONG_PRESS_HOLD,  BTN1_LONG_PRESS_HOLD_Handler);
	MTButtonStart(&btn1);

	while(1)
	{
        MTButtonTicks(5);
        delay(5ms);
    }
}
```
