#include "stm32f10x.h"                  // Device header
#include "PID.h"

PID_t SpeedPID = {					//角度环PID结构体变量，定义的时候同时给部分成员赋初值

	.Target = 0,						//目标值，由用户设定

	.Kp = 2,						//比例项权重
	.Ki = 0.05,						//积分项权重
	.Kd = 0,						//微分项权重
	
	.OutMax = 20,					//输出限幅的最大值
	.OutMin = -20,					//输出限幅的最小值
	
	.ErrorIntMax = 150,				//误差积分的最大值
	.ErrorIntMin = -150,			//误差积分的最小值
};

PID_t TurnPID = {					//转向环PID结构体变量，定义的时候同时给部分成员赋初值
	.Kp = 4,						//比例项权重
	.Ki = 3,						//积分项权重
	.Kd = 0,						//微分项权重
	
	.OutMax = 50,					//输出限幅的最大值
	.OutMin = -50,					//输出限幅的最小值
	
	.ErrorIntMax = 20,				//误差积分的最大值
	.ErrorIntMin = -20,				//误差积分的最小值
};
/**
  * 函    数：PID初始化
  * 参    数：p 指定结构体的地址
  * 返 回 值：无
  */
void PID_Init(PID_t *p)
{
	/*把PID表示状态的参数清零，避免之前遗留的参数对本次启动造成影响*/
	p->Target = 0;
	p->Actual = 0;
	p->Actual1 = 0;
	p->Out = 0;
	p->Error0 = 0;
	p->Error1 = 0;
	p->ErrorInt = 0;
}

/**
  * 函    数：PID计算及结构体变量值更新
  * 参    数：p 指定结构体的地址
  * 返 回 值：无
  */
void PID_Update(PID_t *p)
{
	/*获取本次误差和上次误差*/
	p->Error1 = p->Error0;					//获取上次误差
	p->Error0 = p->Target - p->Actual;		//获取本次误差，目标值减实际值，即为误差值
	
	/*外环误差积分（累加）*/
	/*如果Ki不为0，才进行误差积分，这样做的目的是便于调试*/
	/*因为在调试时，我们可能先把Ki设置为0，这时积分项无作用，误差消除不了，误差积分会积累到很大的值*/
	/*后续一旦Ki不为0，那么因为误差积分已经积累到很大的值了，这就导致积分项疯狂输出，不利于调试*/
	if (p->Ki != 0)				//如果Ki不为0
	{
		p->ErrorInt += p->Error0;	//进行误差积分
		
		/*误差积分限幅*/
		if (p->ErrorInt > p->ErrorIntMax) {p->ErrorInt = p->ErrorIntMax;}	//限制误差积分最大为结构体指定的ErrorIntMax
		if (p->ErrorInt < p->ErrorIntMin) {p->ErrorInt = p->ErrorIntMin;}	//限制误差积分最小为结构体指定的ErrorIntMin
	}
	else							//否则
	{
		p->ErrorInt = 0;			//误差积分直接归0
	}
	
	/*PID计算*/
	/*使用位置式PID公式，计算得到输出值*/
	p->Out = p->Kp * p->Error0
		   + p->Ki * p->ErrorInt
//		   + p->Kd * (p->Error0 - p->Error1);		//普通PID计算公式
		   - p->Kd * (p->Actual - p->Actual1);		//微分先行计算公式
	
	/*输出偏移*/
	if (p->Out > 0) {p->Out += p->OutOffset;}		//如果输出值为正，则加上结构体指定的OutOffset
	if (p->Out < 0) {p->Out -= p->OutOffset;}		//如果输出值为负，则减去结构体指定的OutOffset
	
	/*输出限幅*/
	if (p->Out > p->OutMax) {p->Out = p->OutMax;}	//限制输出值最大为结构体指定的OutMax
	if (p->Out < p->OutMin) {p->Out = p->OutMin;}	//限制输出值最小为结构体指定的OutMin
	
	/*获取上次实际值*/
	p->Actual1 = p->Actual;		//本轮计算后进行变量传递，下轮计算时Actual1即为上次实际值
}

/*获取编码器的计次值增量，并计算电机旋转速度*/
/*Encoder_Get函数，可以获取两次读取编码器的计次值增量*/
/*编码器磁铁旋转一圈，计次增量为44，编码器读取间隔是50ms（0.05s）*/
/*因此磁铁旋转速度 = 计次增量 / 44 / 0.05，单位是转每秒*/
/*平衡车使用的电机带有减速箱，减速比为9.27666*/
/*因此电机输出轴旋转速度 = 磁铁旋转速度 / 9.27666，单位是转每秒*/
int16_t RunFlag;						//目标值
int16_t LeftPWM, RightPWM;			//左PWM，右PWM
int16_t AvePWM, DifPWM;				//平均PWM，差分PWM

float LeftSpeed, RightSpeed;		//左速度，右速度
float AveSpeed, DifSpeed;			//平均速度，差分速度
void PID_GetSpeed(void)
{
	LeftSpeed = Encoder_Get(1) / 44.0 / 0.05 / 9.27666;
	RightSpeed = Encoder_Get(2) / 44.0 / 0.05 / 9.27666;

	/*信号量转换*/
	AveSpeed = (LeftSpeed + RightSpeed) / 2.0;	//由左轮速度和右轮速度计算得到平均速度
	DifSpeed = LeftSpeed - RightSpeed;			//由左轮速度和右轮速度计算得到差分速度

	/*执行PID调控程序*/
	if (RunFlag)					//RunFlag非0时，启动PID程序
	{
		
		/*速度环PID控制*/
		SpeedPID.Actual = AveSpeed;			//速度环实际值为AveSpeed
		PID_Update(&SpeedPID);				//调用封装好的函数，一步完成PID计算和更新
		AvePWM = SpeedPID.Out;					//速度环的输出值给到电机平均PWM，用于控制前进和后退
		/*转向环PID控制*/
		TurnPID.Actual = DifSpeed;			//转向环实际值为DifSpeed
		PID_Update(&TurnPID);				//调用封装好的函数，一步完成PID计算和更新
		DifPWM = TurnPID.Out;				//转向环的输出值给到电机差分PWM，用于控制左右转弯

		/*控制量转换*/
		LeftPWM = AvePWM + DifPWM / 2;		//由平均PWM和差分PWM计算得到左轮PWM
		RightPWM = AvePWM - DifPWM / 2;		//由平均PWM和差分PWM计算得到右轮PWM
		
		/*PWM限幅*/
		/*上式计算后，LeftPWM和RightPWM可能会超出电机允许的PWM范围，此处将PWM值范围限制在-100~100之内*/
		if (LeftPWM > 100) {LeftPWM = 100;} else if (LeftPWM < -100) {LeftPWM = -100;}
		if (RightPWM > 100) {RightPWM = 100;} else if (RightPWM < -100) {RightPWM = -100;}
		
		/*PWM输出给电机*/
		Motor_SetPWM(1, LeftPWM);		//LeftPWM输出给左轮电机
		Motor_SetPWM(2, RightPWM);		//RightPWM输出给右轮电机

	}
	else		//RunFlag为0时，停止PID程序
	{
		/*左右电机PWM均设置为0*/
		Motor_SetPWM(1, 0);
		Motor_SetPWM(2, 0);
	}


}