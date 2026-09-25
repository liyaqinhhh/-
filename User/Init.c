#include "stm32f10x.h"
#include "system_stm32f10x.h"
#include "Encoder.h"
#include "Key.h"
#include "Motor.h"
#include "MPU6050.h"
#include "OLED.h"
#include "Serial.h"
#include "LED.h"
#include "Timer.h"

void Init_all(void)
{
	/*系统时钟必须先初始化，后续外设和延时均依赖系统时钟*/
	SystemInit();

	/*基础外设初始化*/
	Encoder_Init();
	Key_Init();
	Motor_Init();
	MPU6050_Init();
	imu_calibrate_gyro();
	OLED_Init();
	Serial_Init();

	/*工程现有主循环依赖的板载LED和时间片定时器*/
	LED_Init();
	Timer_Init();
}
