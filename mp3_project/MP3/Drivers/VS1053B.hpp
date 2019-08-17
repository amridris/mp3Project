/*
* VS1053B.hpp
*
*      Created on: December 5, 2018
*      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
*/

#ifndef VS1053B_HPP_
#define VS1053B_HPP_

#include "SPI.hpp"

#define MP3 VS1053B::sharedInstance()

#define VS1053B_BLOCK_SIZE  (512)
#define VS1053B_BUFFER_SIZE (32)

#define VS1053B_MAX_VOL     (0xFF)

// Device SCI Register Addresses
#define SCI_MODE            (0x0)
#define SCI_STATUS          (0x1)
#define SCI_CLOCKF          (0x3)
#define SCI_DECODE_TIME     (0x4)
#define SCI_AUDATA          (0x5)
#define SCI_WRAM            (0x6)
#define SCI_WRAMADDR        (0x7)
#define SCI_HDAT0           (0x8)
#define SCI_HDAT1           (0x9)
#define SCI_VOL             (0xB)


class VS1053B: public SPI {

public:

    typedef union {
        uint32_t bytes;
        struct {

            uint8_t  emphasis:       2;
            uint8_t  original:       1;
            uint8_t  copyright:      1;
            uint8_t  extension:      2;
            uint8_t  mode:           2;
            uint8_t  private_bit:    1;
            uint8_t  pad_bit:        1;
            uint8_t  samplerate:     2;
            uint8_t  bitrate:        4;

            uint8_t  protected_bit:  1;
            uint8_t  layer:          2;
            uint8_t  id:             2;
            uint16_t syncword:      11;
        } __attribute__((packed));
    } HeaderData;

    static const uint16_t sample_LUT[4][4];

    static VS1053B& sharedInstance();

    virtual ~VS1053B();

    // Hardware reset
    void reset();

    // Software reset
    void softReset();

    // checks to see DREQ signal and see if hardware is busy
    inline bool isReady();


    HeaderData getHeaderDAT();

    // set hardware volume
    void setVol(uint8_t volume);

    // music features
    void Playback();
    void resumePlay();
    void playbackStop();

    // send data to the decoder buffer
    void decoderBuffer(uint8_t *song_data, uint32_t length);


protected:

    typedef enum {
        // Diff bit. 0 for normal audio, 1 for left channel invert
        DIFF          = (1 << 0),
        // MPEG layer
        LAYER12       = (1 << 1),
        // Software reset
        DECODER_RESET         = (1 << 2),
        // "cancel" decoding
        DECODER_CANCEL        = (1 << 3),
        // Enable or disable Low earspeaker setting
        EARSPEAKER_LO = (1 << 4),
        TESTS         = (1 << 5),
        //stream enable bit
        STREAM        = (1 << 6),
        // enable or disable ear speaker high setting
        EARSPEAKER_HI = (1 << 7),
        //D clock active edge... 0 = rising and 1 = falling
        DACT          = (1 << 8),
        //SDI bit ordering. 0 = MSB first and 1 for LSB first
        SDIORD        = (1 << 9),
        SDISHARE      = (1 << 10),
        SDINEW        = (1 << 11),
        ADPCM         = (1 << 12),
        LINE1         = (1 << 14),
        // Decoder input clock range. 0 = 12-13 MHZ and 1 = 24-26 MHZ
        CLK_RANGE     = (1 << 15)
    } MODE_OPTIONS;

    typedef enum {
        REFERENCE_SEL = (1 << 0),
        AD_CLOCK      = (1 << 1),
        APDOWN1       = (1 << 2),
        APDOWN2       = (1 << 3),
        VER           = (1 << 4),
        VCM_DISABLE   = (1 << 10),
        VCM_OVERLOAD  = (1 << 11),
        SWING         = (1 << 12),
        NO_JUMP       = (1 << 15)
    } STATUS_OPTIONS;

    typedef enum {
        MULT_1x   = 0x0000,
        MULT_2x   = 0x2000,
        MULT_25x = 0x4000,
        MULT_3x   = 0x6000,
        MULT_35x = 0x8000,
        MULT_4x   = 0xA000
    } CLOCKF_OPTIONS;

    typedef enum {
        STEREO = (1 << 0),
        D44100  = 44100,
    } DATA_OPTIONS;

    static VS1053B *instance;

    // reset signal to the decoder
    LabGPIO *RESET;
    // dreq pin to the decoder
    LabGPIO *DREQ;
    // xdcs pin to the decoder
    LabGPIO *XDCS;
    // sdcs pin to the decoder
    LabGPIO *SDCS;

    VS1053B();

    // SCI R/W
    inline uint16_t SCIread(uint8_t address);

    // 2 bytes of data write to SCI
    inline void SCIwrite(uint8_t address, uint16_t data);


    // write n bytes data to the SCI register
    inline void writeSCI(uint8_t address, uint16_t *data, uint32_t length);

    // SDI Write
    inline void writeSDI(uint8_t data);
    inline void writeSDI(uint8_t *data, uint32_t length);

    // SCI Register R/W
    inline uint16_t readREG(uint8_t address);

    // write to register
    inline void writeREG(uint8_t address, uint16_t reg_name);
    inline void clearDecodeTime();

};

#endif
