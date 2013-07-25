/*********************************
UART
*********************************/
#ifndef uart_h
#define uart_h

#include <avr/io.h>

#define ENTER 13

void send_number(unsigned char n);
/*Convert byte number to ASCII and send by UART*/

void send_string(char* data);
/*Send string*/

#endif //uart_h