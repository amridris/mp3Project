/*
 * currentView.cpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#include <MP3/Drivers/ST7735R.hpp>
#include <MP3/MP3FileStructure.hpp>
#include <MP3/UI/currentView.hpp>
#include <string.h>


// play icon created using the icon edit software
const uint16_t play_icon[] = {
    0b0000000000000000,
    0b0001111111111100,
    0b0000111111111000,
    0b0000011111110000,
    0b0000001111100000,
    0b0000000111000000,
    0b0000000010000000,
    0b0000000000000000
};

// play icon created using the icon edit software
const uint16_t paused_icon[] = {
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000000000000000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000000000000000,
};

// play icon created using the icon edit software
const uint16_t stop_icon[] = {
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000111111111000,
    0b0000000000000000
};

// notifyView Implementation

void currentView::startPlaying(MP3FileStructure *song) {
    _state = MP3Player::PLAYING;
    _song = song;
    _dirty = true;
    reDraw();
}

void currentView::toPause() {
    _state = MP3Player::PAUSED;
    reDraw();
}

void currentView::toResume() {
    _state = MP3Player::PLAYING;
    reDraw();
}

void currentView::toStop() {
    _state = MP3Player::STOPPED;
    reDraw();
}


currentView::currentView(Frame frame) : View(frame) {
    _state = MP3Player::STOPPED;
    _song = NULL;
}

currentView::~currentView() {
    _song = NULL;
}

void currentView::reDraw() {
    if (_dirty) View::reDraw();

    // Frame for drawing play, pause, or stop icon...
    Frame icon_frame = Frame { _frame.x + 5, _frame.y + 6, 8, 16 };

    switch (_state) {
        case MP3Player::STOPPED: {
            LCD_display.Bitmap16Draw(icon_frame, stop_icon, RED, _background_Color);
        } break;

        case MP3Player::PLAYING: {
            LCD_display.Bitmap16Draw(icon_frame, play_icon, RED, _background_Color);

            uint8_t length = strlen(_song->getTitle());
            if (length > 20) length = 20;

            char str[length];
            if (length > 21) {
                length = 21;
                strncpy(str, _song->getTitle(), length);
                for (uint8_t i = length-3; i < length; i++)
                    str[i] = '.';

            } else {
                strncpy(str, _song->getTitle(), length);
            }

            for (uint8_t i = 0; i < length; i++) {
                const uint8_t padding = _frame.x + 20;
                const uint8_t charSpacing = (i * 1);
                const uint8_t charPos = (i * 5);

                const uint8_t x = padding + charSpacing + charPos;
                const uint8_t y = _frame.y + 5;

                const uint8_t *bitmap = ascii_characters[int(str[i])];

                LCD_display.fontDraw(coordinate_2D{x, y}, bitmap, RED, _background_Color);
            }


            for (uint8_t i = 0; i < strlen(_song->getArtist()); i++) {
                const uint8_t padding = _frame.x + 20;
                const uint8_t charSpacing = (i * 1);
                const uint8_t charPos = (i * 5);

                const uint8_t x = padding + charSpacing + charPos;
                const uint8_t y = _frame.y + 15;

                const uint8_t *bitmap = ascii_characters[int(_song->getArtist()[i])];

                LCD_display.fontDraw(coordinate_2D{x, y}, bitmap, RED, _background_Color);
            }

        } break;

        case MP3Player::PAUSED: {
            LCD_display.Bitmap16Draw(icon_frame, paused_icon, RED, _background_Color);
        } break;
    }

    _dirty = false;
}


