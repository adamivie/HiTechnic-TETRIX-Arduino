/*
 * SoftwareI2C.cpp
 * 
 * Bit-banged I2C implementation
 */

#include "SoftwareI2C.h"

SoftwareI2C::SoftwareI2C(uint8_t sdaPin, uint8_t sclPin) {
  _sdaPin = sdaPin;
  _sclPin = sclPin;
  _rxBufferIndex = 0;
  _rxBufferLength = 0;
  _txBufferIndex = 0;
  _txBufferLength = 0;
  _transmitting = false;
}

void SoftwareI2C::begin() {
  // Set pins as inputs (high impedance) with pullups
  pinMode(_sdaPin, INPUT_PULLUP);
  pinMode(_sclPin, INPUT_PULLUP);
  digitalWrite(_sdaPin, HIGH);
  digitalWrite(_sclPin, HIGH);
  delay(10);
}

void SoftwareI2C::delayHalf() {
  // Half of I2C clock period (100kHz = 10us period, 5us half)
  delayMicroseconds(5);
}

void SoftwareI2C::setSDA(bool high) {
  if (high) {
    pinMode(_sdaPin, INPUT_PULLUP);  // Release (pull-up pulls high)
  } else {
    pinMode(_sdaPin, OUTPUT);
    digitalWrite(_sdaPin, LOW);      // Pull low
  }
  delayHalf();
}

bool SoftwareI2C::readSDA() {
  pinMode(_sdaPin, INPUT_PULLUP);
  delayHalf();
  return digitalRead(_sdaPin);
}

void SoftwareI2C::setSCL(bool high) {
  if (high) {
    pinMode(_sclPin, INPUT_PULLUP);  // Release
    // Clock stretching: wait for slave to release SCL
    uint16_t timeout = 1000;
    while (digitalRead(_sclPin) == LOW && timeout > 0) {
      delayMicroseconds(1);
      timeout--;
    }
  } else {
    pinMode(_sclPin, OUTPUT);
    digitalWrite(_sclPin, LOW);      // Pull low
  }
  delayHalf();
}

bool SoftwareI2C::readSCL() {
  pinMode(_sclPin, INPUT_PULLUP);
  delayHalf();
  return digitalRead(_sclPin);
}

void SoftwareI2C::startCondition() {
  // SDA high, SCL high -> SDA goes low while SCL high
  setSDA(true);
  setSCL(true);
  setSDA(false);  // START condition
  setSCL(false);
}

void SoftwareI2C::stopCondition() {
  // SDA low, then SCL high, then SDA high
  setSDA(false);
  setSCL(true);
  setSDA(true);   // STOP condition
}

bool SoftwareI2C::writeBit(bool bit) {
  setSDA(bit);
  setSCL(true);   // Clock high - data is read
  setSCL(false);  // Clock low
  return true;
}

bool SoftwareI2C::readBit() {
  setSDA(true);   // Release SDA
  setSCL(true);   // Clock high
  bool bit = readSDA();
  setSCL(false);  // Clock low
  return bit;
}

bool SoftwareI2C::writeByte(uint8_t byte) {
  // Write 8 bits
  for (uint8_t i = 0; i < 8; i++) {
    writeBit((byte & 0x80) != 0);
    byte <<= 1;
  }
  
  // Read ACK bit
  bool ack = !readBit();  // ACK is low
  return ack;
}

uint8_t SoftwareI2C::readByte(bool ack) {
  uint8_t byte = 0;
  
  // Read 8 bits
  for (uint8_t i = 0; i < 8; i++) {
    byte <<= 1;
    if (readBit()) {
      byte |= 1;
    }
  }
  
  // Send ACK/NACK
  writeBit(!ack);
  
  return byte;
}

void SoftwareI2C::beginTransmission(uint8_t address) {
  _address = address;
  _txBufferIndex = 0;
  _txBufferLength = 0;
  _transmitting = true;
}

uint8_t SoftwareI2C::write(uint8_t data) {
  if (_transmitting) {
    if (_txBufferLength < 32) {
      _txBuffer[_txBufferLength++] = data;
      return 1;
    }
  }
  return 0;
}

uint8_t SoftwareI2C::endTransmission() {
  // Send start condition
  startCondition();
  
  // Send address with write bit
  if (!writeByte((_address << 1) | 0)) {
    stopCondition();
    return 2;  // NACK on address
  }
  
  // Send data bytes
  for (uint8_t i = 0; i < _txBufferLength; i++) {
    if (!writeByte(_txBuffer[i])) {
      stopCondition();
      return 3;  // NACK on data
    }
  }
  
  // Send stop condition
  stopCondition();
  
  _txBufferLength = 0;
  _transmitting = false;
  
  return 0;  // Success
}

uint8_t SoftwareI2C::requestFrom(uint8_t address, uint8_t quantity) {
  if (quantity > 32) {
    quantity = 32;
  }
  
  // Send start condition
  startCondition();
  
  // Send address with read bit
  if (!writeByte((address << 1) | 1)) {
    stopCondition();
    return 0;  // NACK
  }
  
  // Read bytes
  _rxBufferLength = 0;
  for (uint8_t i = 0; i < quantity; i++) {
    bool ack = (i < quantity - 1);  // ACK all but last byte
    _rxBuffer[_rxBufferLength++] = readByte(ack);
  }
  
  // Send stop condition
  stopCondition();
  
  _rxBufferIndex = 0;
  return _rxBufferLength;
}

uint8_t SoftwareI2C::read() {
  if (_rxBufferIndex < _rxBufferLength) {
    return _rxBuffer[_rxBufferIndex++];
  }
  return 0;
}

uint8_t SoftwareI2C::available() {
  return _rxBufferLength - _rxBufferIndex;
}
