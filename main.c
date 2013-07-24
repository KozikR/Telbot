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

/*Interrupts*/
ISR(USART_RX_vect)
{
	cli();
	char a = UDR0;
	if(a == '1')	LED_PORT ^= LED_RED;
	if(a == '2')
	{
		send_string("FRONT:");
		send_number(get_ADC(GROUND_FRONT));
		send_string("BACK:");
		send_number(get_ADC(GROUND_BACK));
		send_string("POWER:");
		send_number(get_ADC(POWER));
	}
	sei();
}

/*Functions*/

/*Global variables*/

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
	
	/*Timer0*/
	TCCR0A = (1<<COM0A1)|(0<<COM0A0)|(1<<COM0B1)|(0<<COM0B0)|(1<<WGM01)|(1<<WGM00); /*non-inverting mode, Fast PWM*/
	TCCR0B = (1<<CS01); /*clk/8*/
	OCR0A = 0;
	OCR0B = 0;
	
	/*Enable interrupts*/
	sei();	
	
	motor(RIGHT, BACK, 200);
	motor(LEFT, BACK, 200);
	
	/*Main loop*/
	while(1)
	{
		LED_PORT ^= LED_BLUE;
		_delay_ms(200);
	}
}