#ifndef HT1632_DRIVER_H
#define HT1632_DRIVER_H

/* -----------------------------------------------------------------------
 * Hardware abstraction
 * ----------------------------------------------------------------------- */
#define access(A)           (*((volatile unsigned char*)A))

#ifndef PORTB
#define PORTB               (access(0x25))
#endif

#ifndef DDRB
#define DDRB                (access(0x24))
#endif

#define SET_ONE(port, pin)  (port |=  (1 << (pin)))
#define SET_ZERO(port, pin) (port &= ~(1 << (pin)))

/* -----------------------------------------------------------------------
 * CPU frequency (required by <util/delay.h>)
 * ----------------------------------------------------------------------- */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdint.h>
#include <util/delay.h>
#include <stdio.h>

/* -----------------------------------------------------------------------
 * Pin assignments  (all on PORTB)
 *   PB5 – WR/EN  (write-enable / clock)
 *   PB3 – DATA
 *   PB2 – CS     (chip-select, active-low)
 * ----------------------------------------------------------------------- */
#define PIN_EN              5
#define PIN_DATA            3
#define PIN_CS              2

/* -----------------------------------------------------------------------
 * HT1632 command set
 * ----------------------------------------------------------------------- */
#define CMD_SYS_EN          0x01    /* Turn on system oscillator          */
#define CMD_LED_ON          0x03    /* Turn on LED driver output          */
#define CMD_BLINK_OFF       0x08    /* Disable blink function             */
#define CMD_PWM_MAX         0xBF    /* Full brightness – 16/16 PWM duty   */
#define CMD_NMOS_8COM       0x20    /* N-MOS open drain, 32×8 COM mode    */

/* -----------------------------------------------------------------------
 * Function prototypes
 * ----------------------------------------------------------------------- */

/**
 * @brief  Configure DDRB pins for EN, DATA, and CS as outputs and
 *         set CS and EN to their idle-high states.
 */
void ht1632_init_pins(void);

/**
 * @brief  Shift @p num_bits bits of @p data out MSB-first on PIN_DATA,
 *         clocked by toggling PIN_EN low then high for each bit.
 *
 * @param  data      Value to transmit (only the lower @p num_bits are used).
 * @param  num_bits  Number of bits to send (1–16).
 */
void ht1632_write_bits(unsigned int data, unsigned char num_bits);

/**
 * @brief  Send a single 8-bit command to the HT1632.
 *         Frames the transfer with the 3-bit "100" command-mode prefix
 *         and a trailing don't-care bit, gated by CS.
 *
 * @param  cmd  One of the CMD_* constants defined above.
 */
void ht1632_send_cmd(unsigned char cmd);

/**
 * @brief  Write a 4-bit nibble to one HT1632 RAM address.
 *         Frames the transfer with the 3-bit "101" write-mode prefix,
 *         a 7-bit address, and 4 data bits, gated by CS.
 *
 * @param  addr  RAM address (0–63, 7 bits).
 * @param  data  Nibble to write (bits [3:0] used).
 */
void ht1632_write_ram(uint8_t addr, uint8_t data);

/**
 * @brief  Initialise the HT1632: configure pins, wait 10 ms, then issue
 *         SYS_EN → NMOS_8COM → LED_ON → BLINK_OFF → PWM_MAX.
 */
void ht1632_init(void);

/**
 * @brief  Light @p mag rows (0–8) of a single display column.
 *         Writes two consecutive nibbles (lower and upper halves of the
 *         8-row column) to RAM addresses @p column and @p column+1.
 *
 * @param  column  Even RAM address of the target column (0, 2, 4 … 62).
 * @param  mag     Number of LEDs to illuminate from the bottom (clamped to 8).
 */
void ht1632_set_amplitude(uint8_t column, uint8_t mag);

/**
 * @brief  Demo entry-point: initialises the display, then sweeps through
 *         all 16 columns lighting 6 rows each for 1 s before turning off.
 *         Loops forever after the sweep.
 *
 * @return Never returns (infinite loop at end).
 */
int main_ht1632_driver(void);

#endif /* HT1632_DRIVER_H */
