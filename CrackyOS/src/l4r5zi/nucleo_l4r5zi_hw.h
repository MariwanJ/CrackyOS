/**
 ******************************************************************************
 * @file    nucleo_l4r5zi.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains all hardware specific definitions
 *          and important variables that are used by the interpreter.
 *          This file should be replaced with a suitable version if the hardware
 *          is changed. This is valid only for NUCLEO L4R5ZI board
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

#ifndef NUCLEO_L4R5ZI_HW_h
#define NUCLEO_L4R5ZI_HW_h

#if TARGET_NUCLEO_L4R5ZI

#include "mbed.h"

// Interactive Terminal
#define SERIAL_TX PD_5
#define SERIAL_RX PD_6

// Debug if enabled:
#define DEBUG_TX PC_10
#define DEBUG_RX PC_11
#include "SD/SDBlockDevice.h"
#include <math.h>

#include <ctype.h> /* toupper */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strcpy */

#include <cstdarg>
#include <stdarg.h>

#include "interrupt_in.h"
#include "ringbuffer.h"

#include "arithmetic.h"
#include "common.h"
#include "i2c_hw.h"
#include "sdcard.h"
#include "serialmessage.h"
#include "spi_hw.h"
#include "ticker_hw.h"
#include "tokenizetofile.h"
#include "tokens.h"
#include "types.h"
/**
 * Enum values defines the token words in a enum form.
 * This enum is the most important enum which any change could cause a problem
 * for script program used. Adding new hardware or instruction require to change
 * this enum ..
 * */
