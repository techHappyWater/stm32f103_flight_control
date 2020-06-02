#ifndef _control_recode
#define _control_recode
#include "sys.h"

extern u16 RC_PIT;
extern u16 RC_ROL;
extern u16 RC_THROTTLE;
extern u16 RC_YAW;

extern u32 tempup1;	//捕获总高电平的时间
extern u32 tempup2;	//捕获总高电平的时间
extern u32 tempup3;	//捕获总高电平的时间
extern u32 tempup4;	//捕获总高电平的时间

void TIM4_Cap_Init(u16 arr, u16 psc);
void recode_test(void);

#endif
