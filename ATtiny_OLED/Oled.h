/*
 * Oled.h
 *
 * Created: 4/19/2016 10:31:20 PM
 *  Author: Mark
 */ 


#ifndef OLED_H_
#define OLED_H_

#include "SSD1306_minimal.h"

void Oled_Init();
void Oled_StartScreen();
void Oled_BeginDataMode();
void Oled_EndDataMode();
void Oled_SendData4(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);



#endif /* OLED_H_ */