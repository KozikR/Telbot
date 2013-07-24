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
#include <avr/delay.h>
#include <string.h>
#include <avr/interrupt.h>

#include "board_def.h"

/*Interrupts*/
ISR(USART_RX_vect)
{
	cli();
	char a = UDR0;
	sei();
}


int main(void)
{
	/*Init*/
	/*Configure ports*/
	LED_DDR |= LED_BLUE | LED_RED | LED_GREEN;
	
	M_LEFT_DDR |= M_LEFT_1 | M_LEFT_2;
	M_RIGHT_DDR_1 |= M_RIGHT_1;
	M_RIGHT_DDR_2 |= M_RIGHT_2;
	M_PWM_DDR |= M_PWM_A | M_PWM_B;
	
	/*UART*/
	UBRR0 = 25; /*19200 B*/
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	
	sei();	/*Enable interrupts*/
	
	/*Main loop*/
	while(1)
	{
		LED_PORT = ~LED_PORT;
	
		_delay_ms(200);
	}
}