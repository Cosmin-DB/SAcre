#pragma once
#include "board.h"
#include <Wire.h>

namespace I2C {
inline void begin() {
  if (!Wire.begin(Pins::SDA, Pins::SCL, 100000)) Board::fail("Inicio I2C");
  Wire.setTimeOut(50);
}

inline bool read(uint8_t address, uint8_t reg, uint8_t* data, size_t size) {
  // STOP+START also works with the original touch controller examples.
  // Always finish with a read: relevant to RV-8803 I2C STOP erratum.
  for (unsigned attempt = 0; attempt < 3; ++attempt) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    if (Wire.endTransmission(true) == 0) {
      if (Wire.requestFrom(address, size, true) == size) {
        for (size_t i = 0; i < size; ++i) data[i] = Wire.read();
        return true;
      }
      while (Wire.available()) Wire.read();
    }
    delay(2);
  }
  return false;
}
inline bool readByte(uint8_t address, uint8_t reg, uint8_t& value) {
  return read(address, reg, &value, 1);
}
inline bool write(uint8_t address, uint8_t reg, const uint8_t* data, size_t size) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data, size);
  const bool acknowledged = Wire.endTransmission(true) == 0;
  // RV-8803 erratum: follow even a failed write with a read attempt.
  if (address == 0x32) {
    uint8_t dummy;
    const bool readOk = readByte(address, 0x00, dummy);
    return acknowledged && readOk;
  }
  return acknowledged;
}
inline bool writeByte(uint8_t address, uint8_t reg, uint8_t value) {
  return write(address, reg, &value, 1);
}
}
