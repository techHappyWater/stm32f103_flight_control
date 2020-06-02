#ifndef _motor_
#define _motor_
#include "sys.h"

void TIM2_PWM_Init(u16 arr,u16 psc);
void Moto_Pwm(u16 MOTO1_PWM,u16 MOTO2_PWM,u16 MOTO3_PWM,u16 MOTO4_PWM);

#endif
