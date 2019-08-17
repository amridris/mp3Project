/*
 * MP3FileStructrue.cpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#include <MP3/Drivers/SPI.hpp>
#include <MP3/MP3FileStructure.hpp>
#include <stdio.h>
#include "string.h"
#include "ff.h"
#include "storage.hpp"


// removes extra spaces from a string
inline void removeSpaces(char *string) {
    uint32_t index = 0;
    uint32_t i = 0;
    while (string[i] != '\0' || i < strlen(string)) {
        if (string[i] != ' ') index = i;
        i++;
    }
    string[index+1] = '\0';
}


// Helper function to copy a ID3v1 tag string
inline void copyAndRemoveStringSpaces(char *dst, const char *src, uint32_t len) {
    strncpy(dst,  (char *)src, len);
    dst[len] = '\0';
    removeSpaces(dst);
}

MP3FileStructure::MP3FileStructure(char *path, uint32_t fileSize) {
    _path = path;
    _name = NULL;
    _file_size = fileSize;
}

MP3FileStructure::~MP3FileStructure() {
    _path = NULL;
}

void MP3FileStructure::getSongInfo() {
    if (xSemaphoreTake(SPI::spi_Mutex[SPI::SSP1], portMAX_DELAY)) {

        const uint8_t id3Size = 128;
        uint8_t data[id3Size];

        Storage::read(_path, data, id3Size, _file_size - id3Size);

        xSemaphoreGive(SPI::spi_Mutex[SPI::SSP1]);

        const uint8_t title_index    = 3;
        const uint8_t artist_index   = 33;
        const uint8_t album_index    = 63;
        const uint8_t year_index     = 93;
        const uint8_t comment_index  = 97;
        const uint8_t genre_index    = 127;

        copyAndRemoveStringSpaces(_header,  (char *)data, 3);
        copyAndRemoveStringSpaces(_title,   (char *)data + title_index, 30);
        copyAndRemoveStringSpaces(_artist,  (char *)data + artist_index, 30);
        copyAndRemoveStringSpaces(_album,   (char *)data + album_index, 30);
        copyAndRemoveStringSpaces(_comment, (char *)data + comment_index, 30);

        _year |= data[year_index]   << 24;
        _year |= data[year_index+1] << 16;
        _year |= data[year_index+2] << 8;
        _year |= data[year_index+3] << 0;

        _genre = data[genre_index];
    }
}

void MP3FileStructure::setName(char *name) { _name = name;    }

char *MP3FileStructure::getPath()          { return _path;    }
uint32_t MP3FileStructure::getFileSize()   { return _file_size; }
char* MP3FileStructure::getTitle()         { return strlen(_title) == 0 || strlen(_title) == 31 ? _name : _title;    }
char* MP3FileStructure::getArtist()        { return _artist;   }