enum {
  TOKEN_NULL = 0, // 0
  TOKEN_EOF,      // 1
  TOKEN_CLEAR,    // 2
  TOKEN_BAD,      // 3
  TOKEN_BEGIN,    // 4
  TOKEN_END,      // 5
  TOKEN_SEC,      // 6
  TOKEN_MSEC,     // 7
  TOKEN_USEC,     // 8
  TOKEN_NC,       // 9
  TOKEN_PA_0,     // 10
  TOKEN_PA_1,     // 11
  TOKEN_PA_2,     // 12
  TOKEN_PA_3,     // 13
  TOKEN_PA_4,     // 14
  TOKEN_PA_5,     // 15
  TOKEN_PA_6,     // 16
  TOKEN_PA_7,     // 17
  TOKEN_PA_8,     // 18
  TOKEN_PA_9,     // 19
  TOKEN_PA_10,    // 20
  TOKEN_PA_11,    // 21
  TOKEN_PA_12,    // 22
  TOKEN_PA_13,    // 23
  TOKEN_PA_14,    // 24
  TOKEN_PA_15,    // 25
  TOKEN_PB_0,     // 26
  TOKEN_PB_1,     // 27
  TOKEN_PB_2,     // 28
  TOKEN_PB_3,     // 29
  TOKEN_PB_4,     // 30
  TOKEN_PB_5,     // 31
  TOKEN_PB_6,     // 32
  TOKEN_PB_7,     // 33
  TOKEN_PB_8,     // 34
  TOKEN_PB_9,     // 35
  TOKEN_PB_10,    // 36
  TOKEN_PB_11,    // 37
  TOKEN_PB_12,    // 38
  TOKEN_PB_13,    // 39
  TOKEN_PB_14,    // 40
  TOKEN_PB_15,    // 41
  TOKEN_PC_0,     // 42
  TOKEN_PC_1,     // 43
  TOKEN_PC_2,     // 44
  TOKEN_PC_3,     // 45
  TOKEN_PC_4,     // 46
  TOKEN_PC_5,     // 47
  TOKEN_PC_6,     // 48
  TOKEN_PC_7,     // 49
  TOKEN_PC_8,     // 50
  TOKEN_PC_9,     // 51
  TOKEN_PC_10,    // 52
  TOKEN_PC_11,    // 53
  TOKEN_PC_12,    // 54
  TOKEN_PC_13,    // 55
  TOKEN_PC_14,    // 56
  TOKEN_PC_15,    // 57
  TOKEN_PD_0,     // 58
  TOKEN_PD_1,     // 59
  TOKEN_PD_2,     // 60
  TOKEN_PD_3,     // 61
  TOKEN_PD_4,     // 62
  TOKEN_PD_5,     // 63
  TOKEN_PD_6,     // 64
  TOKEN_PD_7,     // 65
  TOKEN_PD_8,     // 66
  TOKEN_PD_9,     // 67
  TOKEN_PD_10,    // 68
  TOKEN_PD_11,    // 69
  TOKEN_PD_12,    // 70
  TOKEN_PD_13,    // 71
  TOKEN_PD_14,    // 72
  TOKEN_PD_15,    // 73
  TOKEN_PE_0,     // 74
  TOKEN_PE_1,     // 75
  TOKEN_PE_2,     // 76
  TOKEN_PE_3,     // 77
  TOKEN_PE_4,     // 78
  TOKEN_PE_5,     // 79
  TOKEN_PE_6,     // 80
  TOKEN_PE_7,     // 81
  TOKEN_PE_8,     // 82
  TOKEN_PE_9,     // 83
  TOKEN_PE_10,    // 84
  TOKEN_PE_11,    // 85
  TOKEN_PE_12,    // 86
  TOKEN_PE_13,    // 87
  TOKEN_PE_14,    // 88
  TOKEN_PE_15,    // 89
  TOKEN_PF_0,     // 90
  TOKEN_PF_1,     // 91
  TOKEN_PF_2,     // 92
  TOKEN_PF_3,     // 93
  TOKEN_PF_4,     // 94
  TOKEN_PF_5,     // 95
  TOKEN_PF_6,     // 96
  TOKEN_PF_7,     // 97
  TOKEN_PF_8,     // 98
  TOKEN_PF_9,     // 99
  TOKEN_PF_10,    // 100
  TOKEN_PF_11,    // 101
  TOKEN_PF_12,    // 102
  TOKEN_PF_13,    // 103
  TOKEN_PF_14,    // 104
  TOKEN_PF_15,    // 105
  TOKEN_PG_0,     // 106
  TOKEN_PG_1,     // 107
  TOKEN_PG_2,     // 108
  TOKEN_PG_3,     // 109
  TOKEN_PG_4,     // 110
  TOKEN_PG_5,     // 111
  TOKEN_PG_6,     // 112
  TOKEN_PG_7,     // 113
  TOKEN_PG_8,     // 114
  TOKEN_PG_9,     // 115
  TOKEN_PG_10,    // 116
  TOKEN_PG_11,    // 117
  TOKEN_PG_12,    // 118
  TOKEN_PG_13,    // 119
  TOKEN_PG_14,    // 120
  TOKEN_PG_15,    // 121
  TOKEN_PH_0,                 // 122
  TOKEN_PH_1,                 // 123
//  TOKEN_PH_2,                 // 124
  TOKEN_PH_3,                 // 125
  TOKEN_PH_4,                 // 126
  TOKEN_PH_5,                 // 127
  TOKEN_PH_6,                 // 128
  TOKEN_PH_7,                 // 129
  TOKEN_PH_8,                 // 130
  TOKEN_PH_9,                 // 131
  TOKEN_PH_10,                // 132
  TOKEN_PH_11,                // 133
  TOKEN_PH_12,                // 134
  TOKEN_PH_13,                // 135
  TOKEN_PH_14,                // 136
  TOKEN_PH_15,                // 137
  TOKEN_CREATE_REG,           // 138
  TOKEN_PUT,                  // 139
  TOKEN_GET,                  // 140
  TOKEN_APUT,                 // 141
  TOKEN_AGET,                 // 142
  TOKEN_GEN_PWM,              // 143
  TOKEN_PRINT,                // 144
  TOKEN_SERIAL_GET,           // 145
  TOKEN_I2C_SEND,             // 146
  TOKEN_I2C_GET,              // 147
  TOKEN_SPI_SEND,             // 148
  TOKEN_SIZE_OF_VAR,          // 149
  TOKEN_SIN,                  // 150
  TOKEN_COS,                  // 151
  TOKEN_TAN,                  // 152
  TOKEN_ASIN,                 // 153
  TOKEN_ACOS,                 // 154
  TOKEN_ATAN,                 // 155
  TOKEN_ABS,                  // 156
  TOKEN_EXP,                  // 157
  TOKEN_LOG,                  // 158
  TOKEN_LN,                   // 159
  TOKEN_ROUND,                // 160
  TOKEN_SQRT,                 // 161
  TOKEN_MOD,                  // 162
  TOKEN_EXIT,                 // 163
  TOKEN_DELETE,               // 164
  TOKEN_FUNCTION,             // 165
  TOKEN_CALL,                 // 166
  TOKEN_DELAY,                // 167
  TOKEN_BAUD_RATE,            // 168
  TOKEN_DEBUG_PORT,           // 169
  TOKEN_TICKER,               // 170
  TOKEN_INTERRUPTIN,          // 171
  TOKEN_COMM_CHANNEL_SERIAL,  // 172
  TOKEN_COMM_CHANNEL_LCD,     // 173
  TOKEN_FOR,                  // 174
  TOKEN_WHILE,                // 175
  TOKEN_IF,                   // 176
  TOKEN_ELSE,                 // 177
  TOKEN_GOTO,                 // 178
  TOKEN_EQUAL,                // 179
  TOKEN_EQUAL_EQUAL,          // 180
  TOKEN_LESSTHAN,             // 181
  TOKEN_LESSTHANEQUAL,        // 182
  TOKEN_GREATERTHAN,          // 183
  TOKEN_GREATERTHANEQUAL,     // 184
  TOKEN_NOTEQUAL,             // 185
  TOKEN_AND,                  // 186
  TOKEN_OR,                   // 187
  TOKEN_INVERT,               // 188
  TOKEN_PLUS,                 // 189
  TOKEN_PLUSPLUS,             // 190
  TOKEN_MINUS,                // 191
  TOKEN_MINUSMINUS,           // 192
  TOKEN_ASTR,                 // 193
  TOKEN_SLASH,                // 194
  TOKEN_BACKSLASH,            // 195
  TOKEN_LEFT_PARENTHESIS,     // 196
  TOKEN_RIGHT_PARENTHESIS,    // 197
  TOKEN_LEFT_CURLY_BRACKET,   // 198
  TOKEN_RIGHT_CURLY_BRACKET,  // 199
  TOKEN_LEFT_SQUARE_BRACKET,  // 200
  TOKEN_RIGHT_SQUARE_BRACKET, // 201
  TOKEN_COMMA,                // 202
  TOKEN_SEMICOLON,             // 203
  TOKEN_CARET,                 // 204
  TOKEN_SHIFT_RIGHT,           // 205
  TOKEN_SHIFT_LEFT,            // 206
  TOKEN_LABEL,                 // 207
  TOKEN_HASH,                  // 208
  TOKEN_NUMBER,                // 209
  TOKEN_STRING,                // 210
  TOKEN_VARIABLE,              // 211
  TOKEN_REM,                   // 212
  TOKEN_SPI_MODULE,            // 213
  TOKEN_SPI_SINGLE,            // 214
  TOKEN_CAN_SEND,              // 215
  TOKEN_CAN_GET,               // 216
  TOKEN_ENABLE_DEBUG,          // 217
  TOKEN_DISABLE_DEBUG,         // 218
  TOKEN_RETURN,                // 219
  TOKEN_NEW_LINE               // 220

};

