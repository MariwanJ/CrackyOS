/**
 ******************************************************************************
 * @file    nucleo_f207zg_hw.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains all hardware specific definitions
 *          and important variables that are used by the interpreter.
 *          This file should be replaced with a suitable version if the hardware
 *          is changed. This is valid only for NUCLEO F207ZG board
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

#ifndef NUCLEO_F767ZI_HW_h
#define NUCLEO_F767ZI_HW_h

#if TARGET_NUCLEO_F767ZI

#include "mbed.h"

#define SERIAL_TX PD_5
#define SERIAL_RX PD_6

// Debug if enabled:
#define DEBUG_TX PD_8
#define DEBUG_RX PD_9

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
  TOKEN_NULL = 0,   // 0
  TOKEN_EOF,        // 1
  TOKEN_CLEAR,      // 2
  TOKEN_BAD,        // 3
  TOKEN_BEGIN,      // 4
  TOKEN_END,        // 5
  TOKEN_SEC,        // 6
  TOKEN_MSEC,       // 7
  TOKEN_USEC,       // 8
  TOKEN_NC,         // 9
  TOKEN_PA_0,       // 10
  TOKEN_PA_1,       // 11
  TOKEN_PA_2,       // 12
  TOKEN_PA_3,       // 13
  TOKEN_PA_4,       // 14
  TOKEN_PA_5,       // 15
  TOKEN_PA_6,       // 16
  TOKEN_PA_7,       // 17
  TOKEN_PA_8,       // 18
  TOKEN_PA_9,       // 19
  TOKEN_PA_10,      // 20
  TOKEN_PA_11,      // 21
  TOKEN_PA_12,      // 22
  TOKEN_PA_13,      // 23
  TOKEN_PA_14,      // 24
  TOKEN_PA_15,      // 25
  TOKEN_PB_0,       // 26
  TOKEN_PB_1,       // 27
  TOKEN_PB_2,       // 28
  TOKEN_PB_3,       // 29
  TOKEN_PB_4,       // 30
  TOKEN_PB_5,       // 31
  TOKEN_PB_6,       // 32
  TOKEN_PB_7,       // 33
  TOKEN_PB_8,       // 34
  TOKEN_PB_9,       // 35
  TOKEN_PB_10,      // 36
  TOKEN_PB_11,      // 37
  TOKEN_PB_12,      // 38
  TOKEN_PB_13,      // 39
  TOKEN_PB_14,      // 40
  TOKEN_PB_15,      // 41
  TOKEN_PC_0,       // 42
  TOKEN_PC_1,       // 43
  TOKEN_PC_2,       // 44
  TOKEN_PC_3,       // 45
  TOKEN_PC_4,       // 46
  TOKEN_PC_5,       // 47
  TOKEN_PC_6,       // 48
  TOKEN_PC_7,       // 49
  TOKEN_PC_8,       // 50
  TOKEN_PC_9,       // 51
  TOKEN_PC_10,      // 52
  TOKEN_PC_11,      // 53
  TOKEN_PC_12,      // 54
  TOKEN_PC_13,      // 55
  TOKEN_PC_14,      // 56
  TOKEN_PC_15,      // 57
  TOKEN_PD_0,       // 58
  TOKEN_PD_1,       // 59
  TOKEN_PD_2,       // 60
  TOKEN_PD_3,       // 61
  TOKEN_PD_4,       // 62
  TOKEN_PD_5,       // 63
  TOKEN_PD_6,       // 64
  TOKEN_PD_7,       // 65
  TOKEN_PD_8,       // 66
  TOKEN_PD_9,       // 67
  TOKEN_PD_10,      // 68
  TOKEN_PD_11,      // 69
  TOKEN_PD_12,      // 70
  TOKEN_PD_13,      // 71
  TOKEN_PD_14,      // 72
  TOKEN_PD_15,      // 73
  TOKEN_PE_0,       // 74
  TOKEN_PE_1,       // 75
  TOKEN_PE_2,       // 76
  TOKEN_PE_3,       // 77
  TOKEN_PE_4,       // 78
  TOKEN_PE_5,       // 79
  TOKEN_PE_6,       // 80
  TOKEN_PE_7,       // 81
  TOKEN_PE_8,       // 82
  TOKEN_PE_9,       // 83
  TOKEN_PE_10,      // 84
  TOKEN_PE_11,      // 85
  TOKEN_PE_12,      // 86
  TOKEN_PE_13,      // 87
  TOKEN_PE_14,      // 88
  TOKEN_PE_15,      // 89
  TOKEN_PF_0,       // 90
  TOKEN_PF_1,       // 91
  TOKEN_PF_2,       // 92
  TOKEN_PF_3,       // 93
  TOKEN_PF_4,       // 94
  TOKEN_PF_5,       // 95
  TOKEN_PF_6,       // 96
  TOKEN_PF_7,       // 97
  TOKEN_PF_8,       // 98
  TOKEN_PF_9,       // 99
  TOKEN_PF_10,      // 100
  TOKEN_PF_11,      // 101
  TOKEN_PF_12,      // 102
  TOKEN_PF_13,      // 103
  TOKEN_PF_14,      // 104
  TOKEN_PF_15,      // 105
  TOKEN_PG_0,       // 106
  TOKEN_PG_1,       // 107
  TOKEN_PG_2,       // 108
  TOKEN_PG_3,       // 109
  TOKEN_PG_4,       // 110
  TOKEN_PG_5,       // 111
  TOKEN_PG_6,       // 112
  TOKEN_PG_7,       // 113
  TOKEN_PG_8,       // 114
  TOKEN_PG_9,       // 115
  TOKEN_PG_10,      // 116
  TOKEN_PG_11,      // 117
  TOKEN_PG_12,      // 118
  TOKEN_PG_13,      // 119
  TOKEN_PG_14,      // 120
  TOKEN_PG_15,      // 121
  TOKEN_PH_0,       // 122
  TOKEN_PH_1,       // 123
  TOKEN_PH_2,       // 124
  TOKEN_CREATE_REG, // 125
  TOKEN_PUT,        // 126
  TOKEN_GET,        // 127
  TOKEN_APUT,       // 128
  TOKEN_AGET,       // 129
  TOKEN_GEN_PWM,    // 130
  TOKEN_PRINT,      // 131
  TOKEN_SERIAL_GET, // 132
  TOKEN_I2C_SEND,   // 133
  TOKEN_I2C_GET,    // 134
  TOKEN_SPI_SEND,   // 135
  TOKEN_SIZE_OF_VAR,
  TOKEN_SIN,                  // 136
  TOKEN_COS,                  // 137
  TOKEN_TAN,                  // 138
  TOKEN_ASIN,                 // 139
  TOKEN_ACOS,                 // 140
  TOKEN_ATAN,                 // 141
  TOKEN_ABS,                  // 142
  TOKEN_EXP,                  // 143
  TOKEN_LOG,                  // 144
  TOKEN_LN,                   // 145
  TOKEN_ROUND,                // 146
  TOKEN_SQRT,                 // 147
  TOKEN_MOD,                  // 148
  TOKEN_EXIT,                 // 149
  TOKEN_DELETE,               // 150
  TOKEN_FUNCTION,             // 151
  TOKEN_CALL,                 // 152
  TOKEN_DELAY,                // 153
  TOKEN_BAUD_RATE,            // 154
  TOKEN_DEBUG_PORT,           // 155
  TOKEN_TICKER,               // 156
  TOKEN_INTERRUPTIN,          // 157
  TOKEN_COMM_CHANNEL_SERIAL,  // 158
  TOKEN_COMM_CHANNEL_LCD,     // 159
  TOKEN_FOR,                  // 160
  TOKEN_WHILE,                // 161
  TOKEN_IF,                   // 162
  TOKEN_ELSE,                 // 163
  TOKEN_GOTO,                 // 164
  TOKEN_EQUAL,                // 165
  TOKEN_EQUAL_EQUAL,          // 166
  TOKEN_LESSTHAN,             // 167
  TOKEN_LESSTHANEQUAL,        // 168
  TOKEN_GREATERTHAN,          // 169
  TOKEN_GREATERTHANEQUAL,     // 170
  TOKEN_NOTEQUAL,             // 171
  TOKEN_AND,                  // 172
  TOKEN_OR,                   // 173
  TOKEN_INVERT,               // 174
  TOKEN_PLUS,                 // 175
  TOKEN_PLUSPLUS,             // 176
  TOKEN_MINUS,                // 177
  TOKEN_MINUSMINUS,           // 178
  TOKEN_ASTR,                 // 179
  TOKEN_SLASH,                // 180
  TOKEN_BACKSLASH,            // 181
  TOKEN_LEFT_PARENTHESIS,     // 182
  TOKEN_RIGHT_PARENTHESIS,    // 183
  TOKEN_LEFT_CURLY_BRACKET,   // 184
  TOKEN_RIGHT_CURLY_BRACKET,  // 185
  TOKEN_LEFT_SQUARE_BRACKET,  // 186
  TOKEN_RIGHT_SQUARE_BRACKET, // 187
  TOKEN_COMMA,                // 188
  TOKEN_SEMICOLON,            // 189
  TOKEN_CARET,                // 190
  TOKEN_SHIFT_RIGHT,          // 191
  TOKEN_SHIFT_LEFT,           // 192
  TOKEN_LABEL,                // 193
  TOKEN_HASH,                 // 194
  TOKEN_NUMBER,               // 195
  TOKEN_STRING,               // 196
  TOKEN_VARIABLE,             // 197
  TOKEN_REM,                  // 198
  TOKEN_SPI_MODULE,           // 199
  TOKEN_SPI_SINGLE,           // 200
  TOKEN_CAN_SEND,             // 201
  TOKEN_CAN_GET,              // 202
  TOKEN_ENABLE_DEBUG,         // 203
  TOKEN_DISABLE_DEBUG,        // 204
  TOKEN_RETURN,               // 205
  TOKEN_NEW_LINE              // 206
};

/**These GPIO will be used for the CS-decoder of the SPI peripherals.
 * Notice that these pins available only on the male header
 */
