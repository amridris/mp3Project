/*
 * UI.hpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#ifndef UI_HPP_
#define UI_HPP_

#include <MP3/UI/cellView.hpp>
#include <vector>

#include "FreeRTOS.h"
#include "queue.h"
#include "scheduler_task.hpp"


class LabGPIO;
class currentView;

class UI: public scheduler_task {

protected:

    std::vector<View *> _Subviews;
    LabGPIO *_buttons;

    cellView *_song_menu;
    currentView *_now_playing;

public:

    UI(uint8_t priority) : scheduler_task("ui", 1024, priority) { };
    virtual ~UI();

    bool init();
    bool run(void *);

    void addView(View *view);
    void updateViews();

};

#endif /* UI_HPP_ */
