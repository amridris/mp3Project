/*
 * MP3Player.cpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#include <MP3/MP3FileStructure.hpp>
#include <MP3/MP3Player.hpp>
#include <stdio.h>
#include "string.h"
#include "ff.h"
#include "storage.hpp"


#define STREAM_QUEUE_SIZE        (3)
#define STREAM_QUEUE_BUFFER_SIZE (1024)

#define SONG_QUEUE_SIZE          (2)

MP3Player* MP3Player::instance = NULL;

MP3Player& MP3Player::sharedInstance() {
    if (instance == NULL) instance = new MP3Player();
    return *instance;
}

MP3Player::MP3Player() {
    _notify = NULL;
    _play_mutex   = xSemaphoreCreateMutex();
    _State = STOPPED;
    _volume = 80;
    setVolume(_volume);

    getAllSongsFromSD();

    _Song_index = 0;

    QueueHandle_t streamQueue = xQueueCreate(STREAM_QUEUE_SIZE, sizeof(uint8_t) * STREAM_QUEUE_BUFFER_SIZE);
    _song_queue   = xQueueCreate(SONG_QUEUE_SIZE,   sizeof(MP3FileStructure));

    bufferTask = new MP3BufferTask(PRIORITY_LOW, _song_queue, streamQueue);
    bufferTask->_player = this;
    scheduler_add_task(bufferTask);
    scheduler_add_task(new MP3StreamTask(PRIORITY_LOW, streamQueue));
}

MP3Player::~MP3Player() {
    _notify = NULL;
}

void MP3Player::setNotify(notifyView *notify) {
    _notify = notify;
}

void MP3Player::getAllSongsFromSD() {
    _song_list.empty();

    const char dirPath[] = "1:";
    DIR directory;
    static FILINFO fileInfo;

#if _USE_LFN
    char lfnBuffer[_MAX_LFN];
    fileInfo.lfsize = _MAX_LFN-1;
    fileInfo.lfname = lfnBuffer;
#endif

    const char *mp3[] = { ".mp3", ".MP3" };
    const char *ext   = strrchr(fileInfo.fname,'.');

    FRESULT res = f_opendir(&directory, dirPath);

    if (res == FR_OK) {

        while (1) {
            fileInfo.lfsize = _MAX_LFN-1;
            fileInfo.lfname = lfnBuffer;

            res = f_readdir(&directory, &fileInfo);

            if (res != FR_OK || fileInfo.fname[0] == 0) break;

            if (!(fileInfo.fattrib & AM_DIR) && (strcmp(ext, mp3[0]) || strcmp(ext, mp3[1]))) {

                const char *fullName = fileInfo.lfname[0] == 0 ? fileInfo.fname : fileInfo.lfname;


                uint32_t len = strlen(dirPath) + strlen(fullName) + 1;
                char *path = new char[len];
                strcpy(path, dirPath);
                strcat(path, fullName);
                path[len-1] = '\0';


                len = strlen(fullName) - strlen(mp3[0]) + 1;
                char *name = new char[len];
                strncpy(name, fullName, len);
                name[len-1] = '\0';

                MP3FileStructure file = MP3FileStructure(path, fileInfo.fsize);
                file.setName(name);
                file.getSongInfo();
                _song_list.push_back(file);
            }
        }
        f_closedir(&directory);
    }
}

MP3Player::SongState MP3Player::state() {
    return _State;
}

void MP3Player::queue(MP3FileStructure *song, uint32_t index) {
    _decoder.Playback();

    _State = PLAYING;

    if (xSemaphoreTake(SPI::spi_Mutex[SPI::SSP1], portMAX_DELAY)) {
        xQueueReset(_song_queue);
        _Song_index = index;

        bufferTask->_newSongSelected = true;
        xSemaphoreGive(SPI::spi_Mutex[SPI::SSP1]);
        _notify->startPlaying(song);
        xQueueSend(_song_queue, &song, portMAX_DELAY);
    }
}

void MP3Player::pause() {
    _notify->toPause();
    _State = PAUSED;
}

void MP3Player::resume() {
    _decoder.resumePlay();
    _notify->toResume();
    _State = PLAYING;
}

void MP3Player::playPrevious() {
    if (!xSemaphoreTake(_play_mutex, 10)) return;

    uint32_t index = _Song_index;
    if (index == 0) index = _song_list.size() - 1;
    else            index -= 1;

    queue(&_song_list.at(index), index);
    xSemaphoreGive(_play_mutex);
}

void MP3Player::playNext() {
    if (!xSemaphoreTake(_play_mutex, 10)) return;

    uint32_t index = _Song_index;
    if (index == _song_list.size() - 1) index = 0;
    else                               index += 1;

    queue(&_song_list.at(index), index);
    xSemaphoreGive(_play_mutex);
}

void MP3Player::increaseVolume() {
    _volume += 5;
    setVolume(_volume);
}

void MP3Player::decreaseVolume() {
    _volume -= 5;
    setVolume(_volume);
}

inline void MP3Player::setVolume(uint8_t percentage) {

    if      (_volume < 0)   _volume = 0;
    else if (_volume > 100) _volume = 100;
    _decoder.setVol( (_volume/100.0) * VS1053B_MAX_VOL );
}



inline uint32_t MP3Player::numOfSongs() const {
    return _song_list.size();
}

inline void MP3Player::updatePlaylist(Cell &cell, uint32_t index) {
    cell.setText(_song_list[index].getTitle());
}

inline void MP3Player::playlistDidSelectEntryAt(const cellView *tableView, Cell &cell, uint32_t index) {
    queue(&_song_list[index], index);
}



bool MP3Player::MP3BufferTask::run(void *) {

    const uint32_t bufferSize = STREAM_QUEUE_BUFFER_SIZE;

    while (1) {
        MP3FileStructure *song = NULL;
        if (xQueueReceive(_song_queue, &song, portMAX_DELAY)) {

            const uint32_t fileSize = song->getFileSize();
            const char *path = song->getPath();

            _newSongSelected = false;

            for (uint32_t i = 0; i < fileSize/bufferSize; i++) {
                uint8_t data[bufferSize] = { 0 };

                while (_player->state() == MP3Player::PAUSED) vTaskDelay(1);

                if (_newSongSelected || _player->state() == MP3Player::STOPPED) goto ENDSTREAM;

                if (xSemaphoreTake(SPI::spi_Mutex[SPI::SSP1], portMAX_DELAY)) {
                    Storage::read(path, data, bufferSize, i * bufferSize);
                    xSemaphoreGive(SPI::spi_Mutex[SPI::SSP1]);
                    xQueueSend(_stream_queue, data, portMAX_DELAY);
                }
                vTaskDelay(15);
            }

            _player->playNext();

            ENDSTREAM:;
        }
    }

    return true;
}


bool MP3Player::MP3StreamTask::run(void *) {
    const uint32_t size     = STREAM_QUEUE_BUFFER_SIZE;
    const uint32_t buffSize = VS1053B_BUFFER_SIZE;

    while (1) {
        uint8_t data[size] = { 0 };
        if (xQueueReceive(_stream_queue, data, portMAX_DELAY)) {

            for (uint32_t j = 0; j < size/buffSize; j++){
                MP3.decoderBuffer(data + (j*buffSize), buffSize);
            }
            vTaskDelay(15);
        }
    }

    return true;
}