// #define LINE0  PG_11
// #define LINE1  PG_13
// #define LINE2  PG_10
// #define LINE3  PG_15

/**These GPIO will be used for the CS-decoder of the SPI peripherals.
 * Notice that these pins available on the Female header
 */
#define LINE0 PB_11
#define LINE1 PB_10
#define LINE2 PE_15
#define LINE3 PE_14

/**
 * System's default SPI port.
 *
 * SPI_MOSI =PB_5 ,
 * SPI_MISO = PA_6,
 * SPI_SCK = PA_5,
 * SPI_CS = PD_14 ,
 * */
#define SPI_1_MOSI PA_7
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
  case TOKEN_PA_0:
  case TOKEN_PA_1:
  case TOKEN_PA_2:
  case TOKEN_PA_3:
  case TOKEN_PA_4:
  case TOKEN_PA_5:
  case TOKEN_PA_6:
  case TOKEN_PA_7:

  case TOKEN_PB_0:
  case TOKEN_PB_1:

  case TOKEN_PC_0:
  case TOKEN_PC_1:
  case TOKEN_PC_2:
  case TOKEN_PC_3:
  case TOKEN_PC_4:
  case TOKEN_PC_5:

  case TOKEN_PF_3:
  case TOKEN_PF_4:
  case TOKEN_PF_5:
  case TOKEN_PF_6:
  case TOKEN_PF_7:
  case TOKEN_PF_8:
  case TOKEN_PF_9:
  case TOKEN_PF_10:
    return 1;

  default:
    return 0;
  }
} /**
   * Check if the script program has a correct PWM pin name.
   *
   * STM32F767ZI:
   * Valid pins are GPIOs having a TIMx_CHy or TIMx_CHyN
   * alternate function.
   */
