/**************************************************************************/
/*!
    @file     Adafruit_MMA8451.h
    @author   K. Townsend (Adafruit Industries)
    @license  BSD (see license.txt)

    This is a library for the Adafruit MMA8451 Accel breakout board
    ----> https://www.adafruit.com/products/2019

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0  - First release
*/
/**************************************************************************/

#define _ADAFRUIT_MMA8451_H_

#include "Arduino.h"
#include <Wire.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define MMA8451_DEFAULT_ADDRESS                 (0x1D)    // if A is GND, its 0x1C
/*=========================================================================*/

#define MMA8451_REG_OUT_X_MSB     0x01
#define MMA8451_REG_SYSMOD        0x0B
#define MMA8451_REG_WHOAMI        0x0D
#define MMA8451_REG_XYZ_DATA_CFG  0x0E
#define MMA8451_REG_PL_STATUS     0x10
#define MMA8451_REG_PL_CFG        0x11
#define MMA8451_REG_CTRL_REG1     0x2A
#define MMA8451_REG_CTRL_REG2     0x2B
#define MMA8451_REG_CTRL_REG3     0x2C
#define MMA8451_REG_CTRL_REG4     0x2D
#define MMA8451_REG_CTRL_REG5     0x2E

#define MMA8451_REG_FF_MT_CFG     0x15
#define MMA8451_REG_FF_MT_SRC     0x16
#define MMA8451_REG_FF_MT_THS     0x17
#define MMA8451_REG_FF_MT_CNT     0x18


#define MMA8451_PL_PUF            0
#define MMA8451_PL_PUB            1
#define MMA8451_PL_PDF            2
#define MMA8451_PL_PDB            3
#define MMA8451_PL_LRF            4
#define MMA8451_PL_LRB            5
#define MMA8451_PL_LLF            6
#define MMA8451_PL_LLB            7

#define MMA8451_RANGE_8_G 0b10
#define MMA8451_RANGE_4_G 0b01
#define MMA8451_RANGE_2_G 0b00
class Adafruit_MMA8451
{
 public:
  Adafruit_MMA8451(int32_t id = -1);

  bool begin(uint8_t addr = MMA8451_DEFAULT_ADDRESS);
  void resetInterrupt(void);
  void writeRegister8(uint8_t reg, uint8_t value);
 protected:
  uint8_t readRegister8(uint8_t reg);
 private:
  int32_t _sensorID;
  int8_t  _i2caddr;
};
