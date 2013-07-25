/******************************
Telbot
main program
Rafa³ Kozik
******************************/
/*
FUSES: 
	low = 0xE2,
	high = HFUSE_DEFAULT,
	extended = EFUSE_DEFAULT,
*/

/* Includes */
#include <inttypes.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>

#include "board_def.h"
#include "adc.h"
#include "uart.h"
#include "motor.h"

/*Macros*/
#define TRUE 1
#define	FALSE 0

/*Global variables*/
volatile unsigned char command[10];
volatile unsigned char new_command = FALSE;
volatile unsigned char current_byte = 0;

/*Functions*/
void send_sensor_data(void)
{
	send_string("FRONT:");
	send_number(get_ADC(GROUND_FRONT));
	send_string("BACK:");
	send_number(get_ADC(GROUND_BACK));
	send_string("POWER:");
	send_number(get_ADC(POWER));
}

/*Interrupts*/
ISR(USART_RX_vect)
{
/*Get data fram
!up to 9 byte#
Command list:
Led:
LBS - set blue led
LBR - reset blue led
LBT - toggle blue led
LRS - set red led
LRR - reset red led
LRT - toggle red led
ADC:
AF - get data from front sensor
AB - get data from back sensor
AP - get akk voltage
MOTOR:
MS - stop motors
MXYZZZ - 	X: 	1 - RIGHT
				2 - LEFT
			Y:	0 - STOP
				1 - FORWARD
				2- BACK
			ZZZ:PWM value
Special:
@ - stop motors
?-start motors
*/
	cli();
	char a = UDR0;
	switch(a)
	{
		case '!':
			/*begin new data frame*/
			current_byte = 0;
			new_command = FALSE;
			break;		
		case '#':
			/*end of data frame*/
			command[current_byte] = '\0';
			new_command = TRUE;	
			break;
			
		case '@':
			stop();
			break;
		case '?':
			motor(RIGHT, FORWARD, 80);
			motor(LEFT, FORWARD, 80);
			break;
			
		default:
			/*get data*/
			command[current_byte]  = a;
			current_byte++;
			if(current_byte > 9)	current_byte = 0;
			break;
	}
	sei();
}



int main(void)
{
	/*Init*/
	/*Configure ports*/
	LED_DDR |= LED_BLUE | LED_RED | LED_GREEN;
	LED_PORT |= LED_BLUE | LED_RED;
	
	M_LEFT_DDR |= M_LEFT_1 | M_LEFT_2;
	M_RIGHT_DDR_1 |= M_RIGHT_1;
	M_RIGHT_DDR_2 |= M_RIGHT_2;
	M_PWM_DDR |= M_PWM_A | M_PWM_B;
	
	/*UART*/
	UBRR0 = 25; /*19200 B*/
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);	/*8-bit*/
	
	/*ADC*/
	ADMUX = (1<<REFS0)|(0<<REFS1)|(1<<ADLAR); /*AVCC*/
	ADCSRA = (1<<ADEN);
	ADCSRA |= (1<<ADSC); 
	
	/*Timer0*/
	TCCR0A = (1<<COM0A1)|(0<<COM0A0)|(1<<COM0B1)|(0<<COM0B0)|(1<<WGM01)|(1<<WGM00); /*non-inverting mode, Fast PWM*/
	TCCR0B = (1<<CS01); /*clk/8*/
	OCR0A = 0;
	OCR0B = 0;
	
	/*Enable interrupts*/
	sei();	
	
	/*End of init*/
	
	
	//M_PWM_PORT |= M_PWM_A | M_PWM_B;
	//M_LEFT_PORT |= M_LEFT_2;
	//M_RIGHT_PORT_2 |= M_RIGHT_2;
	
	/*Main loop*/
	
	while(1)
	{
		if(new_command)
		{
			switch(command[0])
			{
				case 'A':
					switch(command[1])
					{
						case 'F':
							send_number(get_ADC(GROUND_FRONT));
							break;
						case 'B':
							send_number(get_ADC(GROUND_BACK));
							break;
						case 'P':
							send_number(get_ADC(POWER));
							break;
						default:
							break;
					}
					break;
				case 'L':
					switch(command[1])
					{
						case 'B':
							switch(command[2])
							{
								case 'S':
									LED_PORT &= ~LED_BLUE;
									break;
								case 'R':
									LED_PORT |= LED_BLUE;
									break;
								case 'T':
									LED_PORT ^= LED_BLUE;
									break;
								default:
									break;
							}
							break;
						case 'R':
							switch(command[2])
							{
								case 'S':
									LED_PORT &= ~LED_RED;
									break;
								case 'R':
									LED_PORT |= LED_RED;
									break;
								case 'T':
									LED_PORT ^= LED_RED;
									break;
								default:
									break;
							}
							break;
						default:
							break;
					}
					break;
				case 'M':
					switch(command[1])
					{
						case 'S':
							stop();
							break;
						default:
							motor(command[1]-'0', command[2]-'0', ((command[3]-'0')*100+(command[4]-'0')*10+command[5]-'0'));
							break;
					}
					break;
				default:
					break;
			}
			
			new_command = FALSE; /*clear flag*/
			
		}
	}
}