/**These GPIO will be used for the CS-decoder of the SPI peripherals.
 */
#define LINE0 PE_0
#define LINE1 PF_11
#define LINE2 PF_15
#define LINE3 PF_3

/**
 * System's default SPI port.
 * */

#define SPI_1_MOSI PB_5
#define SPI_1_MISO PA_6
#define SPI_1_SCK PA_5
#define SPI_1_CS PD_14

/** Check if the script program had a correct pinname
 *   Care needed for keeping this list correct
 */
inline int correctAnOutPinName(const int &mcurrentToken) {
  return (mcurrentToken == TOKEN_PA_4) || (mcurrentToken == TOKEN_PA_5);
}


/**
 *Correct analog IN pin name.
 */
inline int correctAnInPinName(const int &currentToken) {
  switch (currentToken) {
  // ADC1_IN1 ... ADC1_IN8
  case TOKEN_PA_0:
  case TOKEN_PA_1:
  case TOKEN_PA_2:
  case TOKEN_PA_3:
  case TOKEN_PA_4:
  case TOKEN_PA_5:
  case TOKEN_PA_6:
  case TOKEN_PA_7:

  // ADC1_IN9 ... ADC1_IN10
  case TOKEN_PB_0:
  case TOKEN_PB_1:

  // ADC1_IN11 ... ADC1_IN16
  case TOKEN_PC_0:
  case TOKEN_PC_1:
  case TOKEN_PC_2:
  case TOKEN_PC_3:
  case TOKEN_PC_4:
  case TOKEN_PC_5:
    return 1;

  default:
    return 0;
  }
} /**
   * Check if the script program had a correct PWM pin name
   *
   * Care needed for keeping this list correct for STM32L4R5ZI.
   */
