#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "MPU6050.h"
#include "Motor.h"
#include "Encoder.h"
#include "Serial.h"
#include "BlueSerial.h"
#include "Init.h"
#include "PID.h"


int16_t KeyNum = 0;	
/* 时间片轮询示例：OLED 显示内容，PC13 LED 每 500 ms 翻转一次 */
int main(void)
{
	/*模块初始化*/
	Init_all();

	while (1)
	{
		//OLED_show();
	}
}

//不使用1ms
void Task_1ms(void)
{
}

void Task_4ms(void)
{
	Update_Angle_Z();
	PID_GetSpeed();
}

void Task_8ms(void)
{
	KeyNum = Key_GetNum();		//获取键码
	if (KeyNum == 1)			//如果K1按下
	{
		RunFlag = 1;			
	}

}

void Task_16ms(void)
{

}

void Task_40ms(void)
{
}

/*定时中断和非阻塞式按键测试*/
/*下载此段程序后，OLED显示变量Count会1ms自增一次*/
/*按下K1按键，OLED显示变量Num加1，按下K2，Num减1，按下K3，Num加10，按下K4，Num减10*/
//uint16_t Count;
//uint8_t KeyNum, Num;

//int main(void)
//{
//	/*模块初始化*/
//	OLED_Init();		//OLED初始化
//	Key_Init();			//非阻塞式按键初始化
//	
//	Timer_Init();		//定时器初始化，1ms定时中断一次
//	
//	while (1)
//	{
//		KeyNum = Key_GetNum();		//获取键码
//		if (KeyNum == 1)			//如果K1按下
//		{
//			Num ++;					//变量Num加1
//		}
//		if (KeyNum == 2)			//如果K2按下
//		{
//			Num --;					//变量Num减1
//		}
//		if (KeyNum == 3)			//如果K3按下
//		{
//			Num += 10;				//变量Num加10
//		}
//		if (KeyNum == 4)			//如果K4按下
//		{
//			Num -= 10;				//变量Num减10
//		}
//		
//		/*OLED显示*/
//		OLED_Printf(0, 0, OLED_8X16, "Count:%05d", Count);	//显示变量Count
//		OLED_Printf(0, 16, OLED_8X16, "Num:%03d", Num);		//显示变量Num
//
//		/*OLED更新*/
//		OLED_Update();
//	}
//}

//void TIM1_UP_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
//	{
//		/*定时中断函数1ms自动执行一次*/
//
//		Count ++;			//测试变量Count自增
//		
//		Key_Tick();			//调用按键模块的Tick函数，用于驱动按键模块工作
//		
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	}
//}

/*MPU6050测试*/
/*下载此段程序后，OLED会显示MPU6050的各项数据*/
//int16_t AX, AY, AZ, GX, GY, GZ;
//uint8_t TimerErrorFlag;
//uint16_t TimerCount;

//int main(void)
//{
//	/*模块初始化*/
//	OLED_Init();		//OLED初始化
//	MPU6050_Init();		//MPU6050初始化
//	
//	Timer_Init();		//定时器初始化，1ms定时中断一次
//	
//	while (1)
//	{
//		/*OLED显示*/
//		OLED_Printf(0, 0, OLED_8X16, "%+06d", AX);		//显示AX
//		OLED_Printf(0, 16, OLED_8X16, "%+06d", AY);		//显示AY
//		OLED_Printf(0, 32, OLED_8X16, "%+06d", AZ);		//显示AZ
//		OLED_Printf(64, 0, OLED_8X16, "%+06d", GX);		//显示GX
//		OLED_Printf(64, 16, OLED_8X16, "%+06d", GY);	//显示GY
//		OLED_Printf(64, 32, OLED_8X16, "%+06d", GZ);	//显示GZ
//		OLED_Printf(0, 48, OLED_8X16, "Flag:%1d", TimerErrorFlag);	//显示TimerErrorFlag
//		OLED_Printf(64, 48, OLED_8X16, "C:%05d", TimerCount);		//显示TimerCount
//		
//		/*OLED更新*/
//		OLED_Update();
//	}
//}

//void TIM1_UP_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
//	{
//		/*定时中断函数1ms自动执行一次*/
//
//		/*进入中断函数后，立刻清标志位*/
//		/*如果中断函数退出前，标志位又置1了，说明中断函数执行时间超过了定时时间（1ms）*/
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//		
//		/*在中断里读取MPU6050，可以保证读取间隔严格为1ms*/
//		/*但要保证MPU6050_GetData执行时间不超过1ms*/
//		MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
//		
//		/*中断函数退出前，再次检查标志位*/
//		if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
//		{
//			/*标志位又置1了，说明中断函数执行时间超过了定时时间（1ms）*/
//			/*置TimerErrorFlag为1，表示定时中断错误*/
//			TimerErrorFlag = 1;
//
//			/*清标志位，避免中断连续触发，导致主函数完全无法执行*/
//			TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//		}
//
//		/*中断函数退出前，读取计数器的值，此值可用于测量中断函数的具体执行时间*/
//		TimerCount = TIM_GetCounter(TIM1);
//	}
//}

