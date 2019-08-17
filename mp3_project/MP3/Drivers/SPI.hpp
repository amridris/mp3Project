/*
 * SPI.hpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#ifndef SPI_HPP_
#define SPI_HPP_

#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "semphr.h"

class LabGPIO;

class SPI {

public:

    typedef enum {
        SSP0 = 0,
        SSP1
    } SSP_Peri;

    typedef enum {
        DIV_4 = 0b00,
        DIV_1 = 0b01,
        DIV_2 = 0b10,
        DIV_8 = 0b11
    } CLOCK_DIV;

    typedef enum {
        BIT4  = 0x3,
        BIT5  = 0x4,
        BIT6  = 0x5,
        BIT7  = 0x6,
        BIT8  = 0x7,
        BIT9  = 0x8,
        BIT10 = 0x9,
        BIT11 = 0xA,
        BIT12 = 0xB,
        BIT13 = 0xC,
        BIT14 = 0xD,
        BIT15 = 0xE,
        BIT16 = 0xF
    } SPIDataSize;

    typedef enum {
        FRAMEMODE_SPI       = 0x0,
        FRAMEMODE_TI        = 0x1,
        FRAMEMODE_MICROWAVE = 0x2
    } SPIFrameMode;

    typedef union {
        uint32_t bytes;
        struct {
            uint32_t TFE : 1;
            uint32_t TNF : 1;
            uint32_t RNE : 1;
            uint32_t RFF : 1;
            uint32_t BUSY: 1;
            uint32_t reserved: 27;
        } __attribute__((packed));
    } SSP_SR;


    // Mutex array of 2 to protect SPI bus 1 and 2
    static SemaphoreHandle_t spi_Mutex[2];

    // initializes SSP1 or SSP0
    bool init(SSP_Peri peripheral, SPIDataSize dataSize, SPIFrameMode mode, CLOCK_DIV clkdiv);

    //data transfers methods
    uint8_t   transfer(uint8_t data);
    uint8_t*  transfer(uint8_t *data, uint32_t len);
    uint16_t  wordTransfer(uint16_t data);
    uint16_t* wordTransfer(uint16_t *data, uint32_t len);

    // SSPI chip select and deselect
    void CS_select();
    void CS_deselect();
    // Destructor
    ~SPI();

protected:

    volatile LPC_SSP_TypeDef *SSPn;
    LabGPIO *CS;
    SSP_Peri Perip;
    // Protected constructor.
    SPI();

private:

    static volatile LPC_SSP_TypeDef *SSP[];

    void en_SSP0(CLOCK_DIV clkdiv);
    void en_SSP1(CLOCK_DIV clkdiv);

};

#endif /* SPI_HPP_ */
