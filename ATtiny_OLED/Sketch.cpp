/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */
#include "Game.h"

//Beginning of Auto generated function prototypes by Atmel Studio
void heartBeat();
void setup();
void loop();
//End of Auto generated function prototypes by Atmel Studio

         
void setup()
{
//    pinMode(4, INPUT);
    Oled_Init();
}

void GameLoop()
{
    GameInit();
    while (1)
    {
        volatile int potPos = analogRead(2); // ADC2 = PB4, Max value = 1023
        volatile int scaledPos = (potPos / 32) - 8;
        if (scaledPos<0) scaledPos =0;
        if (scaledPos>15) scaledPos = 15;
        car = scaledPos;
        GameTick();
        delay(5);
    }
}

void loop()
{
    GameLoop();
}