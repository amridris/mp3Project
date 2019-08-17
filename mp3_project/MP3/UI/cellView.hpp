/*
 * CellView.hpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#ifndef CELLVIEW_HPP_
#define CELLVIEW_HPP_

#include "View.hpp"

class cellView;
class Cell;

class cellDataSource {
public:

    virtual inline uint32_t numOfSongs() const = 0;
    virtual inline void updatePlaylist(Cell &cell, uint32_t index) = 0;
};

class cellNotify {
public:


    virtual inline void playlistDidSelectEntryAt(const cellView *tableView, Cell &cell, uint32_t index) = 0;
};



class cellView: public View {

public:

    typedef enum {
        DIRECTION_UP   = 0,
        DIRECTION_DOWN ,
    } CursorDirection;

    cellView(Frame frame);
    virtual ~cellView();

    void setDataSource(cellDataSource* const data_source);
    void setNotify(cellNotify* const notify);
    void reDraw() override;
    void updatePlaylistIfNeeded();
    void setValueOfRows(uint8_t rows);
    void setRowHeight(uint8_t height);
    void selectCurrentRow();
    void setCursorLocation(CursorDirection direction);

protected:

    cellDataSource *_dataSource;
    cellNotify *_notify;
    Cell *_cells;
    uint8_t _rows;
    uint8_t _row_height;
    uint32_t _item_count;
    uint8_t _cursor_position;
    uint8_t _index_start;
    uint8_t _index_end;
    Cell& rowCell(uint8_t row);
    inline void selectCellAt(uint8_t row);
    inline void unselectCellAt(uint8_t row);
    void reDraw(uint8_t row);
};

class Cell: public View {

protected:

    char *_text;
    bool _selected;
    Color _selected_color;
    void init();

public:

    Cell();
    Cell(Frame frame);

    void setText(char *text);
    char* getText() const { return _text; };

    void setHighlighted(bool highlighted);
    void setHighlightedColor(Color color);

    void reDraw() override;
};

#endif /* CELLVIEW_HPP_ */
