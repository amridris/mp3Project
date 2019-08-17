/*
 * ST7735R.hpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#ifndef ST7735R_HPP_
#define ST7735R_HPP_

#include <MP3/graphic.h>
#include "SPI.hpp"

class LabGPIO;

#define LCD_display ST7735R::sharedInstance()

#define ST7735_TFT_WIDTH  128
#define ST7735_TFT_HEIGHT 160


class ST7735R: public SPI {

protected:

    static ST7735R *lcd_instance;

    LabGPIO *DC;
    LabGPIO *RESET;

    ST7735R();

    // Toggle D/C low to interpret transmission byte as command byte.
    inline void DC_select();
    // Toggle D/C high to interpret transmission byte as data byte.
    inline void DC_deselect();

    /**
     * Write a byte of data to display.
     * @param  data 8-bit data
     * @return
     */
    inline uint8_t write(uint8_t data);

    /**
     * Write a word to display.
     *
     * @param  data 16-bit data.
     * @return
     */
    inline uint16_t writeWord(uint16_t data);

    /**
     * Send command opcode to display
     * @param cmd Opcode
     */
    inline void writeCMD(uint8_t cmd);

    /**
     * Set address window for drawing.
     *
     * @param frame Frame of window.
     */
    inline void setAddressWindow(Frame frame);

    /**
     * Set pixel color.
     * @param color
     * @param repeat
     */
    inline void pixelColor(Color color, uint32_t repeat = 1);

public:

    static ST7735R& sharedInstance();

    virtual ~ST7735R();

    /// Toggle hardware reset.
    void toggleReset();
    /// Toggle sleep display to save power.
    void Sleep(bool on);
    /// Turn display on or off.
    void DisplayOnOff(bool on);

    void fillScreen(Frame frame, Color c);

    
    //character draw starting at given point.

    void fontDraw(coordinate_2D p, const uint8_t *font, Color color, Color backgroundColor);

    
    //draw 8 bit bitmap to the given frame.
    
    void bitmap8Draw(Frame frame, const uint8_t *bitmap, Color color, Color backgroundColor);

    
    //draw a 16-bit bitmap to the given frame.
    
    void Bitmap16Draw(Frame frame, const uint16_t *bitmap, Color color, Color backgroundColor);
};

#endif /* ST7735_HPP_ */
