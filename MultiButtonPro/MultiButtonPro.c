/********************************************************************************


 **** Copyright (C), 2024, Yuanlong Xu <Yono233@outlook.com>    ****
 **** All rights reserved                                       ****

 ********************************************************************************
 * File Name     : MultiButtonPro.c
 * Author        : Yuanlong Xu
 * Date          : 2024-05-13
 * Version       : 1.0
********************************************************************************/
/**************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "MultiButtonPro.h"
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static MT_BUTTON *head_handle = NULL; // 按钮对象链头指针
/* Private Constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

#define PRESS_REPEAT_MAX_NUM 15 /* 重复计数器的最大值 */

/**
 * @brief 检查事件函数并且触发事件处理
 * @param ev 事件值
 */
#define EVENT_CB(ev)   \
    if(handle->cb[ev]) \
    handle->cb[ev]((void *)handle)

/* Private function prototypes -----------------------------------------------*/
static void MTButtonHandler(MT_BUTTON *handle, uint8_t cycle);
/* Private functions ---------------------------------------------------------*/

/**
 * @brief 初始化按钮对象
 * @param handle 按钮对象指针
 * @param pin_level 获取按钮值的函数
 * @param active_level 按钮按下时的按钮值
 * @param button_id 按钮ID
 * @param DebounceC 消抖变换确立值判断时间值(周期数)，最大范围见定义
 * @param ShortT 短按判断时间值(ms)
 * @param button_id 长按判断时间值(ms)
 */
void MTButtonInit(MT_BUTTON *handle,
                  uint8_t (*pin_level)(uint8_t),
                  uint8_t  active_level,
                  uint8_t  button_id, /* 基础部分 */
                  uint8_t  DebounceC,
                  uint16_t ShortT,
                  uint16_t LongT /* 拓展部分 */)
{
    MT_BUTTON *target  = NULL;
    uint8_t    isFound = 0;
    for(target = head_handle; target; target = target->next)
    {
        if(target == handle)
            isFound = 1;
    }

    if(isFound == 0) // 并未存在链表中，进行memset内存初始化
        memset(handle, 0, sizeof(MT_BUTTON));

    handle->event               = (uint8_t)NONE_PRESS;
    handle->hal_button_Level    = pin_level;
    handle->button_level        = !active_level;
    handle->active_level        = active_level;
    handle->button_id           = button_id;

    handle->ConfMs.DebounceCnts = DebounceC;
    handle->ConfMs.ShortTicks   = ShortT;
    handle->ConfMs.LongTicks    = LongT;
}

/**
 * @brief 注册事件回调函数
 * @param handle 按钮对象指针
 * @param event 期望注册的事件
 * @param cb 注册成为的回调函数指针
 */
void MTButtonAttach(MT_BUTTON *handle, PressEvent event, BtnCallback cb)
{
    handle->cb[event] = cb;
}

/**
 * @brief 获得当前的按钮事件
 * @param handle 按钮对象指针
 * @return 按钮对象对应的当前事件
 */
PressEvent MTButtonEventGet(MT_BUTTON *handle)
{
    return (PressEvent)(handle->event);
}

/**
 * @brief 按钮驱动核心，驱动状态机
 * @param handle 按钮对象指针
 * @param cycle 调用本函数的周期值Ms
 */
