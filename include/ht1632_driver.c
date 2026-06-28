#include "ht1632_driver.h"

void ht1632_init_pins(void) {
    SET_ONE(DDRB, PIN_EN);
    SET_ONE(DDRB, PIN_DATA);
    SET_ONE(DDRB, PIN_CS);
    SET_ONE(PORTB, PIN_CS);
    SET_ONE(PORTB, PIN_EN);
}

void ht1632_write_bits(unsigned int data, unsigned char num_bits) {
    for (int i = num_bits - 1; i >= 0; i--) {
        SET_ZERO(PORTB, PIN_EN);

        if (data & (1 << i)) SET_ONE(PORTB, PIN_DATA);
        else                  SET_ZERO(PORTB, PIN_DATA);

        SET_ONE(PORTB, PIN_EN);
    }
}

void ht1632_send_cmd(unsigned char cmd) {
    SET_ZERO(PORTB, PIN_CS);
    ht1632_write_bits(0b100, 3);
    ht1632_write_bits(cmd,   8);
    ht1632_write_bits(0,     1);
    SET_ONE(PORTB, PIN_CS);
}

void ht1632_write_ram(uint8_t addr, uint8_t data) {
    SET_ZERO(PORTB, PIN_CS);
    ht1632_write_bits(0b101, 3);    /* write mode ID  */
    ht1632_write_bits(addr,  7);    /* 7-bit address  */
    ht1632_write_bits(data,  4);    /* 4-bit nibble   */
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

    uint16_t lower_mask = 0;
    uint16_t upper_mask = 0;

    for (int i = 0; i < 4; i++) {
        if (mask & (1 << i))       lower_mask |= (1 << (3 - i));
        if (mask & (1 << (i + 4))) upper_mask |= (1 << (3 - i));
    }

    ht1632_write_ram(column,     lower_mask);
    ht1632_write_ram(column + 1, upper_mask);
}

int main_ht1632_driver(void) {
    ht1632_init();

    for (uint8_t i = 0; i < 32; i += 2) {
        ht1632_set_amplitude(i, 6);
        _delay_ms(1000);
        ht1632_set_amplitude(i, 0);
    }

    while (1) {}
}
