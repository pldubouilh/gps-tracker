#include <Wire.h>
#include <Adafruit_MMA8451.h>

static inline uint8_t i2cread(void) {
  return Wire.read();
}

static inline void i2cwrite(uint8_t x) {
  Wire.write((uint8_t)x);
}

void Adafruit_MMA8451::writeRegister8(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(_i2caddr);
  i2cwrite((uint8_t)reg);
  i2cwrite((uint8_t)(value));
  Wire.endTransmission();
}

uint8_t Adafruit_MMA8451::readRegister8(uint8_t reg) {
    // I don't know - maybe the other verion of requestFrom works on all platforms.
    //  honestly, I don't want to go through and test them all.  Doing it this way
    //  is already known to work on everything else
    Wire.beginTransmission(_i2caddr);
    i2cwrite(reg);
    Wire.endTransmission(false); // MMA8451 + friends uses repeated start!!
    Wire.requestFrom(_i2caddr, 1);
    if (! Wire.available()) return -1;
    return (i2cread());
}

Adafruit_MMA8451::Adafruit_MMA8451(int32_t sensorID) {
  _sensorID = sensorID;
}

bool Adafruit_MMA8451::begin(uint8_t i2caddr) {
  Wire.begin();
  _i2caddr = i2caddr;

  uint8_t deviceid = readRegister8(MMA8451_REG_WHOAMI);
  if (deviceid != 0x1A) {
    return false;
  }

  // reset
  writeRegister8(MMA8451_REG_CTRL_REG2, 0x40);
  while (readRegister8(MMA8451_REG_CTRL_REG2) & 0x40);

  // 2G range
  writeRegister8(MMA8451_REG_XYZ_DATA_CFG, MMA8451_RANGE_2_G);

  // Auto-SLEEP
  // writeRegister8(MMA8451_REG_CTRL_REG2, 0x04);

  // MT (movement) interrupt wakes up from sleep
  writeRegister8(MMA8451_REG_CTRL_REG3, 0x08);

  // enable MT interrupt
  writeRegister8(MMA8451_REG_CTRL_REG4, 0x04);

  // MT interrupt on pin1
  writeRegister8(MMA8451_REG_CTRL_REG5, 0x04);

  // interrupt on X, Y, Z, motion detect
  writeRegister8(MMA8451_REG_FF_MT_CFG, 0x08 | 0x10 | 0x20 | 0x40);

  // threshold: 17*0.063g = 1.07g
  writeRegister8(MMA8451_REG_FF_MT_THS, 0x11);

  // no count
  writeRegister8(MMA8451_REG_FF_MT_CNT, 0x01);

  // Enable, reduced-noise, sleep-low-power, low-power
  // writeRegister8(MMA8451_REG_CTRL_REG1, 0x01 | 0x04 | (0x10 | 0x08) | (0x01 | 0x02));

  // or just enable
  writeRegister8(MMA8451_REG_CTRL_REG1, 0x01);

  return true;
}


void Adafruit_MMA8451::resetInterrupt(void) {
  readRegister8(MMA8451_REG_FF_MT_SRC);
}

