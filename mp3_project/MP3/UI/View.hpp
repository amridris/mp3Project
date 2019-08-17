/*
 * View.hpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#ifndef VIEW_HPP_
#define VIEW_HPP_

#include <MP3/graphic.h>


// View base class for displaying on the lcd
class View {

public:

    View(Frame frame);
    virtual ~View();

    void setFrame(Frame frame);
    void setOrigin(coordinate_2D origin);
    void setSize(size_2D size);
    void setBackgroundColor(Color c);
    virtual void draw();
    virtual void reDraw();
    virtual void reDrawLCDWithBackground(Color *color);

protected:

    Frame _frame;
    Color _background_Color;

    bool _dirty;
};

#endif /* VIEW_HPP_ */
