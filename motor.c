#include "motor.h"
#include "board_def.h"

void motor(char side, char state, char pwm)
{
	if(side == LEFT)
	{
		OCR0A = pwm;
		switch(state)
		{
			case STOP:
				M_LEFT_PORT &= ~(M_LEFT_1 | M_LEFT_2);
				break;
			case FORWARD:
				M_LEFT_PORT &= ~M_LEFT_1;
				M_LEFT_PORT |= M_LEFT_2;
				break;
			case BACK:
				M_LEFT_PORT |= M_LEFT_1;
				M_LEFT_PORT &= ~M_LEFT_2;
				break;
			default:
				break;
		}
	}
	else if(side == RIGHT)
	{
		OCR0B = pwm;
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

void stop(void)
{
	OCR0A = 0;
	OCR0B = 0;
	M_LEFT_PORT &= ~(M_LEFT_1 | M_LEFT_2);
	M_RIGHT_PORT_1 &= ~M_RIGHT_1;
	M_RIGHT_PORT_2 &= ~M_RIGHT_2;
}
