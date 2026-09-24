# STM32F103 基础驱动工程

使用 Keil 5 打开 `Project.uvprojx`，目标芯片为 STM32F103C8。工程包含 GPIO、定时器、OLED、按键、串口、电机等基础驱动。

当前 `User/main.c` 是时间片轮询示例：TIM1 每 1 ms 触发一次中断，主循环调用 `Time_Slice_Main()`，依次执行 1/4/8/16/40 ms 任务。可在 `User/main.c` 的 `Task_1ms`、`Task_4ms`、`Task_8ms`、`Task_16ms`、`Task_40ms` 中加入业务代码。现有示例初始化 OLED，并在 4 ms 任务中每 500 ms 翻转一次 PC13 LED。

时间片调度逻辑位于 `System/Timer.c`，移植自 SEEKFREE TC387 Opensource Library（Copyright 2022 SEEKFREE），该部分按 GPL-3.0-or-later 授权。任务应避免长时间阻塞；4/8/40 ms 任务的超期次数可通过 `Time_Slice_Get_*_Overrun_Count()` 查看。
