#include "stm32f10x.h"                  // Device header
#include "Timer.h"

/* SPDX-License-Identifier: GPL-3.0-or-later
 * Time-slice logic ported from SEEKFREE TC387 Opensource Library,
 * Copyright (C) 2022 SEEKFREE.
 */
#define TIME_SLICE_TASK_1MS     0x01u
#define TIME_SLICE_TASK_16MS    0x02u
#define TIME_SLICE_PENDING_MAX  500u

static volatile uint8_t time_slice_flags;
static volatile uint16_t time_slice_pending_4ms;
static volatile uint16_t time_slice_pending_8ms;
static volatile uint16_t time_slice_pending_40ms;

static uint32_t time_slice_overrun_4ms_count;
static uint32_t time_slice_overrun_8ms_count;
static uint32_t time_slice_overrun_40ms_count;

static void Time_Slice_Tick(void);

/**
  * 函    数：定时中断初始化
  * 参    数：无
  * 返 回 值：无
  */
void Timer_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);			//开启TIM1的时钟
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM1);		//选择TIM1为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;                //计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;               //预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器会用到，此处配置为0
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);             //将结构体变量交给TIM_TimeBaseInit，配置TIM1的时基单元
	
	/*中断输出配置*/
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);			//清除定时器更新标志位
	                                                //TIM_TimeBaseInit函数末尾，手动产生了更新事件
	                                                //若不清除此标志位，则开启中断后，会立刻进入一次中断
	                                                //如果不介意此问题，则不清除此标志位也可
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);		//开启TIM1的更新中断
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//配置NVIC为分组2
	                                                //即抢占优先级范围：0~3，响应优先级范围：0~3
	                                                //此分组配置在整个工程中仅需调用一次
	                                                //若有多个中断，可以把此代码放在main函数内，while循环之前
	                                                //若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;          //选择配置NVIC的TIM1_UP线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;   //指定NVIC线路的抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          //指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);                             //将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*TIM使能*/
	TIM_Cmd(TIM1, ENABLE);			//使能TIM1，定时器开始运行
}

void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		Time_Slice_Tick();
	}
}

static void Time_Slice_Tick(void)
{
	static uint8_t count_4ms;
	static uint8_t count_8ms;
	static uint8_t count_16ms;
	static uint8_t count_40ms;

	time_slice_flags |= TIME_SLICE_TASK_1MS;

	if (++count_4ms >= 4u)
	{
		count_4ms = 0;
		Task_4ms();
	}
	if (++count_8ms >= 8u)
	{
		count_8ms = 0;
		Task_8ms();
	}
	if (++count_16ms >= 16u)
	{
		count_16ms = 0;
		Task_16ms();
	}
	if (++count_40ms >= 40u)
	{
		count_40ms = 0;
		Task_40ms();
	}
}
//弃用
void Time_Slice_Main(void)
{
	uint8_t task_flags;
	uint16_t pending_4ms;
	uint16_t pending_8ms;
	uint16_t pending_40ms;
	uint32_t interrupt_state = __get_PRIMASK();

	__disable_irq();
	task_flags = time_slice_flags;
	time_slice_flags = 0;
	pending_4ms = time_slice_pending_4ms;
	pending_8ms = time_slice_pending_8ms;
	pending_40ms = time_slice_pending_40ms;
	time_slice_pending_4ms = 0;
	time_slice_pending_8ms = 0;
	time_slice_pending_40ms = 0;
	__set_PRIMASK(interrupt_state);

	if (task_flags & TIME_SLICE_TASK_1MS)
	{
		//Task_1ms();
	}
	if (pending_4ms)
	{
		if (pending_4ms > 1u)
		{
			//time_slice_overrun_4ms_count += (uint32_t)(pending_4ms - 1u);
		}
		Task_4ms();
	}
	if (pending_8ms)
	{
		if (pending_8ms > 1u)
		{
			//time_slice_overrun_8ms_count += (uint32_t)(pending_8ms - 1u);
		}
		Task_8ms();
	}
	if (task_flags & TIME_SLICE_TASK_16MS)
	{
		Task_16ms();
	}
	if (pending_40ms)
	{
		if (pending_40ms > 1u)
		{
			//time_slice_overrun_40ms_count += (uint32_t)(pending_40ms - 1u);
		}
		Task_40ms();
	}
}

uint32_t Time_Slice_Get_4ms_Overrun_Count(void)
{
	return time_slice_overrun_4ms_count;
}

uint32_t Time_Slice_Get_8ms_Overrun_Count(void)
{
	return time_slice_overrun_8ms_count;
}

uint32_t Time_Slice_Get_40ms_Overrun_Count(void)
{
	return time_slice_overrun_40ms_count;
}
