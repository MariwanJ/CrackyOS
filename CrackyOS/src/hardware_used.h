/**
******************************************************************************
* @file    hardware_used.h
* @author  Mariwan Jalal
* @version V0.0.1
* @date
* @brief   This file contains Main header file which represent the HW used
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

#ifndef HARDWARE_USED_H
#define HARDWARE_USED_H

/**
 *   To use another hardware, try to make the corresponding files. Start with
 * the included file here.
 */

#if TARGET_NUCLEO_L476RG
#include "l476rg/nucleo_l476rg_hw.h"
#elif TARGET_NUCLEO_F207ZG
#include "f207zg/nucleo_f207zg_hw.h"
#elif TARGET_NUCLEO_L4R5ZI
#include "l4r5zi/nucleo_l4r5zi_hw.h"
#elif TARGET_NUCLEO_F767ZI
#include "f767zi/nucleo_f767zi_hw.h"
#endif
#endif
