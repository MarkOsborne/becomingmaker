/*
 * UsiSerialSend.h
 *
 * Created: 7/1/2016 2:08:56 PM
 *  Author: Mark
 */ 


#ifndef USISERIALSEND_H_
#define USISERIALSEND_H_

#include <stdbool.h>
#include <stdint-gcc.h>

#ifdef __cplusplus
extern "C" {
#endif

// Set your baud rate and here
#define BAUDRATE            9600
#define STOPBITS            1
//F_CPU defined by Arduino, e.g. 1000000, 8000000, 16000000
#define F_CPU 8000000

extern bool usiserial_send_available();
extern void usiserial_send_byte(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* USISERIALSEND_H_ */