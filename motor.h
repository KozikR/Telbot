/*********************************
ADC
*********************************/
#ifndef motor_h
#define motor_h

#include <avr/io.h>

#define LEFT 		2
#define RIGHT		1
#define STOP		0
#define	FORWARD		1
#define	BACK		2

void motor(unsigned char side, unsigned char state, unsigned char pwm);
/*Control motor state
side: LEFT, RIGHT
state: STOP, FORWARD, BACK
pwm: 0 - 255*/

void stop(void);
/*stop all motors*/

#endif //motor_h