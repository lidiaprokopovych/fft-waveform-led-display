#define access(A) (*((volatile unsigned char*)A))
#define PORTB (access(0x25))
#define DDRB (access(0x24))
#define SET_ONE(port, pin) (port |= (1 << pin))
#define SET_ZERO(port, pin) (port &= ~(1<< pin))
#define F_CPU 16000000UL
#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>
// write en is PB5
// data is PB3
// CS is PB2

#define PIN_EN 5
#define PIN_DATA 3
#define PIN_CS 2

#define CMD_SYS_EN   0x01   // Turn on oscillator
#define CMD_LED_ON   0x03   // Turn on LED driver
#define CMD_BLINK_OFF 0x08  // Disable blinking
#define CMD_PWM_MAX  0xBF   // Full brightness (16/16 PWM)
#define CMD_NMOS_8COM 0x20  // N-MOS, 32x8 mode (most common)

void ht1632_init_pins () {
	SET_ONE(DDRB, PIN_EN);
	SET_ONE(DDRB, PIN_DATA);
	SET_ONE(DDRB, PIN_CS);
	SET_ONE(PORTB, PIN_CS);
	SET_ONE(PORTB, PIN_EN);
}

void ht1632_write_bits (unsigned int data, unsigned char num_bits) {
	for (int i = num_bits-1; i >= 0; i--) {
		SET_ZERO(PORTB, PIN_EN);
		
		if (data&(1<<i)) SET_ONE(PORTB, PIN_DATA);
		else SET_ZERO(PORTB, PIN_DATA);



		SET_ONE(PORTB, PIN_EN);

	}
}

void ht1632_send_cmd (unsigned char cmd) {
	SET_ZERO(PORTB, PIN_CS);
	ht1632_write_bits(0b100, 3);
	ht1632_write_bits(cmd, 8);
	ht1632_write_bits(0, 1);
	SET_ONE(PORTB, PIN_CS);
	
}

void ht1632_write_ram(uint8_t addr, uint8_t data) {
    SET_ZERO(PORTB, PIN_CS);
		ht1632_write_bits(0b101, 3);     // write mode ID
    ht1632_write_bits(addr, 7);      // 7-bit address
    ht1632_write_bits(data, 4);      // 4-bit data (nibble)
		SET_ONE(PORTB, PIN_CS);
}

void ht1632_init(void) {
    ht1632_init_pins();
    _delay_ms(10);
    ht1632_send_cmd(CMD_SYS_EN);
    ht1632_send_cmd(CMD_NMOS_8COM);
    ht1632_send_cmd(CMD_LED_ON);
    ht1632_send_cmd(CMD_BLINK_OFF);
    ht1632_send_cmd(CMD_PWM_MAX);
}

void ht1632_set_amplitude(uint8_t column, uint8_t mag) {
		if (mag > 8) mag = 8;
		uint16_t mask = (1 << mag) - 1;
		ht1632_write_ram(column*2, mask & (0b1111));
		ht1632_write_ram(column*2 + 1, mask >> 4);
}

int main(void) {
    ht1632_init();
    // Light up all LEDs at address 0x00
    for (uint8_t i = 0; i < 32; i+=2) {
				ht1632_set_amplitude(i, 1);
				_delay_ms(1000);	 	
        ht1632_set_amplitude(i, 0);
		}

    while (1) {}
}