inline int correctPWMpinName(int currentToken) {
  switch (currentToken) {
  // Port A
  case TOKEN_PA_0:
  case TOKEN_PA_1:
  case TOKEN_PA_2:
  case TOKEN_PA_3:
  case TOKEN_PA_5:
  case TOKEN_PA_6:
  case TOKEN_PA_7:
  case TOKEN_PA_8:
  case TOKEN_PA_9:
  case TOKEN_PA_10:
  case TOKEN_PA_11:
  case TOKEN_PA_15:

  // Port B
  case TOKEN_PB_0:
  case TOKEN_PB_1:
  case TOKEN_PB_3:
  case TOKEN_PB_4:
  case TOKEN_PB_5:
  case TOKEN_PB_6:
  case TOKEN_PB_7:
  case TOKEN_PB_8:
  case TOKEN_PB_9:
  case TOKEN_PB_10:
  case TOKEN_PB_11:
  case TOKEN_PB_13:
  case TOKEN_PB_14:
  case TOKEN_PB_15:

  // Port C
  case TOKEN_PC_6:
  case TOKEN_PC_7:
  case TOKEN_PC_8:
  case TOKEN_PC_9:

  // Port D
  case TOKEN_PD_12:
  case TOKEN_PD_13:
  case TOKEN_PD_14:
  case TOKEN_PD_15:

  // Port E
  case TOKEN_PE_0:
  case TOKEN_PE_1:
  case TOKEN_PE_3:
  case TOKEN_PE_4:
  case TOKEN_PE_5:
  case TOKEN_PE_6:
  case TOKEN_PE_8:
  case TOKEN_PE_9:
  case TOKEN_PE_10:
  case TOKEN_PE_11:
  case TOKEN_PE_12:
  case TOKEN_PE_13:
  case TOKEN_PE_14:

  // Port F
  case TOKEN_PF_6:
  case TOKEN_PF_7:
  case TOKEN_PF_8:
  case TOKEN_PF_9:

  // Port G
  case TOKEN_PG_9:
  case TOKEN_PG_10:
  case TOKEN_PG_11:

  // Port H
  case TOKEN_PH_10:
  case TOKEN_PH_11:
  case TOKEN_PH_12:
  case TOKEN_PH_13:
  case TOKEN_PH_14:
  case TOKEN_PH_15:
  return 1; 
  default:
    return 0;
  }
}
/**
 * Check if the script program had a correct CAN pin name
 *
 * Care needed for keeping this list correct for STM32L4R5ZI.
 *
 * This device has only one CAN port: FDCAN1.
 */
inline int correctCANpinName(int word) {
  switch (word) {
  // FDCAN1
  case TOKEN_PA_11: // FDCAN1_RX
  case TOKEN_PA_12: // FDCAN1_TX

  case TOKEN_PB_8: // FDCAN1_RX
  case TOKEN_PB_9: // FDCAN1_TX

  case TOKEN_PD_0: // FDCAN1_RX
  case TOKEN_PD_1: // FDCAN1_TX
    return 1;

  default:
    return 0;
  }
}
#endif

#endif
