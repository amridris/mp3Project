/*
 * graphic.c
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#include <MP3/graphic.h>

Color ColorMake(uint8_t r, uint8_t g, uint8_t b) {
    Color c;
    c.r = r;
    c.g = g;
    c.b = g;
    return c;
}

coordinate_2D Point2DMake(int16_t x, int16_t y) {
    coordinate_2D p;
    p.x = x;
    p.y = y;
    return p;
}

