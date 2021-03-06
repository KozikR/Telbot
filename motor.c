#include "motor.h"
#include "board_def.h"

void motor(unsigned char side,unsigned char state, unsigned char pwm)
{
	if(side == LEFT)
	{
		OCR0B = pwm;
		switch(state)
		{
			case STOP:
				M_LEFT_PORT &= ~(M_LEFT_1 | M_LEFT_2);
				break;
			case BACK:
				M_LEFT_PORT &= ~M_LEFT_1;
				M_LEFT_PORT |= M_LEFT_2;
				break;
			case FORWARD:
				M_LEFT_PORT |= M_LEFT_1;
				M_LEFT_PORT &= ~M_LEFT_2;
				break;
			default:
				break;
		}
	}
	else if(side == RIGHT)
	{
		OCR0A = pwm;
		switch(state)
		{
			case STOP:
				M_RIGHT_PORT_1 &= ~M_RIGHT_1;
				M_RIGHT_PORT_2 &= ~M_RIGHT_2;
				break;
			case FORWARD:
				M_RIGHT_PORT_1 &= ~M_RIGHT_1;
				M_RIGHT_PORT_2 |= M_RIGHT_2;
				break;
			case BACK:
				M_RIGHT_PORT_1 |= M_RIGHT_1;
				M_RIGHT_PORT_2 &= ~M_RIGHT_2;
				break;
			default:
				break;
		}
	
	}
}
void motors(unsigned char state_right, unsigned char state_left)
{
	switch(state_left)
	{
		case STOP:
			M_LEFT_PORT &= ~(M_LEFT_1 | M_LEFT_2);
			break;
		case BACK:
			M_LEFT_PORT &= ~M_LEFT_1;
			M_LEFT_PORT |= M_LEFT_2;
			break;
		case FORWARD:
			M_LEFT_PORT |= M_LEFT_1;
			M_LEFT_PORT &= ~M_LEFT_2;
			break;
		default:
			break;
	}
	switch(state_right)
	{
		case STOP:
			M_RIGHT_PORT_1 &= ~M_RIGHT_1;
			M_RIGHT_PORT_2 &= ~M_RIGHT_2;
			break;
		case FORWARD:
			M_RIGHT_PORT_1 &= ~M_RIGHT_1;
			M_RIGHT_PORT_2 |= M_RIGHT_2;
			break;
		case BACK:
			M_RIGHT_PORT_1 |= M_RIGHT_1;
			M_RIGHT_PORT_2 &= ~M_RIGHT_2;
			break;
		default:
			break;
	}
}

void stop(void)
{
	M_LEFT_PORT &= ~(M_LEFT_1 | M_LEFT_2);
	M_RIGHT_PORT_1 &= ~M_RIGHT_1;
	M_RIGHT_PORT_2 &= ~M_RIGHT_2;
}
