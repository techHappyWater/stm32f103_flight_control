#ifndef _uart_report
#define _uart_report
#include "sys.h"

void usart1_niming_report(u8 fun,u8*data,u8 len);
void mpu6050_send_data(float pitch,float roll,float yaw);
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw);
#endif
