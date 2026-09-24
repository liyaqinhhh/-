#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>

void Timer_Init(void);
void Time_Slice_Main(void);

void Task_1ms(void);
void Task_4ms(void);
void Task_8ms(void);
void Task_16ms(void);
void Task_40ms(void);

uint32_t Time_Slice_Get_4ms_Overrun_Count(void);
uint32_t Time_Slice_Get_8ms_Overrun_Count(void);
uint32_t Time_Slice_Get_40ms_Overrun_Count(void);

#endif
