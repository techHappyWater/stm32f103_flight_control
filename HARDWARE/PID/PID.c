#include "all.h"

// x:pitch    y:roll    z:yaw

//              y
//              |
//           1  |  2
//            \ | /
//             \ /
//              * ------x
//             / \
//            /   \
//           4     3

u8 armed=1;
u16 MOTO1_PWM,MOTO2_PWM,MOTO3_PWM,MOTO4_PWM;    //四电机pwm

pidsuite pid_out_Pitch;			//俯仰角外环pid
pidsuite pid_out_Roll;			//横滚角外环pid
pidsuite pid_in_Pitch;			//俯仰角内环pid
pidsuite pid_in_Roll;				//横滚角内环pid
pidsuite pid_in_Yaw;				//偏航角内环pid

float pid_out_pitch=0;
float pid_out_roll=0;
float pid_in_pitch=0;
float pid_in_roll=0;
float pid_in_yaw=0;

/*------------------------------------------pid结构初始化-----------------------------------------*/
//输入参数：结构体指针，期望值，kp,ki,kd
void pidInit(pidsuite* pid, const float desired, const float kp,
             const float ki, const float kd)
{

  pid->error = 0;
  pid->prevError = 0;
  pid->integ = 0;
  pid->deriv = 0;
  pid->desired = desired;
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  
  pid->iLimit = DEFAULT_PID_INTEGRATION_LIMIT;
}

/*----------------------------------------------pid积分部分限制值-------------------------------------------*/
void pidSetIntegralLimit(pidsuite* pid, const float limit)
{
  pid->iLimit = limit;
}

void PID_controllerInit(void)     //pid参数初始化
{
	pidInit(&pid_out_Pitch, 0, PID_OUT_PITCH_KP, PID_OUT_PITCH_KI, PID_OUT_PITCH_KD);
	pidInit(&pid_out_Roll, 0, PID_OUT_ROLL_KP, PID_OUT_ROLL_KI, PID_OUT_ROLL_KD);
	
	pidInit(&pid_in_Pitch, 0, PID_IN_PITCH_KP, PID_IN_PITCH_KI, PID_IN_PITCH_KD);
  pidInit(&pid_in_Roll, 0, PID_IN_ROLL_KP, PID_IN_ROLL_KI, PID_IN_ROLL_KD);
	pidInit(&pid_in_Yaw,0,PID_IN_YAW_KP,PID_IN_YAW_KI,PID_IN_YAW_KD);

	pidSetIntegralLimit(&pid_out_Pitch, PID_OUT_PITCH_INTEGRATION_LIMIT);
	pidSetIntegralLimit(&pid_out_Roll, PID_OUT_ROLL_INTEGRATION_LIMIT);
  pidSetIntegralLimit(&pid_in_Pitch, PID_IN_PITCH_INTEGRATION_LIMIT);
	pidSetIntegralLimit(&pid_in_Roll, PID_IN_ROLL_INTEGRATION_LIMIT);
	pidSetIntegralLimit(&pid_in_Yaw, PID_IN_YAW_INTEGRATION_LIMIT);
}

/*--------------------------------------pid输出更新------------------------------------------*/
//输入参数：pid结构体指针，测量值 ,期望值
//输出：pid输出
float pidUpdate(pidsuite* pid, const float measured,float expect)
{
  float output;

  pid->desired=expect;			 				//获取期望角度

  pid->error = pid->desired - measured;	 	  //偏差：期望-测量值
  
  pid->integ += pid->error;	  //偏差积分
	
  if (pid->integ > pid->iLimit)				  //积分限幅
  {
    pid->integ = pid->iLimit;
  }
  else if (pid->integ < -pid->iLimit)		//积分限幅
  {
    pid->integ = -pid->iLimit;
  }
  pid->deriv = pid->error - pid->prevError;		//微分

	pid->outP = pid->kp * pid->error;					  //方便独立观察
	pid->outI = pid->ki * pid->integ;
	pid->outD = pid->kd * pid->deriv;
	
	output = pid->outP +
					 pid->outI +
					 pid->outD;
  
  pid->prevError = pid->error;							 		//更新前一次偏差

  return output;
}

void Control(u8 armed)
{
	float pitch_expect=(float)((RC_PIT-1500)/10.0f);  //最大值1950左右
	float roll_expect=(float)((RC_ROL-1500)/10.0f);   //限制在45°
	float yaw_expect=(float)((RC_YAW-1500)/10.0f);
	
	if(RC_THROTTLE>YM_Dead)        //油门大于死区才进行控制
	{
		pid_out_pitch = pidUpdate(&pid_out_Pitch,pitch,pitch_expect);
		//pid_out_pitch=pitch_expect;
		pid_in_pitch=pidUpdate(&pid_in_Pitch,gyroy,pid_out_pitch);
		
		pid_out_roll = pidUpdate(&pid_out_Roll,roll,roll_expect);
		//pid_out_roll=roll_expect;
		pid_in_roll=pidUpdate(&pid_in_Roll,gyrox,pid_out_roll);
		
		pid_in_yaw=pidUpdate(&pid_in_Yaw,gyroz,yaw_expect);
	}
	else if(RC_THROTTLE<YM_Dead)   //否则积分清零
	{
		pid_in_Pitch.integ=0;
		pid_in_Roll.integ=0;
		pid_in_Yaw.integ=0;
	}
	
	//动力分配
	MOTO1_PWM=RC_THROTTLE-pid_in_pitch+pid_in_roll+pid_in_yaw;
	MOTO2_PWM=RC_THROTTLE-pid_in_pitch-pid_in_roll-pid_in_yaw;
	MOTO3_PWM=RC_THROTTLE+pid_in_pitch-pid_in_roll+pid_in_yaw;
	MOTO4_PWM=RC_THROTTLE+pid_in_pitch+pid_in_roll-pid_in_yaw;
	
	if(RC_THROTTLE>YM_Dead)
	{
		//限幅
		if(MOTO1_PWM<YM_Dead)MOTO1_PWM=YM_Dead;else if(MOTO1_PWM>2000)MOTO1_PWM=2000;
		if(MOTO2_PWM<YM_Dead)MOTO2_PWM=YM_Dead;else if(MOTO2_PWM>2000)MOTO2_PWM=2000;
		if(MOTO3_PWM<YM_Dead)MOTO3_PWM=YM_Dead;else if(MOTO3_PWM>2000)MOTO3_PWM=2000;
		if(MOTO4_PWM<YM_Dead)MOTO4_PWM=YM_Dead;else if(MOTO4_PWM>2000)MOTO4_PWM=2000;
	}
	else 
	{
	  MOTO1_PWM=1000,MOTO2_PWM=1000,MOTO3_PWM=1000,MOTO4_PWM=1000;
	}
	
	Moto_Pwm(MOTO1_PWM,MOTO2_PWM,MOTO3_PWM,MOTO4_PWM);
	
}



