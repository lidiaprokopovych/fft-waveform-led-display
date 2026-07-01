#include <avr/io.h>

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void USART_Init(unsigned int ubrr) {
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Set frame format: 8data, 1stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

unsigned char USART_Wait_To_Receive(void) {
    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)));
    // Get and return received data from buffer
    return UDR0;
}

unsigned char USART_Send(unsigned char data) 
{
	while (1) {
		if (UCSR0A & (1<<UDRE0)) break;
	}
	UDR0 = data;
}
