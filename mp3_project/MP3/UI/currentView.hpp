/*
 *  currentView.hpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#ifndef CURRENTVIEW_HPP_
#define CURRENTVIEW_HPP_

#include <MP3/MP3Player.hpp>
#include <MP3/UI/View.hpp>

class MP3FileStructure;


/*
 * current view class: displays the song title and artist of the song in a small section of the screen
 * notifyView class: help detect when user selects play or pause and updates view (frame)
 */
class currentView: public View, protected virtual notifyView {

protected:

    /// Current song that is being played and displayed.
    MP3FileStructure *_song;
    MP3Player::SongState _state;

    // updates the current play view of the song info
    virtual void startPlaying(MP3FileStructure *new_song) final;
    // puts the paused icon in the current view
    virtual void toPause() final;
    // puts the play icon in the current view
    virtual void toResume() final;
    /// puts the stop icon in the current view
    virtual void toStop() final;

public:

    currentView(Frame frame);
    virtual ~currentView();
    virtual void reDraw() override;
};

#endif /* CURRENTVIEW_HPP_ */
