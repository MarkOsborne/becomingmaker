/*
 * usiserialsend.c
 *
 * Created: 7/1/2016 2:09:31 PM
 *  Author: Mark
 */ 

#include "usiserialsend.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/iotnx5.h>
#include <avr/interrupt.h>

// If bit width in cpu cycles is greater than 255 then  divide by 8 to fit in timer
// Calculate prescaler setting
#define CYCLES_PER_BIT       ( (F_CPU) / (BAUDRATE) )
#if (CYCLES_PER_BIT > 255)
#define DIVISOR             8
#define CLOCKSELECT         2
#else
#define DIVISOR             1
#define CLOCKSELECT         1
#endif
#define FULL_BIT_TICKS      ( (CYCLES_PER_BIT) / (DIVISOR) )

#ifdef ARDUINO
volatile static uint8_t oldTCCR0A;
volatile static uint8_t oldTCCR0B;
volatile static uint8_t oldTCNT0;
#endif

// USISerial send state variable
enum USISERIAL_SEND_STATE { AVAILABLE, FIRST, SECOND };
static volatile uint8_t usiserial_send_state = AVAILABLE;
static inline uint8_t usiserial_send_get_state(void)
{
    return usiserial_send_state;
}
static inline void usiserial_send_set_state(enum USISERIAL_SEND_STATE state)
{
    usiserial_send_state=state;
}
bool usiserial_send_available()
{
    return usiserial_send_get_state()==AVAILABLE;
}

// Transmit data persistent between USI OVF interrupts
static volatile uint8_t usiserial_tx_data;
static inline uint8_t usiserial_get_tx_data(void)
{
    return usiserial_tx_data;
}
static inline void usiserial_set_tx_data(uint8_t tx_data)
{
    usiserial_tx_data = tx_data;
}

static uint8_t reverse_byte (uint8_t x) {
    x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
    x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
    x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
    return x;
}

void usiserial_send_byte(uint8_t data)
{
    while (usiserial_send_get_state() != AVAILABLE)
    {
        // Spin until we finish sending previous packet
    };
    usiserial_send_set_state(FIRST);
    usiserial_set_tx_data(reverse_byte(data));

    // Save current Arduino timer state
#ifdef ARDUINO
    oldTCCR0B = TCCR0B;
    oldTCCR0A = TCCR0A;
    oldTCNT0 = TCNT0;
#endif

    // Configure Timer0
    TCCR0A = 2<<WGM00;                      // CTC mode
    TCCR0B = CLOCKSELECT;                   // Set prescaler to clk or clk /8
    GTCCR |= 1 << PSR0;                     // Reset prescaler
    OCR0A = FULL_BIT_TICKS;                 // Trigger every full bit width
    TCNT0 = 0;                              // Count up from 0 

    // Configure USI to send high, start bit and 6 bits of data
    USIDR = 0x00 |                            // Start bit (low)
        usiserial_get_tx_data() >> 1;         // followed by first 7 bits of serialOutput
    USICR  = (1<<USIOIE)|                     // Enable USI Counter OVF interrupt.
        (0<<USIWM1)|(1<<USIWM0)|              // Select three wire mode to ensure USI written to PB1
        (0<<USICS1)|(1<<USICS0)|(0<<USICLK);  // Select Timer0 Compare match as USI Clock source.
    DDRB  |= (1<<PB1);                        // Configure USI_DO as output.
    USISR = 1<<USIOIF | (16 - 8);             // Clear USI overflow interrupt flag and set USI counter to 8
}

// USI overflow interrupt indicates we've sent a packet
ISR (USI_OVF_vect) {
    if (usiserial_send_get_state() == FIRST)
    {
        usiserial_send_set_state(SECOND);
        USIDR = usiserial_get_tx_data() << 7  // Send last 1 bit of data
            | 0x7F;                             // and stop bits (high)
        USISR = 1<<USIOIF |                   // Clear USI overflow interrupt flag
            (16 - (1 + (STOPBITS)));          // and set USI counter to send last bit and stop bits
    }
    else
    {
        PORTB |= 1 << PB1;              // Ensure output is high
        DDRB  |= (1<<PB1);              // Configure USI_DO as output.
        USICR = 0;                      // Disable USI.
        USISR |= 1<<USIOIF;             // clear interrupt flag
        usiserial_send_set_state(AVAILABLE);

        //Restore old timer values for Arduino
#ifdef ARDUINO
        TCCR0A = oldTCCR0A;
        TCCR0B = oldTCCR0B;
        // Note Arduino millis() and micros() will lose the time it took us to send a byte
        // Approximately 1ms at 9600 baud
        TCNT0 = oldTCNT0;
#endif
    }
}
