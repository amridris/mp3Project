/*
 * ST7735R.cpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */


#include <stddef.h>
#include "utilities.h"
#include "LabGPIO.hpp"
#include <MP3/Drivers/ST7735R.hpp>

//software reset
#define ST7735_SWRESET (0x01)   // Software RESET

//device ID (24 bit)
#define ST7735_RDDID   (0x04)
//Column address
#define ST7735_CASET   (0x2A)
//Row Address
#define ST7735_RASET   (0x2B)
// write to memory
#define ST7735_RAMWR   (0x2C)
//read from memory
#define ST7735_RAMRD   (0x2E)
// Sleep
#define ST7735_SLPIN   (0x10)
//wake (sleep out)
#define ST7735_SLPOUT  (0x11)
//display off
#define ST7735_DISPOFF (0x28)
//display on
#define ST7735_DISPON  (0x29)
// Write direction
#define ST7735_MADCTL  (0x36)

ST7735R* ST7735R::lcd_instance = NULL;

ST7735R& ST7735R::sharedInstance() {
    if (lcd_instance == NULL) lcd_instance = new ST7735R();
    return *lcd_instance;
}

ST7735R::ST7735R() {
    init(SSP1, BIT8, FRAMEMODE_SPI, DIV_1);


    SSPn->CR0 &= ~(1 << 8);
    SSPn->CPSR = 4;

    SSPn->CR1 &= ~(1 << 2);

    CS    = new LabGPIO(2, 7, true, true);
    DC    = new LabGPIO(2, 8, true, true);
    RESET = new LabGPIO(2, 9, true, true);

    toggleReset();
}

ST7735R::~ST7735R() {
    delete DC;
    delete RESET;
}

void ST7735R::toggleReset() {
    CS_select();
    {
        RESET->setLow();
        delay_ms(10);
        RESET->setHigh();
        delay_ms(10);
    }
    CS_deselect();

    writeCMD(ST7735_SLPOUT);
    delay_ms(10);
    writeCMD(ST7735_DISPON);
    delay_ms(10);

    // white screen
    fillScreen(Frame { 0, 0, LCD_WIDTH, LCD_HEIGHT }, WHITE);

    writeCMD(ST7735_MADCTL);
    write(0x84);
}

void ST7735R::Sleep(bool on) {
    DC_select();
    {

        SSPn->CPSR = 4;
        transfer(on ? ST7735_DISPON : ST7735_DISPOFF);

        delay_ms(10);
    }
    DC_deselect();
}

void ST7735R::DisplayOnOff(bool on) {
    DC_select();
    {

        SSPn->CPSR = 4;
        transfer(on ? ST7735_DISPON : ST7735_DISPOFF);

        delay_ms(10);
    }
    DC_deselect();
}

inline void ST7735R::DC_select() {
    if (xSemaphoreTake(spi_Mutex[Perip], portMAX_DELAY)) {
        DC->setLow();
        CS->setLow();
    }
}

inline void ST7735R::DC_deselect() {
    CS->setHigh();
    DC->setHigh();
    xSemaphoreGive(spi_Mutex[Perip]);
}

inline uint8_t ST7735R::write(uint8_t data) {
    uint8_t byte = 0x00;
    CS_select();
    {
        SSPn->CPSR = 2;
        byte = transfer(data);
    }
    CS_deselect();
    return byte;
}

inline uint16_t ST7735R::writeWord(uint16_t data) {
    uint16_t word = 0x0000;
    word = (write(data >> 8) << 8) & 0xFF00;
    word = write(data & 0xFF) & 0xFF;
    return word;
}

inline void ST7735R::writeCMD(uint8_t cmd) {
    DC_select();
    {
        SSPn->CPSR = 2;
        transfer(cmd);
    }
    DC_deselect();
}

inline void ST7735R::pixelColor(Color color, uint32_t repeat) {
    while (repeat > 0) {
        write(color.r);
        write(color.g);
        write(color.b);
        repeat--;
    }
}

inline void ST7735R::setAddressWindow(Frame frame) {


    writeCMD(ST7735_CASET);
    writeWord(frame.y);
    writeWord(frame.y + frame.height - 1);
    writeCMD(ST7735_RASET);
    writeWord(frame.x);
    writeWord(frame.x + frame.width - 1);
    writeCMD(ST7735_RAMWR);
}

void ST7735R::fillScreen(Frame frame, Color c) {
    setAddressWindow(frame);
    pixelColor(c, frame.width * frame.height);
}

void ST7735R::fontDraw(coordinate_2D p, const uint8_t *font, Color color, Color backgroundColor) {
    const uint8_t kFontWidth  = 5;
    const uint8_t kFontHeight = 8;
    bitmap8Draw(Frame { p.x, p.y, kFontWidth, kFontHeight }, font, color, backgroundColor);
}

void ST7735R::bitmap8Draw(Frame frame, const uint8_t *bitmap, Color color, Color backgroundColor) {
    setAddressWindow(frame);
    for (uint8_t y = 0; y < frame.width; y++) {
        for (uint8_t x = 0; x < frame.height; x++) {
            pixelColor( !!(bitmap[y] & (1 << x)) ? color : backgroundColor );
        }
    }
}

void ST7735R::Bitmap16Draw(Frame frame, const uint16_t *bitmap, Color color, Color backgroundColor) {
    setAddressWindow(frame);
    for (uint8_t y = 0; y < frame.width; y++) {
        for (uint8_t x = 0; x < frame.height; x++) {
            pixelColor( !!(bitmap[y] & (1 << x)) ? color : backgroundColor );
        }
    }
}
