/**
 *******************************************************************************
 * @file    interruptin.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief  This file contains definitions for interruptIn instruction
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

#ifndef INTERRUPTIN_H
#define INTERRUPTIN_H
#include "hardware_used.h"

/**
 * This function will be the interruptIn function
 * It executes the function connected to the GPIO.
 * @param enumNumber : Represent the pinname in enum values.
 * */
void GeneralInterruptInCallback(int *enumNumber);

#endif /* INTERRUPTIN_H_ */
