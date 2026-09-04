/**
 ******************************************************************************
 * @file    i2c_hw.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains I2C instructions definitions.
 *
 * Copyright (C) 2026 Mariwan Jalal
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * CrackyOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CrackyOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef i2c_hw_H
#define i2c_hw_H
#include "hardware_used.h"
#include "types.h"

/**Be aware of the reserved addresses as follow :

 *    Reserved address
 *index   8-bit byte  Description
 *    7-bit address   R/W value

 *MSB(4-bit) LSB(3-bit)   1-bit
 *1   0000    000         0   General call
 *2   0000    000         1   Start byte
 *3   0000    001         X   CBUS address
 *4   0000    010         X   Reserved for different bus format
 *5   0000    011         X   Reserved for future purpose
 *6   0000    1XX         X   HS-mode master code
 *7   1111    1XX         1   Device ID
 *8   1111    0XX         X   10-bit slave addressing
 *
 *Non-reserved addresses in 7-bit address space
 *MSB (4-bit)     Typical usage
 *0001            Digital receivers, SMBus
 *0010            TV video line decoders, IPMB
 *0011            AV codecs
 *0100            Video encoders, GPIO expanders
 *0101            ACCESS bus, PMBus
 *0110            VESA DDC, PMBus
 *0111            Display controller
 *1000            TV signal processing, audio processing, SMBus
 *1001            AV switching, ADCs and DACs, IPMB, SMBus
 *1010            Storage memory, real-time clock
 *1011            AV processors
 *1100            PLLs and tuners, modulators and demodulators, SMBus
 *1101            AV processors and decoders, audio power amplifiers, SMBus
 *1110            AV colour space converters
 */

/** ENUM definition of I2C type used by the script interpreter*/
typedef enum { I2CSLAVE = 0, I2CMASTER = 1, I2CUDEFINED = 2 } i2ctype;

/** I2C_Communication class
 *
 *  I2C class implementation
 *
 */
class I2C_Communication {
public:
  /**Class I2C_Communication constructor
   *   @param SDA      :    Pinname of SDA
   *   @param SCL      :    Pinname of SCL
   *   @param i2ctype  :    ENUM communication type (0=I2CSLAVE , 1= I2CMASTER,
   * 2= I2CUDEFINED
   */
  I2C_Communication(PinName SDA, PinName SCL, i2ctype Slave);
  /**
   * Class I2C_Communication Destructor
   * */
  ~I2C_Communication();

  /**
   * Initialization function.
   * */
  int init(void);
  /**
   * Register the address
   * @param int address of the I2C
   * */
  void address(int address);

  /**
   * Change type of I2C
   * @param type I3CTYPE enum value.
   *
   *  0=I2CSLAVE ,
   *  1= I2CMASTER
   *  2= I2CUDEFINED
   * */
  void type(i2ctype typ);

  /**
   * Pointer to the I2C Object as slave
   **/
  I2CSlave *m_i2cSlave;

  /**
   * Pointer to the I2C Object as master
   **/

  I2C *m_i2cMaster;
  /**
   * Pointer to an array points to the values to be send
   **/

  char *m_sendData;
  /**
   * Pointer to an array points to the values to be received
   **/
  char *m_recieveData;

private:
  /**
   * I2C Type local variable
   * */
  int m_type;
  /**
   * I2C Type local address
   * */

  int m_address;

  /**
   * I2C local pinName for DATA
   * */

  PinName m_sda;
  /**
   * I2C local pinName for CLK
   * */

  PinName m_scl;
};

/**
 * Global variable keeps the I2C OBJECT.
 * */
extern I2C_Communication i2c;

#endif
