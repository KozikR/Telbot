/*********************************
Board definitions
*********************************/

#ifndef board_def_h
#define board_def_h

#include <avr/io.h>
/*LED*/

#define LED_PORT 		PORTC
#define LED_DDR			DDRC
#define LED_GREEN		0x10
#define	LED_RED			0x04
#define	LED_BLUE		0x08

/*Motors*/
#define M_LEFT_PORT 	PORTB
#define M_LEFT_DDR		DDRB
#define M_LEFT_1		0x40
#define M_LEFT_2		0x80

#define M_RIGHT_PORT_1	PORTB
#define M_RIGHT_DDR_1	DDRB
#define M_RIGHT_PORT_2	PORTD
#define M_RIGHT_DDR_2	DDRD
#define M_RIGHT_1		0x01
#define M_RIGHT_2		0x80

#define M_PWM_DDR		DDRD
#define M_PWM_PORT		PORTD
#define	M_PWM_A			0x20	/*OC0B*/
#define	M_PWM_B			0x40	/*OC0A*/		

/*ADC*/
#define	POWER			0
#define	GROUND_FRONT	5
#define	GROUND_BACK		1

#endif //board_def_h