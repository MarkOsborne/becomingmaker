/*
 * UsiSerialSend.c
 *
 * Created: 6/28/2016 2:25:43 PM
 * Author : Mark
 */ 

//F_CPU defined by Arduino, e.g. 1000000, 8000000, 16000000
#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/iotnx5.h>
#include <avr/interrupt.h>
#include <stdint-gcc.h>
#include "usiserialsend.h"


void setup()
{
    PORTB |= 1 << PB1;              // Ensure serial output is high
    DDRB  |= (1<<PB1);              // Configure USI_DO as output.
    USICR = 0;                      // Start with USI disabled.
    sei();                          // Enable global interrupts
}

void test()
{
    while (1)
    {
        for(uint8_t i = 0; i<2; i++)
        {
            while (!usiserial_send_available())
            {
                // Wait for last send to complete
            }
            usiserial_send_byte(0b00011000);
        }
        _delay_ms(500);
    }
}

void helloWorld()
{
    char message[] = "Hello World!\r\n";
    uint8_t len = sizeof(message)-1;
    while (1)
    {
        for (uint8_t i = 0; i<len; i++)
        {
            while (!usiserial_send_available())
            {
                // Wait for last send to complete
            }
            usiserial_send_byte(message[i]);
        }
        _delay_ms(1000);
    }
}
int main(void)
{
    setup();
    //test();
    helloWorld();
}

