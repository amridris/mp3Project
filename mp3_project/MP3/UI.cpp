/*
 * UI.cpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#include <stdio.h>
#include <string.h>

#include "Drivers/LabGPIO.hpp"
#include <MP3/Drivers/ST7735R.hpp>
#include <MP3/MP3Player.hpp>
#include <MP3/UI/currentView.hpp>
#include <MP3/UI.hpp>

typedef enum {
    PLAY = 0,
    PLAY_PREV,
    PLAY_NEXT,
    VOL_UP,
    VOL_DOWN,

    MENU_SEL,
    MENU_UP,
    MENU_DOWN,

    BUTTON_COUNT
} Button_Map;

UI::~UI() {
    delete _song_menu;   _song_menu   = NULL;
    delete _now_playing; _now_playing = NULL;
    _Subviews.empty();
}

// Member Functions

bool UI::init() {

    MP3Player &player = MP3Player::sharedInstance();

    const uint8_t menu_height    = 100;
    const uint8_t row_menu_height = 10;
    const uint8_t num_rows       = 10;

    // Initialize playlist menu...
    _song_menu = new cellView(Frame { 0, 0, LCD_WIDTH, menu_height });
    _song_menu->setNotify((cellNotify *) &player);
    _song_menu->setDataSource((cellDataSource *) &player);
    _song_menu->setRowHeight(row_menu_height);
    _song_menu->setValueOfRows(num_rows);
    addView(_song_menu);

    // Initialize song info view...
    const uint8_t now_playing_height = LCD_HEIGHT - menu_height;

    _now_playing = new currentView(Frame {0, menu_height, LCD_WIDTH, now_playing_height});
    _now_playing->setBackgroundColor(GREEN);
    player.setNotify((notifyView *) _now_playing);
    addView(_now_playing);

    return true;
}

bool UI::run(void *) {

    updateViews();



    LabGPIO *play_button     = new LabGPIO(1, 9);     // play button P1.9
    LabGPIO *prev_button = new LabGPIO(1, 10);    // Previous button P1.10
    LabGPIO *next_button = new LabGPIO(1, 14);    // Next button P1.14

    LabGPIO *menuSelect_button  = new LabGPIO(1, 15);    // Menu Select button P1.15
    LabGPIO *menuUp_button   = new LabGPIO(1, 22);    // Menu up button P1.22
    LabGPIO *menuDown_button = new LabGPIO(1, 23);    // Menu down button P1.23

    LabGPIO *volUp_button    = new LabGPIO(1, 28);    // Volume up button P1.28
    LabGPIO *VolDown_button  = new LabGPIO(1, 29);    // Volume down button P1.29

    MP3Player &player = MP3Player::sharedInstance();

    while (1) {

        if      (play_button->getLevel())     { player.state() == MP3Player::PLAYING ? player.pause() : player.resume(); vTaskDelay(500); }
        else if (prev_button->getLevel()) { player.playPrevious(); vTaskDelay(500); }
         if (next_button->getLevel()) { player.playNext();     vTaskDelay(500); }

        else if (menuSelect_button->getLevel())  { _song_menu->selectCurrentRow(); }

        else if (menuUp_button->getLevel())   { _song_menu->setCursorLocation(cellView::DIRECTION_UP); }
        else if (menuDown_button->getLevel()) { _song_menu->setCursorLocation(cellView::DIRECTION_DOWN); }

        else if (volUp_button->getLevel())    { player.increaseVolume(); }
        else if (VolDown_button->getLevel())  { player.decreaseVolume(); }

        vTaskDelay(50);
    }

    return true;
}

void UI::addView(View *view) {
    _Subviews.push_back(view);
}

void UI::updateViews() {
    for (View *view : _Subviews) {
        view->reDraw();
    }
}
