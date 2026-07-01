#include <avr/io.h>
#include "../include/ht1632_driver.h"
#include "../include/uart_comms.h"

#define BAUD     9600
#define UBRR_VAL (F_CPU / 16 / BAUD - 1)

int main(void) {
    ht1632_init();
    USART_Init(UBRR_VAL);

    while (1) {
        uint8_t col = USART_Wait_To_Receive();
        uint8_t mag = USART_Wait_To_Receive();
        ht1632_set_amplitude(col, mag);
    }
}
