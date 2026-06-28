#include <avr/io.h>
#include "ht1632_driver.h"

#define BAUD     9600
#define UBRR_VAL (F_CPU / 16 / BAUD - 1)

void uart_init(void) {
    UBRR0H = (UBRR_VAL >> 8);
    UBRR0L =  UBRR_VAL;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);   /* 8-bit frames */
}

uint8_t uart_read(void) {
    while (!(UCSR0A & (1 << RXC0)));           /* wait for byte */
    return UDR0;
}

int main(void) {
    ht1632_init();
    uart_init();

    while (1) {
        uint8_t col = uart_read();
        uint8_t mag = uart_read();
        ht1632_set_amplitude(col, mag);
    }
}
