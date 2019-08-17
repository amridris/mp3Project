#ifndef LABGPIO_H
#define LABGPIO_H

#include "LPC17xx.h"

class LabGPIO {

protected:

    volatile LPC_GPIO_TypeDef *mpGPIOPin;
    uint8_t _port;
    uint8_t _pin;

public:

    LabGPIO(uint8_t port, uint8_t pin, bool output = false, bool high = false);
    uint8_t getPortNum() const;
    uint8_t getPinNum() const;
    void setAsInput();
    void setAsOutput();
    void setDirection(bool output);
    void setHigh();
    void setLow();
    void set(bool high);
    bool getLevel();
    ~LabGPIO();
};

#endif
