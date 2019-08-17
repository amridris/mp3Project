/*
 * View.cpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#include <MP3/Drivers/ST7735R.hpp>
#include "View.hpp"

#include <stddef.h>
#include <stdio.h>

View::View(Frame frame) {
    _frame = frame;
    _background_Color = TEAL;
    _dirty = true;
}

View::~View() { }

void View::setFrame(Frame frame) {
    _frame = frame;
}

void View::setOrigin(coordinate_2D origin) {
    _frame.origin = origin;
}
void View::setSize(size_2D size) {
    _frame.size = size;
}

void View::setBackgroundColor(Color c) {
    _background_Color = c;
}

void View::draw() {
    _dirty = true;
    reDraw();
}

void View::reDraw() {
    reDrawLCDWithBackground(NULL);
}

void View::reDrawLCDWithBackground(Color *color) {
    if (_dirty) {
        LCD_display.fillScreen(_frame, color == NULL ? _background_Color : *color);
        _dirty = false;
    }
}
