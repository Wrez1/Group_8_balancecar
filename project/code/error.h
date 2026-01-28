#ifndef __ERROR_H
#define	__ERROR_H

#include "sensor.h"	   
/*-------------数字端口----------------*/
int error_calcaulate(void);
extern int err;
int get_fused_error(int sensor_err, float gyro_z);
float Right_err(void);

#endif 
