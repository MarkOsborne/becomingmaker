/*
* Game.cpp
*
* Created: 4/19/2016 10:23:45 PM
*  Author: Mark
*/
/*
display 128x64
snake size 4x4

128 / 8 => 16cols
64 / 8 => 8rows

*/
//#include <Arduino.h>
//#include "pch.h"
#include <inttypes.h>
#include "Game.h"

#define ColCount 32
#define RowCount 16 

// there are different wall types
unsigned char wall[6][4] = {
    0x0, 0x0, 0x0, 0x0,
    0xf, 0xf, 0xf, 0xf,
    0xf, 0x9, 0x9, 0xf,
    0x9, 0x9, 0x9, 0x9,
    0x6, 0x9, 0x9, 0x6
};

// the car shape
unsigned char carShape[8] = { 0xf, 0xf, 0xf, 0xf,  0x9, 0xf, 0xf, 0x6 };


static unsigned char snakeRow = 10;
static unsigned char snakeCol = 7;
static char snakeRowDir = +1;
static char snakeColDir = -1;
static char wallIdx = 1;

uint8_t car = 8;
static uint8_t centerBuffer[ColCount];
uint8_t bufferPos = 0;
uint16_t gameCounter = 0;

// this is the room shape
const static uint8_t room[] PROGMEM = {
    1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,2,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,0,0,0,2,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,2,0,0,0,0,2 , 2,2,2,0,0,0,0,1,
    1,0,0,0,2,0,0,0 , 0,0,2,2,2,0,0,0 , 0,0,2,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,0,0,0,2,0,0,0 , 0,0,0,0,2,0,0,0 , 0,0,2,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,0,0,0,2,0,0,0 , 0,0,0,0,2,2,2,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0 , 0,2,2,2,2,0,0,0 , 0,2,0,0,0,0,0,2 , 2,2,2,2,0,0,0,1,
    1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,2,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1,
};

static uint8_t bitScreen[16][4];/* = {
    { 0b11111111,0b11111111,0b11111111,0b11111111 },
    { 0b00000000,0b00000000,0b00000000,0b00000000 },
    { 0b00000000,0b00000000,0b00000000,0b00000000 },
    { 0b00000000,0b00000001,0b10000000,0b00000000 },
    { 0b00000000,0b00000010,0b01000000,0b00000000 },
    { 0b00000000,0b00000100,0b00100000,0b00000000 },
    { 0b00000000,0b00000000,0b00000000,0b00000000 },
    { 0b00000000,0b00000000,0b00000000,0b00000000 },
    { 0b00000000,0b00000000,0b00000000,0b00000000 },
    { 0b00000000,0b00000010,0b01000000,0b00000000 },
    { 0b00000000,0b00000001,0b10000000,0b00000000 },
    { 0b00000000,0b00000000,0b00000000,0b00000000 },
    { 0b00000000,0b00000000,0b00000000,0b00000000 },
    { 0b00000000,0b00000000,0b00000000,0b00000000 },
    { 0b00000000,0b00000000,0b00000000,0b00000000 },
    { 0b11111111,0b11111111,0b11111111,0b11111111 }
};*/

unsigned char getRoom(unsigned char row, unsigned char col) {

    unsigned char data = pgm_read_byte(&(room[row*ColCount + col]));

    return data;
}

unsigned char getSnake(unsigned char row, unsigned char col) {


    unsigned char data = 0;

    if ((row == snakeRow) && (col == snakeCol)) {
        data = 2;
    }

    return data;
}


unsigned char hitRoom(unsigned char row, unsigned char col) {

    uint8_t data;
    data = getRoom(row, col);

    return data;
}


void changeWallIdx() {
    wallIdx++;
    if (wallIdx>1) {
        wallIdx = 0;
    }
}

void move() {

    char bHitRoom;
    char hitType;

    do {
        bHitRoom = 0;
        // test row
        hitType = hitRoom(snakeRow + snakeRowDir, snakeCol);
        if (hitType != 0) {
            snakeRowDir = -1 * snakeRowDir;
            bHitRoom = hitType;
        }
        // test col
        hitType = hitRoom(snakeRow, snakeCol + snakeColDir);
        if (hitType != 0) {
            snakeColDir = -1 * snakeColDir;
            bHitRoom = hitType;
        }
        //
        if (bHitRoom == 1) {
            //nur an der wand ändern
            //changeWallIdx();
        }

    } while (bHitRoom);

    snakeRow = snakeRow + snakeRowDir;
    snakeCol = snakeCol + snakeColDir;


}

