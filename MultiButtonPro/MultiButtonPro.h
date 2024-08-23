/*
 * Copyright (C), 2024, Yuanlong Xu <Yono233@outlook.com> 
 * All rights reserved
 */

/*
    小巧简单易用的事件驱动型按键驱动模块，可无限量扩展按键，采用事件回调驱动，其他机制在外部包装
    此为全功能版本
*/
#ifndef _MULTI_BUTTON_PRO_H_
#define _MULTI_BUTTON_PRO_H_
#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef void (*BtnCallback)(void *);

/**
 * @brief 支持的事件表
 */
typedef enum
{
    PRESS_DOWN = 0,    // 按键按下，每次按下都触发
    PRESS_UP,          // 按键弹起，每次松开都触发
    PRESS_REPEAT,      // 重复按下触发，变量repeat计数连击次数
    SINGLE_CLICK,      // 单击按键事件
    DOUBLE_CLICK,      // 双击按键事件
    LONG_CLICK,        // 长按击键事件
    SHORT_PRESS_START, // 达到短按时间阈值时触发一次
    LONG_PRESS_START,  // 达到长按时间阈值时触发一次
    LONG_PRESS_HOLD,   // 长按期间一直触发
    MUTLTIB_EVENT_MAX,
    NONE_PRESS
} PressEvent;

/**
 * @brief 各种按钮参数的时间配置值
 */
typedef struct
{
    uint8_t  DebounceCnts; // (周期数) 消抖稳定周期值 依据debounce_cnt位确立最大值，目前是7
    uint16_t ShortTicks;   // (Ms) 短按判定阈值
    uint16_t LongTicks;    // (Ms) 长按判定阈值
} MT_BUTTON_CONF;

/**
 * @brief MultiButton库的对象结构体
 */
typedef struct MT_BUTTON
{
    MT_BUTTON_CONF ConfMs;
    uint16_t       ticks;                            // tick标准计数器
    uint8_t        repeat      :4;                   // 连击计数器
    uint8_t        event       :4;                   // 事件寄存器
    uint8_t        state       :3;                   // 驱动状态机寄存器
    uint8_t        debounce_cnt:3;                   // 消抖计数器(非Ms单位，以周期为单位)
    uint8_t        active_level:1;                   // 按下电平返回值绑定
    uint8_t        button_level:1;                   // 当前按钮确立值
    uint8_t        button_id;                        // 按钮ID号
    uint8_t (*hal_button_Level)(uint8_t button_id_); // 按钮电平获得函数，需要返回0或1
    BtnCallback       cb[MUTLTIB_EVENT_MAX];         // 事件回调组
    struct MT_BUTTON *next;
} MT_BUTTON;
/* Exported variables ---------------------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/

extern void       MTButtonInit(MT_BUTTON *handle,
                               uint8_t (*pin_level)(uint8_t),
                               uint8_t  active_level,
                               uint8_t  button_id, /* 基础部分 */
                               uint8_t  DebounceC,
                               uint16_t ShortT,
                               uint16_t LongT /* 拓展部分 */);
extern void       MTButtonAttach(MT_BUTTON *handle, PressEvent event, BtnCallback cb);
extern PressEvent MTButtonEventGet(MT_BUTTON *handle);
extern uint32_t   MTButtonStart(MT_BUTTON *handle);
extern void       MTButtonStop(MT_BUTTON *handle);
extern void       MTButtonTicks(uint8_t cycle);

#ifdef __cplusplus
}
#endif

#endif
