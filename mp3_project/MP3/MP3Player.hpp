/*
 * MP3Player.h
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#ifndef MP3PLAYER_HPP_
#define MP3PLAYER_HPP_

#include <MP3/Drivers/VS1053B.hpp>
#include <MP3/MP3FileStructure.hpp>
#include <MP3/UI/cellView.hpp>
#include <vector>
#include <memory>

#include "FreeRTOS.h"
#include "queue.h"
#include "scheduler_task.hpp"


// class notify view redeclared to re-implement the functions (Abstract class)
class notifyView {
public:
    virtual void startPlaying(MP3FileStructure *mp3) = 0;
    virtual void toPause() = 0;
    virtual void toResume() = 0;
    virtual void toStop() = 0;
};


class MP3Player: protected virtual cellDataSource, cellNotify {

private:

    class MP3BufferTask;
    class MP3StreamTask;

public:

    typedef enum {
        STOPPED = 0,
        PLAYING,
        PAUSED
    } SongState;

    static MP3Player& sharedInstance();
    virtual ~MP3Player();
    void setNotify(notifyView *notify);
    MP3FileStructure* songAt(uint32_t index) { return &_song_list.at(index); }
    SongState state();
    void queue(MP3FileStructure *song_file, uint32_t index);
    void pause();
    void resume();
    void playPrevious();
    void playNext();
    void increaseVolume();
    void decreaseVolume();

protected:

    typedef enum {
        PREVIOUS_TRACK = 0,
        NEXT_TRACK,
    } QueueOption;

    static MP3Player *instance;
    std::vector<MP3FileStructure> _song_list;
    VS1053B &_decoder = VS1053B::sharedInstance();
    notifyView *_notify;
    MP3BufferTask *bufferTask;
    QueueHandle_t _song_queue;
    SemaphoreHandle_t _play_mutex;
    SongState _State;
    uint8_t _volume;
    uint32_t _Song_index;

    MP3Player();
    void getAllSongsFromSD();
    inline void setVolume(uint8_t percentage);
    virtual inline uint32_t numOfSongs() const final;
    virtual inline void updatePlaylist(Cell &cell, uint32_t index) final;
    virtual inline void playlistDidSelectEntryAt(const cellView *tableView, Cell &cell, uint32_t index) final;

};

// Read and buffer song data from SD and queue it for the decoder to read
class MP3Player::MP3BufferTask final: public scheduler_task {

protected:
    QueueHandle_t _song_queue;
    QueueHandle_t _stream_queue;

public:

    MP3Player *_player;
    bool _newSongSelected;

    MP3BufferTask(uint8_t priority, QueueHandle_t songQueue, QueueHandle_t streamQueue) : scheduler_task("buffer_song", 1024 * 3, priority), _song_queue(songQueue), _stream_queue(streamQueue) { };
    bool run(void *);
};

// class to take buffered song data (dequeue) and send it to the audio decoder (VS1053B) to decode
class MP3Player::MP3StreamTask final: public scheduler_task {

protected:
    QueueHandle_t _stream_queue;

public:
    MP3StreamTask(uint8_t priority, QueueHandle_t queue) : scheduler_task("stream_song", 1024 * 2, priority), _stream_queue(queue) { };
    bool run(void *);
};

#endif /* MUSICPLAYER_H_ */
