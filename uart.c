/*********************************
UART
*********************************/
#include "uart.h"

void send_number(char n)
{
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = n/100+'0';
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = (n%100)/10+'0';
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = (n%10)+'0';
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = ENTER;
}

void send_string(char* data)
{
	int i = 0;
	while(data[i] != '\0')
	{
		while ( !( UCSR0A & (1<<UDRE0)) );
		UDR0 = data[i];
		i++;
	}
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = ENTER;
}