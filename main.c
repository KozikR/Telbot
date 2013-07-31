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

#define FRAME_LENGHT 15

#define SAFETY_MARGIN 30

/*Global variables*/
volatile unsigned char command[FRAME_LENGHT+1];
volatile unsigned char new_command = FALSE;
volatile unsigned char current_byte = 0;

volatile unsigned char current_sensor = 0;


/*Functions*/
void send_sensor_data(char sesnor_number)
{
	unsigned char data = 0;
	while ( !( UCSR0A & (1<<UDRE0)) );
	switch(sesnor_number)
	{
		case 0:
			data = get_ADC(POWER);
			UDR0 = 'P';
			break;
		case 1:
			data = get_ADC(GROUND_FRONT);
			UDR0 = 'F';
			break;
		case 2:
			data = get_ADC(GROUND_BACK);
			UDR0 = 'B';
			break;
		default:
			break;
	}

	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = data/100+'0';
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = (data%100)/10+'0';
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = (data%10)+'0';
	
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = ENTER;
}

/*Interrupts*/
ISR(USART_RX_vect)
{
/*Get data fram
!up to 14 byte#
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
MBYZZZYZZZ - send state of RIGHT and LEFT motor
MPXZZZ	-set ZZZ as PWM of X motor
MPBZZZ	-set ZZZ as PWM of both motor
MDYY	-set direction of motor RIGHT and LEFT
SS - set safety mode
SR - stop safety mode

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
			if(current_byte > (FRAME_LENGHT-1))	current_byte = 0;
			break;
	}
	sei();
}

int main(void)
{
	/*Variables*/
	unsigned char safety_mode = 0;
	unsigned char last_front = 0;
	unsigned char last_back = 0;
	
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
	ADMUX = (1<<REFS0)|(0<<REFS1)|(1<<ADLAR)|POWER; /*AVCC*/
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
	int time = 0;
	int time2 = 0;
	char sensor_number = 0;
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
						case 'B':
							motor(1, command[2]-'0', ((command[3]-'0')*100+(command[4]-'0')*10+command[5]-'0'));
							motor(2, command[6]-'0', ((command[7]-'0')*100+(command[8]-'0')*10+command[9]-'0'));
							break;
						case 'P':
							switch(command[2])
							{
								case (LEFT+'0'):
									OCR0B =	((command[3]-'0')*100+(command[4]-'0')*10+command[5]-'0');
										break;
								case (RIGHT+'0'):
									OCR0A = ((command[3]-'0')*100+(command[4]-'0')*10+command[5]-'0');
									break;
								case 'B':
									OCR0A = ((command[3]-'0')*100+(command[4]-'0')*10+command[5]-'0');
									OCR0B =	((command[3]-'0')*100+(command[4]-'0')*10+command[5]-'0');
									break;
								default:
									break;
							}
							break;
						case 'D':
							motors(command[2]-'0', command[3]-'0');
							break;
						default:
							motor(command[1]-'0', command[2]-'0', ((command[3]-'0')*100+(command[4]-'0')*10+command[5]-'0'));
							break;
					}
					break;
				case 'S':
					switch(command[1])
					{
						case 'S':
							safety_mode = 1;
							break;
						case 'R':
							safety_mode = 0;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
			
			new_command = FALSE; /*clear flag*/
			
		}
		time++;
		if(time == 10000)
		{
			time = 0;
			time2++;
			if(time2 > 2)
			{
				unsigned char data = 0;
				while (!( UCSR0A & (1<<UDRE0)));
				switch(sensor_number)
				{
					case 0:
						data = get_ADC(POWER);
						UDR0 = 'P';
						break;
					case 1:
						data = get_ADC(GROUND_FRONT);
						if((data > last_front + SAFETY_MARGIN || data < last_front - SAFETY_MARGIN) && safety_mode == 1)
						{
							UDR0 = 'G';
							stop();
						}
						last_front = data;
						UDR0 = 'F';
						break;
					case 2:
						data = get_ADC(GROUND_BACK);
						if((data > last_back + SAFETY_MARGIN || data < last_back - SAFETY_MARGIN) && safety_mode == 1)
						{
							UDR0 = 'C';
							stop();
						}
						last_back = data;
						UDR0 = 'B';
						break;
					default:
						break;
				}

				while ( !( UCSR0A & (1<<UDRE0)) );
				UDR0 = data/100+'0';
				while ( !( UCSR0A & (1<<UDRE0)) );
				UDR0 = (data%100)/10+'0';
				while ( !( UCSR0A & (1<<UDRE0)) );
				UDR0 = (data%10)+'0';
				
				while ( !( UCSR0A & (1<<UDRE0)) );
				UDR0 = ENTER;
				sensor_number++;
				if(sensor_number >= 3)	sensor_number = 0;
				time2 = 0;
			}
		}
		
		
	}
}