/*直流电机和编码器测试*/
/*下载此段程序后，按下K1，左电机速度增加，按下K2，左电机速度减小*/
/*按下K3，右电机速度增加，按下K4，右电机速度减小*/
/*OLED会显示左右电机的PWM值和编码器测量得到的实际速度值*/
//uint8_t KeyNum;
//int8_t PWML, PWMR;
//float SpeedL, SpeedR;

//int main(void)
//{
//	/*模块初始化*/
//	OLED_Init();		//OLED初始化
//	Key_Init();			//非阻塞式按键初始化
//	Motor_Init();		//电机初始化
//	Encoder_Init();		//编码器初始化
//	
//	Timer_Init();		//定时器初始化，1ms定时中断一次
//	
//	while (1)
//	{
//		KeyNum = Key_GetNum();		//获取键码
//		if (KeyNum == 1)			//如果K1按下
//		{
//			PWML += 10;				//左电机PWM加10
//		}
//		if (KeyNum == 2)			//如果K2按下
//		{
//			PWML -= 10;				//左电机PWM减10
//		}
//		if (KeyNum == 3)			//如果K3按下
//		{
//			PWMR += 10;				//右电机PWM加10
//		}
//		if (KeyNum == 4)			//如果K4按下
//		{
//			PWMR -= 10;				//右电机PWM减10
//		}
//		
//		/*电机设定PWM，将PWML和PWMR变量的值给电机*/
//		Motor_SetPWM(1, PWML);		//1表示左电机，设定为PWML
//		Motor_SetPWM(2, PWMR);		//2表示右电机，设定为PWMR
//		
//		/*OLED显示*/
//		OLED_Printf(0, 0, OLED_8X16, "PWML:%+04d", PWML);		//显示左轮的PWM
//		OLED_Printf(0, 16, OLED_8X16, "PWMR:%+04d", PWMR);		//显示右轮的PWM
//		OLED_Printf(0, 32, OLED_8X16, "SpdL:%+06.2f", SpeedL);	//显示左轮的速度
//		OLED_Printf(0, 48, OLED_8X16, "SpdR:%+06.2f", SpeedR);	//显示右轮的速度
//		
//		/*OLED更新*/
//		OLED_Update();
//	}
//}

//void TIM1_UP_IRQHandler(void)
//{
//	static uint16_t Count;
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
//	{
//		/*定时中断函数1ms自动执行一次*/
//
//		Key_Tick();				//调用按键模块的Tick函数，用于驱动按键模块工作
//		
//		Count ++;				//使用Count变量计次进行分频
//		if (Count >= 50)		//计次到达50后，进入一次if，即50ms进一次if
//		{
//			Count = 0;			//Count清零，以便下一个周期的计数
//			
//			/*获取编码器的计次值增量，并计算电机旋转速度*/
//			/*Encoder_Get函数，可以获取两次读取编码器的计次值增量*/
//			/*编码器磁铁旋转一圈，计次增量为44，编码器读取间隔是50ms（0.05s）*/
//			/*因此磁铁旋转速度 = 计次增量 / 44 / 0.05，单位是转每秒*/
//			/*平衡车使用的电机带有减速箱，减速比为9.27666*/
//			/*因此电机输出轴旋转速度 = 磁铁旋转速度 / 9.27666，单位是转每秒*/
//			SpeedL = Encoder_Get(1) / 44.0 / 0.05 / 9.27666;		//左轮旋转速度
//			SpeedR = Encoder_Get(2) / 44.0 / 0.05 / 9.27666;		//右轮旋转速度
//		}
//		
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	}
//}

/*串口和蓝牙串口测试*/
/*下载此段程序后，复位瞬间，串口和蓝牙串口会发送HelloWorld字符串*/
/*随后，串口收到一个字节后，会在OLED上显示这个字节数据*/
/*蓝牙串口收到一个方括号包裹的文本数据包后，会在OLED上显示这个数据包内容*/
//int main(void)
//{
//	/*模块初始化*/
//	OLED_Init();		//OLED初始化
//	Serial_Init();		//串口初始化，波特率9600
//	BlueSerial_Init();	//蓝牙串口初始化，波特率9600
//	
//	/*串口发送HelloWorld字符串*/
//	Serial_SendString("Hello");
//	Serial_Printf("World");
//	
//	/*蓝牙串口发送HelloWorld字符串*/
//	BlueSerial_SendString("hello");
//	BlueSerial_Printf("world");
//	
//	while (1)
//	{
//		if (Serial_GetRxFlag() == 1)				//串口收到一个字节
//		{
//			uint8_t RxData = Serial_GetRxData();	//获取这个字节数据
//			
//			OLED_Printf(0, 0, OLED_8X16, "RxData:%02X", RxData);	//OLED上显示这个字节数据
//			OLED_Update();
//		}
//		
//		if (BlueSerial_RxFlag == 1)					//蓝牙串口收到一个方括号包裹的文本数据包
//		{
//			OLED_Printf(0, 16, OLED_8X16, "%s", BlueSerial_RxPacket);	//OLED上显示这个数据包内容
//			OLED_Update();
//			
//			BlueSerial_RxFlag = 0;					//处理完成后，标志位置0，允许接收下一个数据包
//		}
//	}
//}
