/*
 * CellView.cpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#include <MP3/Drivers/ST7735R.hpp>
#include <MP3/UI/cellView.hpp>
#include <string.h>
#include "utilities.h"


cellView::cellView(Frame frame) : View(frame) {
    _cells = NULL;
    _dataSource = NULL;
    _notify = NULL;
    _rows = 0;
    _row_height = 0;
    _cursor_position = 0;
    _index_start = 0;
    _index_end = 0;
    _item_count = 0;
}

cellView::~cellView() {
    _dataSource = NULL;
    _notify = NULL;

    delete _cells;
    _cells = NULL;
}

void cellView::updatePlaylistIfNeeded() {

    if (_dirty || (_cells == NULL && _rows > 0) || _item_count != (*_dataSource).numOfSongs()) {

        _item_count = (*_dataSource).numOfSongs();

        _cells = new Cell[_rows];
        for (uint8_t i = 0; i < _rows; i++) {
            uint8_t y = i * _row_height + (!!i * 1);
            _cells[i] = Cell(Frame { 0, y, _frame.width, _row_height });
        }

        selectCellAt(0);


        _cursor_position = 0;
        _index_start = 0;
        _index_end = _rows - 1;
    }
}

void cellView::reDraw() {

    updatePlaylistIfNeeded();

    View::reDraw();

    for (uint8_t row = 0; row < _rows; row++) {
        uint32_t index = _index_start + row;
        if (index >= _item_count) break;
        (*_dataSource).updatePlaylist(rowCell(row), index);
        reDraw(row);
    }
}

void cellView::reDraw(uint8_t row)                                  { rowCell(row).reDraw(); }
void cellView::setDataSource(cellDataSource* const dataSource) { _dataSource = dataSource; }
void cellView::setNotify(cellNotify* const delegate)       { _notify = delegate; }
void cellView::setValueOfRows(uint8_t rows)                        { _rows = rows; }
void cellView::setRowHeight(uint8_t height)                         { _row_height = height; }
void cellView::selectCurrentRow()                                   { (*_notify).playlistDidSelectEntryAt(this, rowCell(_cursor_position), _index_start + _cursor_position); }
Cell& cellView::rowCell(uint8_t row)                    { return _cells[row]; }

void cellView::selectCellAt(uint8_t row) {
    rowCell(row).setHighlighted(true);
    reDraw(row);
}

void cellView::unselectCellAt(uint8_t row) {
    rowCell(row).setHighlighted(false);
    reDraw(row);
}

void cellView::setCursorLocation(CursorDirection direction) {
    const uint32_t prevPos = _cursor_position;

    switch (direction) {

        case DIRECTION_UP: {

            if (_cursor_position != 0) {
                _cursor_position--;
                unselectCellAt(prevPos);
                selectCellAt(_cursor_position);

            } else {
                if( _index_start == 0) return;
                _index_start--;
                _index_end--;
                reDraw();
            }
        } break;

        case DIRECTION_DOWN: {
            if (_cursor_position == _item_count - 1) return;


            if (_cursor_position < _rows-1) {
                _cursor_position++;
                unselectCellAt(prevPos);
                selectCellAt(_cursor_position);

            } else {
                if (_index_end == _item_count - 1) return; // reached the end
                _index_start++;
                _index_end++;
                reDraw();
            }
        } break;
    }
}

Cell::Cell() : View(Frame{0,0,0,0}) {
    init();
}

Cell::Cell(Frame frame) : View(frame) {
    init();
}

void Cell::init() {
    _text = NULL;

    _selected = false;
    _selected_color = BLACK;
}

void Cell::setText(char *text) {
    _text = text;
    reDraw();
}

void Cell::setHighlighted(bool highlighted) { _selected = highlighted; }
void Cell::setHighlightedColor(Color color) { _selected_color = color;  }

void Cell::reDraw() {

    if (!_dirty) return;

    uint8_t selIcon[] = {
            0b11111110,
            0b01111100,
            0b00111000,
            0b00010000
    };

    uint8_t unselIcon[] = { 0, 0, 0, 0 };

    const uint8_t kIconWidth = 4;
    const uint8_t kIconHeight = 8;
    Frame selFrame = Frame { _frame.x + 2, _frame.y, kIconWidth, kIconHeight };

    uint8_t len = strlen(_text);
    char str[len];

    if (len > 21) {
        len = 21;
        strncpy(str, _text, len);
        for (uint8_t i = len-3; i < len; i++)
            str[i] = '.';

    } else {
        strncpy(str, _text, len);
    }

    if   (_selected) LCD_display.bitmap8Draw(selFrame, selIcon,   BLACK, _background_Color);
    else                LCD_display.bitmap8Draw(selFrame, unselIcon, BLACK, _background_Color);


    for (uint8_t i = 0; i < len; i++) {
        const uint8_t padding = _frame.x + 8;
        const uint8_t charSpacing = (i * 1);
        const uint8_t charPos = (i * 5);

        const uint8_t x = padding + charSpacing + charPos;
        const uint8_t y = _frame.y + 1;

        const uint8_t *bitmap = ascii_characters[int(str[i])];

        LCD_display.fontDraw(coordinate_2D{x, y}, bitmap, BLACK, _background_Color);
    }
}
