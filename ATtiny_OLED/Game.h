/*
* Game.h
*
* Created: 4/19/2016 10:27:07 PM
*  Author: Mark
*/


#ifndef GAME_H_
#define GAME_H_

//#include <avr/pgmspace.h>
#define PROGMEM
#define pgm_read_byte *
#define sleep(a)

#include "oled.h"

extern uint8_t car;

void GameLoop();
void GameTick();

void GameInit();

#endif /* GAME_H_ */
