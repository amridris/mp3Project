/*
 * VS1053B.cpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#include <MP3/Drivers/VS1053B.hpp>
#include <stddef.h>
#include "utilities.h"
#include "LabGPIO.hpp"

#define SCI_WRITE       (0x02)
#define SCI_READ        (0x03)

#define RESYNC_BYTES    (0x1E29)
#define END_FILL_BYTES  (0x1E06)

#define SCI_SOFT_RESET   ( SDINEW | DECODER_RESET )
#define SCI_MODE_DEFAULT ( LINE1 | SDINEW )
#define SCI_MODE_STREAM  ( LINE1 | SDINEW | STREAM )
#define SCI_MODE_CANCEL  ( LINE1 | SDINEW | DECODER_CANCEL )

const uint16_t VS1053B::sample_LUT[4][4] = {
    { 11025, 11025, 22050, 44100 },
    { 12000, 12000, 24000, 48000 },
    {  8000,  8000, 16000, 32000 },
    {     0,     0,     0,     0 }
};

VS1053B* VS1053B::instance = NULL;

VS1053B& VS1053B::sharedInstance() {
    if (instance == NULL) instance = new VS1053B();
    return *instance;
}

VS1053B::VS1053B() {
    init(SSP0, BIT8, FRAMEMODE_SPI, DIV_1);

    SSPn->CPSR = 16;

    DREQ  = new LabGPIO(2, 4, false, false);    // DREQ = pin 2.4
    RESET = new LabGPIO(2, 3, true,  true);     // Reset = pin 2.3
    CS    = new LabGPIO(2, 2, true,  true);     // CS = pin 2.2
    SDCS  = new LabGPIO(2, 1, true,  true);     // SDCS = pin 2.1
    XDCS  = new LabGPIO(2, 0, true,  true);     // XDCS  = pin 2.0

    reset();

    writeREG(SCI_MODE,   SCI_MODE_DEFAULT);
    writeREG(SCI_CLOCKF, MULT_4x);
    setVol(255);
}

VS1053B::~VS1053B() {
    delete DREQ;  DREQ  = NULL;
    delete RESET; RESET = NULL;
    delete SDCS;  SDCS  = NULL;
    delete XDCS;  XDCS  = NULL;
}

void VS1053B::reset() {
    RESET->setHigh();
    RESET->setLow();
    delay_ms(1);
    RESET->setHigh();

    while(!isReady());
}

void VS1053B::softReset() {
    SCIwrite(SCI_MODE, SCI_SOFT_RESET);
}

bool VS1053B::isReady() {
    return (*DREQ).getLevel();
}

uint16_t VS1053B::SCIread(uint8_t addr) {
    uint16_t data;
    while(!isReady());
    CS_select();
    {
        SSPn->CPSR = 16;
        transfer(SCI_READ);
        transfer(addr);
        data = wordTransfer(0x00);
    }
    CS_deselect();
    return data;
}

void VS1053B::SCIwrite(uint8_t addr, uint16_t data) {
    writeSCI(addr, &data, 1);
}

void VS1053B::writeSCI(uint8_t addr, uint16_t *data, uint32_t len) {
    while(!isReady()) delay_ms(0.1);
    CS_select();
    {
        SSPn->CPSR = 16;
        transfer(SCI_WRITE);
        transfer(addr);
        for (uint32_t i = 0; i < len; i++) {
            wordTransfer(data[i]);
        }
    }
    CS_deselect();
}

void VS1053B::writeSDI(uint8_t data) {
    writeSDI(&data, 1);
}

void VS1053B::writeSDI(uint8_t *data, uint32_t len) {
    while(!isReady());
    if (xSemaphoreTake(spi_Mutex[Perip], portMAX_DELAY)) {
        XDCS->setLow();
        {
            SSPn->CPSR = 4;
            transfer(data, len);
        }
        XDCS->setHigh();
        xSemaphoreGive(spi_Mutex[Perip]);
    }
}

uint16_t VS1053B::readREG(uint8_t addr) {
    return SCIread(addr);
}

void VS1053B::writeREG(uint8_t addr, uint16_t reg) {
    SCIwrite(addr, reg);
}

void VS1053B::setVol(uint8_t volume) {
    uint8_t vol = 0xFF - volume;

    SCIwrite(SCI_VOL, ((vol << 8) | vol));
}


void VS1053B::resumePlay() {
    writeREG(SCI_MODE, SCI_MODE_DEFAULT);
    writeREG(SCI_AUDATA, (D44100 | STEREO));
}

void VS1053B::playbackStop() {
    writeREG(SCI_MODE, SCI_MODE_CANCEL);


    while (!isReady() || (readREG(SCI_MODE) & DECODER_CANCEL));
}

void VS1053B::decoderBuffer(uint8_t *songData, uint32_t len) {
    writeSDI(songData, len);
}


void VS1053B::clearDecodeTime() {
    SCIwrite(SCI_DECODE_TIME, 0x00);
    SCIwrite(SCI_DECODE_TIME, 0x00);
}

VS1053B::HeaderData VS1053B::getHeaderDAT() {
    HeaderData hdat;
    hdat.bytes = (readREG(SCI_HDAT1) << 16) | readREG(SCI_HDAT0);
    return hdat;
}

void VS1053B::Playback() {
    writeREG(SCI_MODE, SCI_MODE_DEFAULT);
    writeREG(SCI_AUDATA, (D44100 | STEREO));

    SCIwrite(SCI_WRAMADDR, 0x1E29);
    SCIwrite(SCI_WRAM, 0);

    clearDecodeTime();
}

