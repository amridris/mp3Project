/*
 * MP3FileStructure.hpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#ifndef MP3FILESTRUCTURE_HPP_
#define MP3FILESTRUCTURE_HPP_

#include "LPC17xx.h"

class MP3FileStructure {

protected:

    char *_path;
    char *_name;
    uint32_t _file_size;

    // ID3v1 MP3 data structure
    // Information taken from: https://en.wikipedia.org/wiki/ID3
    
    char _header[4];     // 3  bytes - "TAG"
    char _title[31];     // 30 bytes - 30 characters of the title
    char _artist[31];    // 30 bytes - 30 characters of the artist name
    char _album[31];     // 30 bytes - 30 characters of the album name
    uint32_t _year;      // 4  bytes - A four-digit year
    char _comment[31];   // 28 or 30 bytes - The Comment
    uint8_t _genre;      // 1  byte - Index of the track's genre (0-255).. https://en.wikipedia.org/wiki/List_of_ID3v1_Genres

public:

    MP3FileStructure(char *song_path, uint32_t fileSize);
    ~MP3FileStructure();
    void getSongInfo();
    void setName(char *name);
    char* getPath();
    uint32_t getFileSize();
    char* getTitle();
    char* getArtist();
};

#endif /* MP3FILESTRUCTURE_HPP_ */
