#include <avr/io.h>
#include "../include/ht1632_driver.h"
#include "../include/uart_comms.h"

#define BAUD     9600
#define UBRR_VAL (F_CPU / 16 / BAUD - 1)

int main(void) {
//    ht1632_init();
//    USART_Init(UBRR_VAL);


    ht1632_init();
    // Light up all LEDs at address 0x00
    for (uint8_t i = 0; i < 32; i+=2) {
				ht1632_set_amplitude(i, 6);
				_delay_ms(1000);	 	
        ht1632_set_amplitude(i, 0);
		}

    while (1) {}

    while (1) {
        uint8_t col = USART_Wait_To_Receive();
        uint8_t mag = USART_Wait_To_Receive();
        ht1632_set_amplitude(col, mag);
    }
}
