#include "all.h"

int main(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(500000);	 	          //串口初始化为500000
	delay_init();	                  //延时初始化 
	LED_Init();		  			          //初始化与LED连接的硬件接口
	TIM4_Cap_Init(0xffff,72-1);     //以1Mhz的频率计数  遥控器
	TIM2_PWM_Init(20000,71);        //144分频，72M/72/20000=50hz
	
//	Moto_Pwm(1880,1880,1880,1880);
//	delay_ms(1000);delay_ms(1000);delay_ms(1000);delay_ms(1000);delay_ms(1000);
	Moto_Pwm(1000,1000,1000,1000);
	LED=0;
	delay_ms(1000);delay_ms(1000);delay_ms(1000);
	while(RC_THROTTLE>YM_Dead-20);                  //等待油门回到安全值
	LED=1;
	
	PID_controllerInit();  //pid参数初始化
	
	MPU_Init();					   //初始化MPU6050

	while(mpu_dmp_init())
	{
	delay_ms(20);
	}
	Timer1_Init(9999,71);         //72分频，周期为1M/10000=100HZ -->10ms
	
	while(1)
	{
		delay_ms(10);
		//Moto_Pwm(1110,1110,1110,1110);
		//recode_test();
	}
}
 


