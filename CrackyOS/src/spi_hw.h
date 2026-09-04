/**
 ******************************************************************************
 * @file    spi_hw.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains SPI instructions.
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
#ifndef SPI_HW_H
#define SPI_HW_H
#include "hardware_used.h"

#define MAX_TRY_SPI_WRITE_READ 20

#define SPI_START 0x55
#define SPI_ACK 0X33
#define SPI_DUMMY 0X00

/*
   We will do like this for the SPI:

*/

/**
Based on the device CD74HC4067, CS should be connected to pin no. 1
WARNING! You should use pull up resistor for the CS pins
                        -----------
             comm IN/OUT|1      24| VCC
                     I7 |2      23| I8
                     I6 |3      22| I9
                     I5 |4      21| I10
                     I4 |5      20| I11
                     I3 |6      19| I12
                     I2 |7      18| I13
                     I1 |8      17| I14
                     I0 |9      16| I15
                     S0 |10     15| Invert_Enable
                     S1 |11     14| S2
                    GND |12     13| S3
                        -----------
                        CD74HC4067
*/

/*
We will have ONLY one master SPI device. But this device has a decoder for the
CS signal. i.e. you can connect up to 15 devices to the master. Device 0
select_spi_channel(CHANNEL0) is reserved for the SD card.
*/
typedef enum { SPI_BYTE, SPI_WORD, SPI_FLOAT } SPIVarType;

typedef enum { SPI_8BIT = 8, SPI_12BIT = 12, SPI_16BIT = 16 } spiDataSize;

// typedef struct {
//   SPIVarType tag;
//   float f;
// } spitt;

// typedef struct {
//   spitt data;
//   unsigned char SPI_Packet[sizeof(spitt)];
// } spispecialVar;

// extern spispecialVar globalSpiVar;

extern SPI MasterSPI_Device;
extern DigitalOut Master_SPI_cs;

void select_spi_channel(unsigned char channelNo);

enum {
  CHANNEL0 = 0, // Reserved for SD-Card
  CHANNEL1 = 1,
  CHANNEL2 = 2,
  CHANNEL3 = 3,
  CHANNEL4 = 4,
  CHANNEL5 = 5,
  CHANNEL6 = 6,
  CHANNEL7 = 7,
  CHANNEL8 = 8,
  CHANNEL9 = 9,
  CHANNEL10 = 10,
  CHANNEL11 = 11,
  CHANNEL12 = 12,
  CHANNEL13 = 13,
  CHANNEL14 = 14,
  CHANNEL15 = 15,
};

/** DecoderGPIOclass class
 *
 *  DecoderGPIOclass class Definition
 *  This object is used to select different channels for the SPI - CS signal.
 *  It allows the system to communicate with up to 15 devices.
 */
class DecoderGPIOclass {
public:
  /**
   * Digital GPIO used to select different channels of the SPI_CS signal
   * */
  DigitalOut *Line3;
  DigitalOut *Line2;
  DigitalOut *Line1;
  DigitalOut *Line0;

  /**
   * Class DecoderGPIOclass constructor
   * @param p0  bit 0
   * @param p1  bit 1
   * @param p2  bit 2
   * @param p3  bit 3
   *
   * */
  DecoderGPIOclass(PinName p3, PinName p2, PinName p1, PinName p0);
  /**
   * Class DecoderGPIOclass copy constructor
   *
   * */
  DecoderGPIOclass(const DecoderGPIOclass &) = delete;
  /**
   * Class DecoderGPIOclass operator =
   *
   * */
  void operator=(const DecoderGPIOclass &) = delete;
  /**
   * Class DecoderGPIOclass destructor
   *
   * */
  ~DecoderGPIOclass();
  /**
   * Class DecoderGPIOclass initialization
   *@param v0   bit 0   --> connected to CD74HC4067M pin 10
   *@param v1   bit 1   --> connected to CD74HC4067M pin 11
   *@param v2   bit 2   --> connected to CD74HC4067M pin 14
   *@param v3   bit 3   --> connected to CD74HC4067M pin 13
   * Use this function to select the required CS for the SPI.
   * These lines are connected to the CD74HC4067M pin 10,11,14,13
   * these lines MUST have a pullup resistor (10K maybe)
   * */
  void select(unsigned char v3, unsigned char v2, unsigned char v1,
              unsigned char v0);
};

// Object will hold the decoder GPIO line
extern DecoderGPIOclass DecoderGPIO;
//

void SendReceiveSPI_Device(
    unsigned char DEVICE_ID,
    int numberOfBytesWillBeReturned, // Do the slave sends us any value?
                                     // //Notice that if the slave return 8
                                     // bits, we should AND the result with
                                     // 0x00FF for each int value.
    unsigned int sendDataToSlave,    //  Data to be sent to the slave.
    int globalVariableData, //  Global variable has got the data if the pointer
                            //  is not -1. This will be applicable when we have
                            //  an array to send.
    int globalVariableResults, //  This should be a position if we need results
                               //  or -1 if we don't need results.
    spiDataSize DataSize,      //  Data size or type -- 8,12,16
    int spi_frequency,         //  SPI channel frequency
    int spi_mode,              //  SPI Mode - Default is 0
    int DelayBetweenPackets);  //  Delay between packets.

#endif