inline int correctPWMpinName(int currentToken) {
  switch (currentToken) {
  // --------------------
  // Port A
  // --------------------
  case TOKEN_PA_0:  // TIM2_CH1 / TIM5_CH1
  case TOKEN_PA_1:  // TIM2_CH2 / TIM5_CH2
  case TOKEN_PA_2:  // TIM2_CH3 / TIM5_CH3 / TIM9_CH1
  case TOKEN_PA_3:  // TIM2_CH4 / TIM5_CH4 / TIM9_CH2
  case TOKEN_PA_5:  // TIM2_CH1
  case TOKEN_PA_6:  // TIM3_CH1 / TIM13_CH1
  case TOKEN_PA_7:  // TIM3_CH2 / TIM14_CH1
  case TOKEN_PA_8:  // TIM1_CH1
  case TOKEN_PA_9:  // TIM1_CH2
  case TOKEN_PA_10: // TIM1_CH3
  case TOKEN_PA_11: // TIM1_CH4
  case TOKEN_PA_15: // TIM2_CH1

  // --------------------
  // Port B
  // --------------------
  case TOKEN_PB_0:  // TIM3_CH3
  case TOKEN_PB_1:  // TIM3_CH4
  case TOKEN_PB_3:  // TIM2_CH2
  case TOKEN_PB_4:  // TIM3_CH1
  case TOKEN_PB_5:  // TIM3_CH2
  case TOKEN_PB_6:  // TIM4_CH1
  case TOKEN_PB_7:  // TIM4_CH2
  case TOKEN_PB_8:  // TIM4_CH3 / TIM10_CH1
  case TOKEN_PB_9:  // TIM4_CH4 / TIM11_CH1
  case TOKEN_PB_10: // TIM2_CH3
  case TOKEN_PB_11: // TIM2_CH4
  case TOKEN_PB_13: // TIM1_CH1N
  case TOKEN_PB_14: // TIM1_CH2N / TIM12_CH1
  case TOKEN_PB_15: // TIM1_CH3N / TIM12_CH2

  // --------------------
  // Port C
  // --------------------
  case TOKEN_PC_6: // TIM3_CH1 / TIM8_CH1
  case TOKEN_PC_7: // TIM3_CH2 / TIM8_CH2
  case TOKEN_PC_8: // TIM3_CH3 / TIM8_CH3
  case TOKEN_PC_9: // TIM3_CH4 / TIM8_CH4

  // --------------------
  // Port D
  // --------------------
  case TOKEN_PD_12: // TIM4_CH1
  case TOKEN_PD_13: // TIM4_CH2
  case TOKEN_PD_14: // TIM4_CH3
  case TOKEN_PD_15: // TIM4_CH4

  // --------------------
  // Port E
  // --------------------
  case TOKEN_PE_5:  // TIM9_CH1
  case TOKEN_PE_6:  // TIM9_CH2
  case TOKEN_PE_8:  // TIM1_CH1N
  case TOKEN_PE_9:  // TIM1_CH1
  case TOKEN_PE_10: // TIM1_CH2N
  case TOKEN_PE_11: // TIM1_CH2
  case TOKEN_PE_12: // TIM1_CH3N
  case TOKEN_PE_13: // TIM1_CH3
  case TOKEN_PE_14: // TIM1_CH4

  // --------------------
  // Port F
  // --------------------
  case TOKEN_PF_6: // TIM10_CH1
  case TOKEN_PF_7: // TIM11_CH1
  case TOKEN_PF_8: // TIM13_CH1
  case TOKEN_PF_9: // TIM14_CH1
    return 1;

  default:
    return 0;
  }
}
/**
 * Check if the script program has a correct CAN pin name.
 *
 * STM32F767ZI has three CAN peripherals:
 *
 * CAN1:
 *   PA11 = CAN1_RX
 *   PA12 = CAN1_TX
 *   PB8  = CAN1_RX
 *   PB9  = CAN1_TX
 *   PD0  = CAN1_RX
 *   PD1  = CAN1_TX
 *
 * CAN2:
 *   PB5  = CAN2_RX
 *   PB6  = CAN2_TX
 *   PB12 = CAN2_RX
 *   PB13 = CAN2_TX
 *
 * CAN3:
 *   PA8  = CAN3_RX
 *   PA15 = CAN3_TX
 *   PB3  = CAN3_RX
 *   PB4  = CAN3_TX
 */
inline int correctCANpinName(int word) {
  switch (word) {
  // CAN1
  case TOKEN_PA_11:
  case TOKEN_PA_12:
  case TOKEN_PB_8:
  case TOKEN_PB_9:
  case TOKEN_PD_0:
  case TOKEN_PD_1:

  // CAN2
  case TOKEN_PB_5:
  case TOKEN_PB_6:
  case TOKEN_PB_12:
  case TOKEN_PB_13:

  // CAN3
  case TOKEN_PA_8:
  case TOKEN_PA_15:
  case TOKEN_PB_3:
  case TOKEN_PB_4:
    return 1;

  default:
    return 0;
  }
}

#endif

#endif
