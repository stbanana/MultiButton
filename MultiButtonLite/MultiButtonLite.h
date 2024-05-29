/*
 * Copyright (C), 2024, Yuanlong Xu <Yono233@outlook.com> 
 * All rights reserved
 */

/*
    小巧简单易用的事件驱动型按键驱动模块，可无限量扩展按键，采用事件回调驱动，其他机制在外部包装
    此为核心精简功能版本
*/
#ifndef _MULTI_BUTTON_H_
#define _MULTI_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

//According to your need to modify the constants.
#define DEBOUNCE_TICKS 3 //MAX 7 (0 ~ 7) 小于SHORT_TICKS/2，且不能过大，否则双击体验很差
#define SHORT_TICKS    (200)
#define LONG_TICKS     (1000)

/* Exported types ------------------------------------------------------------*/
typedef void (*BtnCallback)(void *);

/**
 * @brief 支持的事件表
 */
typedef enum
{
    SINGLE_CLICK,      // 单击按键事件
    LONG_CLICK,        // 长按击键事件
    SHORT_PRESS_START, // 达到短按时间阈值时触发一次
    LONG_PRESS_START,  // 达到长按时间阈值时触发一次
    LONG_PRESS_HOLD,   // 长按期间一直触发
    MUTLTIB_EVENT_MAX,
    NONE_PRESS
} PressEvent;

/**
 * @brief MultiButton库的对象结构体
 */
typedef struct MT_BUTTON
{
    uint16_t ticks;                                  // tick标准计数器
    uint8_t  repeat      :4;                         // 连击计数器
    uint8_t  event       :4;                         // 事件寄存器
    uint8_t  state       :3;                         // 驱动状态机寄存器
    uint8_t  debounce_cnt:3;                         // 消抖计数器(非Ms单位，以周期为单位)
    uint8_t  active_level:1;                         // 按下电平返回值绑定
    uint8_t  button_level:1;                         // 当前按钮确立值
    uint8_t  button_id;                              // 按钮ID号
    uint8_t (*hal_button_Level)(uint8_t button_id_); // 按钮电平获得函数，需要返回0或1
    BtnCallback       cb[MUTLTIB_EVENT_MAX];         // 事件回调组
    struct MT_BUTTON *next;
} MT_BUTTON;
/* Exported variables ---------------------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

    extern void       MTButtonInit(MT_BUTTON *handle, uint8_t (*pin_level)(uint8_t), uint8_t active_level, uint8_t button_id);
    extern void       MTButtonAttach(MT_BUTTON *handle, PressEvent event, BtnCallback cb);
    extern PressEvent MTButtonEventGet(MT_BUTTON *handle);
    extern int        MTButtonStart(MT_BUTTON *handle);
    extern void       MTButtonStop(MT_BUTTON *handle);
    extern void       MTButtonTicks(uint8_t cycle);

#ifdef __cplusplus
}
#endif

#endif