static void MTButtonHandler(MT_BUTTON *handle, uint8_t cycle)
{
    uint8_t read_gpio_level = handle->hal_button_Level(handle->button_id);

    /* tick计数器进行 */
    if((handle->state) > 0)
        handle->ticks += cycle;

    /* 按钮变化计数器进行(是消抖的主要部分) */
    if(read_gpio_level != handle->button_level)
    {
        if(++(handle->debounce_cnt) >= handle->ConfMs.DebounceCnts)
        { /* 连续变化达阈值，切换按钮状态 */
            handle->button_level = read_gpio_level;
            handle->debounce_cnt = 0;
        }
    }
    else
    {
        handle->debounce_cnt = 0;
    }

    /* 状态机驱动进行 */
    switch(handle->state)
    {
    case 0:
        if(handle->button_level == handle->active_level)
        { /* 按键按下 */
            handle->event = (uint8_t)PRESS_DOWN;
            EVENT_CB(PRESS_DOWN);
            handle->ticks  = 0;
            handle->repeat = 1;
            handle->state  = 1;
        }
        else
        {
            handle->event = (uint8_t)NONE_PRESS;
        }
        break;

    case 1:
        if(handle->button_level != handle->active_level)
        { /* 按键释放 */
            handle->event = (uint8_t)PRESS_UP;
            EVENT_CB(PRESS_UP);
            handle->ticks = 0;
            handle->state = 2;
        }
        else if(handle->ticks > handle->ConfMs.LongTicks)
        { /* 长按达成事件触发 */
            handle->event = (uint8_t)LONG_PRESS_START;
            EVENT_CB(LONG_PRESS_START);
            handle->state = 5;
        }
        else if(handle->ticks == handle->ConfMs.ShortTicks)
        { /* 短按达成事件触发 */
            handle->event = (uint8_t)SHORT_PRESS_START;
            EVENT_CB(SHORT_PRESS_START);
        }

        break;

    case 2:
        if(handle->button_level == handle->active_level)
        { /* 按键按下 */
            handle->event = (uint8_t)PRESS_DOWN;
            EVENT_CB(PRESS_DOWN);
            if(handle->repeat != PRESS_REPEAT_MAX_NUM)
            {
                handle->repeat++;
            }
            handle->event = (uint8_t)PRESS_REPEAT;
            EVENT_CB(PRESS_REPEAT);
            handle->ticks = 0;
            handle->state = 3;
        }
        else if(handle->ticks > handle->ConfMs.ShortTicks)
        { /* 达短按阈值 */
            if(handle->repeat == 1)
            {
                handle->event = (uint8_t)SINGLE_CLICK;
                EVENT_CB(SINGLE_CLICK);
            }
            else if(handle->repeat == 2)
            {
                handle->event = (uint8_t)DOUBLE_CLICK;
                EVENT_CB(DOUBLE_CLICK); // repeat hit
            }

            handle->state = 0;
        }
        break;

    case 3:
        if(handle->button_level != handle->active_level)
        { /* 按键释放 */
            handle->event = (uint8_t)PRESS_UP;
            EVENT_CB(PRESS_UP);
            if(handle->ticks < handle->ConfMs.ShortTicks)
            {
                handle->ticks = 0;
                handle->state = 2; // 重复按，回到状态2
            }
            else
            {
                handle->state = 0;
            }
        }
        else if(handle->ticks > handle->ConfMs.ShortTicks)
        { // handle->ConfMs.ShortTicks < press down hold time < handle->ConfMs.LongTicks
            handle->state = 1;
        }
        break;

    case 5:
        if(handle->button_level == handle->active_level)
        { /* 按键按下 */
            handle->event = (uint8_t)LONG_PRESS_HOLD;
            EVENT_CB(LONG_PRESS_HOLD);
        }
        else
        {
            handle->event = (uint8_t)PRESS_UP;
            EVENT_CB(PRESS_UP);
            handle->event = (uint8_t)LONG_CLICK;
            EVENT_CB(LONG_CLICK);
            handle->state = 0; //reset
        }
        break;
    default:
        handle->state = 0; //reset
        break;
    }
}

/**
 * @brief 启动按钮工作，添加按钮对象指针到工作列表。
 * @param handle 按钮对象指针
 * @return 0: 成功操作. -1: 重复启动操作
 */
uint32_t MTButtonStart(MT_BUTTON *handle)
{
    MT_BUTTON *target;
    for(target = head_handle; target; target = target->next)
    {
        if(target == handle)
            return -1;
    }
    handle->next = head_handle;
    head_handle  = handle;
    return 0;
}

/**
 * @brief 停止按钮工作，删除按钮对象指针从工作列表。
 * @param handle 按钮对象指针
 */
void MTButtonStop(MT_BUTTON *handle)
{
    MT_BUTTON **curr;
    for(curr = &head_handle; *curr;)
    {
        MT_BUTTON *entry = *curr;
        if(entry == handle)
        {
            *curr = entry->next;
            return;
        }
        else
        {
            curr = &entry->next;
        }
    }
}

/**
 * @brief 必须周期调用，驱动按钮系统的关键函数
 * @param cycle 调用本函数的周期值Ms
 */
void MTButtonTicks(uint8_t cycle)
{
    MT_BUTTON *target;
    for(target = head_handle; target; target = target->next)
    {
        MTButtonHandler(target, cycle);
    }
}