void updateOLED(byte i)
{
    Oled_StartScreen();
    uint8_t upperRow;
    uint8_t lowerRow;
    uint8_t data[4];

    for (char r = 0; r<RowCount; r = r + 2) {
        for (char c = 0; c<ColCount; c++) {
            // send a bunch of data in one xmission
            Oled_BeginDataMode();

            //data= 0x0;
            data[0] = 0x0;
            data[1] = 0x0;
            data[2] = 0x0;
            data[3] = 0x0;

            // road
            upperRow = bitScreen[r][c / 8] & (128 >> (c % 8));
            lowerRow = bitScreen[r + 1][c / 8] & (128 >> (c % 8));
            if (upperRow) {
                data[0] |= wall[4][0] << 0;
                data[1] |= wall[4][1] << 0;
                data[2] |= wall[4][2] << 0;
                data[3] |= wall[4][3] << 0;
            }
            if (lowerRow) {
                data[0] |= wall[4][0] << 4;
                data[1] |= wall[4][1] << 4;
                data[2] |= wall[4][2] << 4;
                data[3] |= wall[4][3] << 4;
            }

            // car
            if (c == 1 && r == car)
            {
                data[0] |= carShape[0] << 0;
                data[1] |= carShape[1] << 0;
                data[2] |= carShape[2] << 0;
                data[3] |= carShape[3] << 0;
            }
            if (c == 1 && r + 1 == car)
            {
                data[0] |= carShape[0] << 4;
                data[1] |= carShape[1] << 4;
                data[2] |= carShape[2] << 4;
                data[3] |= carShape[3] << 4;
            }
            if (c == 2 && r == car)
            {
                data[0] |= carShape[4] << 0;
                data[1] |= carShape[5] << 0;
                data[2] |= carShape[6] << 0;
                data[3] |= carShape[7] << 0;
            }
            if (c == 2 && r + 1 == car)
            {
                data[0] |= carShape[4] << 4;
                data[1] |= carShape[5] << 4;
                data[2] |= carShape[6] << 4;
                data[3] |= carShape[7] << 4;
            }

            Oled_SendData4(data[0], data[1], data[2], data[3]);
            Oled_EndDataMode();
        }
    }
}

void shiftRowLeft(byte row, bool roll, byte newBit = 0)
{
    byte nBit = roll ? (bitScreen[row][0] >> 7) : newBit;
    byte overflow = nBit ? 1 : 0;
    for (byte i = 4; i>0; i--)
    {
        byte c = i - 1;
        byte newOverflow = bitScreen[row][c] >> 7;
        bitScreen[row][c] = (bitScreen[row][c] << 1) | overflow;
        overflow = newOverflow;
    }
}

void shiftRowRight(byte row, bool roll, byte newBit = 0)
{
    byte nBit = roll ? (bitScreen[row][3] << 7) : newBit;
    byte overflow = nBit ? 128 : 0;
    for (byte i = 0; i<4; i++)
    {
        byte c = i;
        byte newOverflow = bitScreen[row][c] << 7;
        bitScreen[row][c] = (bitScreen[row][c] >> 1) | overflow;
        overflow = newOverflow;
    }
}

byte center = 8;
byte width = 8;
static const int8_t PROGMEM bends[6][8] = {
    { -1,-2,-3,-4, -3,-2,-1, 0 },
    { 1, 2, 3, 4,  3, 2, 1, 0 },
    { 0, 1, 1, 2,  2, 1,1, 0 },
    { 0,-1,-1,-2, -2,-1, -1, 0 },
    { 1, 0,-1, 0,  1, 2, 1, 0 },
    { -1,-2,-2,-3, -3,-2,-1, 0 } };

#define DIVISOR ( RAND_MAX / 6 )

bool autoPilot = false;

int8_t top;
int8_t bot;
volatile int16_t bend = DIVISOR;
int roadSegIndex = 0;
bool gameOver = false;
int gameOverCountdown = 0;

bool HitTest()
{
    // Check second and third positions in the car row
    return (bitScreen[car][0] & 0b01100000);
}

void GameTick()
{
    if (gameOver)
    {
        if (--gameOverCountdown)
            return;
        gameOver = false;
        //car = centerBuffer[bufferPos];
    }


    // If we are starting a new segment, pick a new bend at random
    if (roadSegIndex == 0)
    {
        bend = rand() / DIVISOR;
    }

    // Get bend data
    int8_t delta = pgm_read_byte(&(bends[bend][roadSegIndex]));
    top = center - width + delta;
    bot = center + width + delta;
    if (bot >= 16) bot = 15;
    if (top < 0) top = 0;

    // Increment the segment index
    roadSegIndex = ++roadSegIndex % 8;

    // Save next position and get current position for autopilot
    centerBuffer[bufferPos] = center + delta;
    bufferPos = ++bufferPos % ColCount;
    if (autoPilot)
    {
        car = centerBuffer[bufferPos];
    }

    // Update the display
    updateOLED(0);

    if (HitTest())
    {
        gameOver = true;
        gameOverCountdown = 200;
    }

    // Scroll the road map to the left
    for (int r = 0; r<16; r++)
    {
        // New bit is set if row matches top of bot road edges
        shiftRowLeft(r, false, r <= top || r >= bot);
    }

    uint16_t scaledCounter = gameCounter/256;
    width = 8 - scaledCounter;
    gameCounter++;
    if (scaledCounter>=6) gameCounter = 0;
    }

void GameInit()
{
    // Initial start position for auto pilot
    for (uint8_t i = 0; i < ColCount; i++)
        centerBuffer[i] = 8;
}