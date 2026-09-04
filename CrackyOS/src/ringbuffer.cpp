/**
 ******************************************************************************
 * @file    ringbuffer.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   Ring buffer implementation (FIFO)
 ******************************************************************************
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ringbuffer.h"
#include <mbed.h>
#include "types.h"

 /** Class Constructor RingBuffer
  * @param bufsize : Size of the object
  * */
RingBuffer::RingBuffer(int bufsize) : _bufsize(bufsize), read_index(0), write_index(0), item_count(0)
{
    if (_bufsize == 0 || _bufsize > DEBUG_MAX_FIFO_RING_BUF_SIZE)
        _bufsize = DEBUG_MAX_FIFO_RING_BUF_SIZE;
    memset(ring_buf, 0, sizeof(ring_buf));
}
/** destructor Ring Buffer*/
RingBuffer::~RingBuffer() = default;

/**  Check for overflow*/
unsigned char RingBuffer::checkOverflow(void) {
    return item_count >= _bufsize;
}
/**  Check if ringbuffer is empty*/
unsigned char RingBuffer::checkEmpty(void) {
    return item_count == 0;
}

/** put a new value into the ringbuffer*/
void RingBuffer::put(unsigned char c)
{
    if (DEBUG_MESSAGE_ENABLE != 1)
        return;   //Nothing to do if the debugger is deactivated.
    while (checkOverflow() == 1) {
        wait_us(100);
    }

    ring_buf[write_index] = c;
    write_index++;
    if (write_index >= _bufsize)
        write_index = 0;
    item_count++;
    return;
}

/** Get current indexed value*/
unsigned char RingBuffer::get(void)
{
    if (checkEmpty() == 1)
        return '\0';    //Nothing to get.

    unsigned char c = ring_buf[read_index];
    ring_buf[read_index] = '\0';
    read_index++;
    if (read_index >= _bufsize)
        read_index = 0;
    item_count--;
    return c;
}

/** Ring Buffer class variable */
RingBuffer DebugSerialFIFO(DEBUG_MAX_FIFO_RING_BUF_SIZE);
