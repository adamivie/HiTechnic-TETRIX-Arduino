/*
 * SoftwareI2C.h
 * 
 * Bit-banged I2C implementation for Arduino Mega
 * Allows multiple I2C buses on any GPIO pins
 * Based on the RobotC code you provided
 */

#ifndef SOFTWAREI2C_H
#define SOFTWAREI2C_H

#include <Arduino.h>

class SoftwareI2C {
  public:
    SoftwareI2C(uint8_t sdaPin, uint8_t sclPin);
    void begin();
    void beginTransmission(uint8_t address);
    uint8_t write(uint8_t data);
    uint8_t endTransmission();
    uint8_t requestFrom(uint8_t address, uint8_t quantity);
    uint8_t read();
    uint8_t available();
    
  private:
    uint8_t _sdaPin;
    uint8_t _sclPin;
    uint8_t _address;
    uint8_t _rxBuffer[32];
    uint8_t _rxBufferIndex;
    uint8_t _rxBufferLength;
    bool _transmitting;
    uint8_t _txBuffer[32];
    uint8_t _txBufferIndex;
    uint8_t _txBufferLength;
    
    // Low-level bit-bang functions
    void setSDA(bool high);
    bool readSDA();
    void setSCL(bool high);
    bool readSCL();
    
    // I2C protocol functions
    void startCondition();
    void stopCondition();
    bool writeBit(bool bit);
    bool readBit();
    bool writeByte(uint8_t byte);
    uint8_t readByte(bool ack);
    
    void delayHalf();
};

#endif
