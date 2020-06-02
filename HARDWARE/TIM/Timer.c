#include "all.h"

//高级定时器1中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器1

void Timer1_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

	TIM_DeInit(TIM1); 

	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ClearFlag(TIM1, TIM_FLAG_Update); 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);

	TIM_Cmd(TIM1,ENABLE);

}

void TIM1_UP_IRQHandler(void)
{
	static u16 LED_count=0;
	static float gurox_modify=11;
  static float guroy_modify=-20;
  static float guroz_modify=16;
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) !=RESET )
	{
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
		
		mpu_dmp_get_data(&pitch,&roll,&yaw);      //角度值
		temp=MPU_Get_Temperature();	              //得到温度值
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据

		gyrox+=gurox_modify;gyrox=gyrox/16.384;   //+-2000
		gyroy+=guroy_modify;gyroy=gyroy/16.384;
		gyroz+=guroz_modify;gyroz=gyroz/16.384;
		
		//printf("%f,%f,%f\r\n",pitch,roll,yaw);
		//printf("%d,%d,%d\r\n",aacx,aacy,aacz);
		//printf("%d,%d,%d\r\n",gyrox,gyroy,gyroz);
		
		pitch=pitch;roll=roll;yaw=yaw;
		//usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
    Control(1);
		
		if(armed)       //解锁
		{
			LED_count++;
			if(LED_count==20)
			{
				LED_count=0;
				LED=~LED;
			}
		}
		else 
		{
		  LED_count=0;
			LED=1;
		}
		
	}
}
