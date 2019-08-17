/*
 * SPI.cpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#include <MP3/Drivers/SPI.hpp>
#include "LabGPIO.hpp"

volatile LPC_SSP_TypeDef* SPI::SSP[] = { LPC_SSP0, LPC_SSP1 };

SemaphoreHandle_t SPI::spi_Mutex[] = { xSemaphoreCreateMutex(), xSemaphoreCreateMutex() };

SPI::SPI() : SSPn(NULL), CS(NULL), Perip(SSP0) { }

SPI::~SPI() {
    delete CS; CS = NULL;
}

bool SPI::init(SSP_Peri peripheral, SPIDataSize dataSize, SPIFrameMode mode, CLOCK_DIV clkdiv) {
    switch (peripheral) {
        case SSP0: en_SSP0(clkdiv); break;
        case SSP1: en_SSP1(clkdiv); break;
    }

    Perip = peripheral;
    SSPn = SSP[Perip];

    SSPn->CR0 = (dataSize << 0);
    SSPn->CR1 |= (1 << 1);
    SSPn->CR1 &= ~(1 << 2);

    return true;
}

void SPI::en_SSP0(CLOCK_DIV clkdiv) {
    LPC_SC->PCONP |= (1 << 21);

    LPC_SC->PCLKSEL1 &= ~(3 << 10);
    LPC_SC->PCLKSEL1 |= (clkdiv << 10);

    LPC_PINCON->PINSEL0 &= ~(3 << 30);
    LPC_PINCON->PINSEL0 |=  (2 << 30);
    LPC_PINCON->PINSEL1 &= ~(3 << 2) | ~(3 << 4);
    LPC_PINCON->PINSEL1 |=  (2 << 2) |  (2 << 4);
}

void SPI::en_SSP1(CLOCK_DIV clkdiv) {
    LPC_SC->PCONP |= (1 << 10);

    LPC_SC->PCLKSEL0 &= ~(3 << 20);
    LPC_SC->PCLKSEL0 |= (clkdiv << 20);

    LPC_PINCON->PINSEL0 &= ~(3 << 14) | ~(3 << 16) | ~(3 << 18);
    LPC_PINCON->PINSEL0 |=  (2 << 16) |  (2 << 18);
}

uint8_t SPI::transfer(uint8_t data) {
    SSPn->DR = data;                                                            
    while (SSPn->SR & (1 << 4));                                                
    return SSPn->DR;
}

uint16_t SPI::wordTransfer(uint16_t data) {
    uint16_t captured = 0;
    captured  = transfer(data >> 8) << 8;                                       
    captured |= transfer((data & 0xFF) & 0xFF);                                 
    return captured;
}

uint8_t* SPI::transfer(uint8_t *data, uint32_t len) {
    uint8_t captured[len];
    for (uint32_t i = 0; i < len; i++)
        captured[i] = transfer(data[i]);
    return captured;
}

uint16_t* SPI::wordTransfer(uint16_t *data, uint32_t len) {
    uint16_t in[len];
    for (uint32_t i = 0; i < len; i++)
        in[i] = wordTransfer(data[i]);
    return in;
}


void SPI::CS_select() {
    if (xSemaphoreTake(spi_Mutex[Perip], portMAX_DELAY))
        CS->setLow();
}

void SPI::CS_deselect() {
    CS->setHigh();
    xSemaphoreGive(spi_Mutex[Perip]);
}

