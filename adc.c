/*********************************
ADC
*********************************/
#include "adc.h"

char get_ADC(char chanel)
{
	ADMUX &= 0xf0; 
	ADMUX |= chanel; 
  
	ADCSRA |= 1<<ADSC; 
	while(ADCSRA & (1<<ADSC));
	//ADCSRA &= ~((1 << ADSC) | (1 << ADIF));
	//ADCSRA |= 1<<ADSC;
	//while(ADCSRA & (1<<ADSC));	
	//ADCSRA &= ~((1 << ADSC) | (1 << ADIF));
	return ADCH;
}