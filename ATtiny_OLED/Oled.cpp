/*
 * Oled.cpp
 *
 * Created: 4/19/2016 10:32:06 PM
 *  Author: Mark
 */ 

#include "Oled.h"

SSD1306_Mini oled;

void Oled_StartScreen()
{
    oled.startScreen();
}

void Oled_Init()
{
    oled.init(0x3c);
    oled.clear();
}

void Oled_BeginDataMode()
{
    Wire.beginTransmission(SlaveAddress);
    Wire.send(GOFi2cOLED_Data_Mode);            // data mode
}

void Oled_EndDataMode()
{
    Wire.endTransmission();
}

void Oled_SendData4(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
        Wire.send( d0 );
        Wire.send( d1 );
        Wire.send( d2 );
        Wire.send( d3 );
}
