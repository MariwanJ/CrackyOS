/**
 ******************************************************************************
 * @file    ringbuffer.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   Ring buffer implementation (FIFO)
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
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#define DEBUG_MAX_FIFO_RING_BUF_SIZE 1024 * 4
#define SERIAL_MAX_FIFO_RING_BUF_SIZE 1024
/** RingBuffer class
 *
 *  RingBuffer: A FIFO buffer implementation.
 *
 */

class RingBuffer {
protected:
  unsigned char ring_buf[DEBUG_MAX_FIFO_RING_BUF_SIZE];
  unsigned int _bufsize;
  unsigned int read_index;
  unsigned int write_index;
  unsigned int item_count;

public:
  /** Class Constructor RingBuffer
   * @param bufsize : Size of the object
   * */
  RingBuffer(int _bufsize = 100);
  /** Class Destructor RingBuffer
   *
   * */
  ~RingBuffer();

  /**
   * Put a value into the buffer
   * @param c : a one byte
   * */
  void put(unsigned char c);
  /**
   * Get a byte from the buffer and change the pointer positions.
   * @return retrieved value from the buffer
   * */
  unsigned char get(void);
  /**
   * Check if the buffer is empty.
   * @return 1 if empty .. 0 if not
   * */
  unsigned char checkEmpty(void);

  /**
   * Check if the buffer has got overflow.
   * @return 1 if overflow occurred .. 0 if not
   * */

  unsigned char checkOverflow(void);

  /**
   * Local variable keeps the size of the buffer.
   * */
  unsigned int buffersize(void) { return _bufsize; };
};

/** Ring buffer variable used by DEBUG message*/
extern RingBuffer DebugSerialFIFO;

#endif
