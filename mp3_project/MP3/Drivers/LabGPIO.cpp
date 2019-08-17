/*
 * LabGPIO.cpp
 *
 *      Created on: December 5, 2018
 *      Author: Aamer Idris, Daniel Pilkington, Krishneel Sahdeo
 */

#include "LabGPIO.hpp"

/// Constructor
LabGPIO::LabGPIO(uint8_t port, uint8_t pin, bool output, bool high) : _port(port), _pin(pin) {
    // base pointers of ports mapped in memory
    LPC_GPIO_TypeDef *basePorts[] = { LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4 };
    // store reference to base pointer of the specified port
    mpGPIOPin = basePorts[_port];

    // set pin function to be GPIO
    volatile uint32_t *pinsel = &(LPC_PINCON->PINSEL0);
    pinsel[2*_port] &= ~(3 << (2*_pin));

    setDirection(output);
    output ? set(high) : set(false);
}

/// Destructor
LabGPIO::~LabGPIO()                     { setLow(); }

uint8_t LabGPIO::getPortNum() const     { return _port; }
uint8_t LabGPIO::getPinNum()  const     { return _pin; }

void LabGPIO::setDirection(bool output) { output ? setAsOutput() : setAsInput(); }
void LabGPIO::setAsInput()              { mpGPIOPin->FIODIR &= ~(1 << _pin); }
void LabGPIO::setAsOutput()             { mpGPIOPin->FIODIR |=  (1 << _pin); }

void LabGPIO::set(bool high)            { high ? setHigh() : setLow(); }
void LabGPIO::setHigh()                 { mpGPIOPin->FIOSET = (1 << _pin); };
void LabGPIO::setLow()                  { mpGPIOPin->FIOCLR = (1 << _pin); };

/// return false if level is 0 or true if level is >= 1
bool LabGPIO::getLevel()                { return !!( mpGPIOPin->FIOPIN & (1 << _pin) ); }
