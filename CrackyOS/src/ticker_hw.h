/**
 ******************************************************************************
 * @file    ticker.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains ticker instruction.
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

#ifndef TICKER_HW_H
#define TICKER_HW_H
#include "hardware_used.h"

/**
 * TICKER Instruction. It creates a ticker object with specific definition
 * Syntax: ticker PinName mypin , TimeInterval inUsec, Start/STOP =1/0
 * */
int token_ticker(void);

// It will creates a Ticker object that could be started and stopped at any
// time. An digital GPIO should be given to the ticker to
/**
 * Flipper class declaration
 * */
class Flipper {
public:
  /**
   * Class Flipper constructor
   * @param m_en enum value for the GPIO pin
   * */
  explicit Flipper(int m_en);
  /**
   * Class Flipper destructor
   *
   * */
  ~Flipper();
  /**
   * Class Flipper copy constructor
   *
   * */
  Flipper(const Flipper &) = delete;
  /**
   * Class Flipper operator =
   *
   * */
  Flipper operator=(const Flipper &) = delete;
  /**
   * This function will toggle the GPIO
   * */
  void Toggle();
  /**
   * This function will retrieve the pinName as an enum value.
   * @return enum name of the GPIO
   * */
  int enumPINNAME(void);
  /**
   * Internal ticker object.
   * */
  Ticker mticker;
  /**
   *  This function will attach the toggle function to the ticker object.
   * */
  void __attach();
  /**
   *  This function will detach the toggle function to the ticker object.
   * */

  void __detach();
  /**
   *  This function will determine the time Interval needed to the ticker lunch
   * the interrupt.
   *  @param g : Interval value in microseconds (float value)
   * */
  void setInterval(uint32_t g);
  GlobalGP *m_pin;

private:
  /**
   * Interval value in usec
   * */
  uint32_t mInterval;
  /**
   * enum name of the GPIO to toggle.
   * */
  int m_enumPINNAME;
};
/**
 * struct definition for ticker object
 * */
typedef struct gTObj {
  Flipper *mToggelPin;
  gTObj *next;
  gTObj *prev;
} tobj;

void FreeUp_Tickers(int enumNumber);

/**
 * Global Ticker Object holder
 * @param enumPinName  Enum name of the GPIO
 * @param uSecTime     time between each interrupt
 * @param DefaultValue DefaultValue of the GPIO 1 or 0
 * @param tickername   Ticker object name in text.
 * */
void TickerObject_add(int enumPinName, uint32_t uSecTime, bool DefaultValue,
                      char *tickername);
/**
 * Find a ticker object in the linked list by using the enum number
 * @param enumPinName GPIO to find
 * */
tobj *TickerObject_find(int enumPinName);

/**
 * Global pointer to the Ticker object.
 * */
extern tobj *GTickerObject;

#endif
