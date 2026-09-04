/**
 ******************************************************************************
 * @file    nucleo_l476rg_hw.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains all hardware specific definitions
 *          and important variables that are used by the interpreter.
 *          This file should be replaced with a suitable version if the hardware
 *          is changed. This is valid only for NUCLEO L476RG board
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

#ifndef NUCLEO_L476RG_HW_H
#define NUCLEO_L476RG_HW_H

#if TARGET_NUCLEO_L476RG
#include "SD/SDBlockDevice.h"
#include "mbed.h"
#include <math.h>

// Interactive Terminal
#define SERIAL_TX PA_2
#define SERIAL_RX PA_3

// Debug if enabled:
#define DEBUG_TX PC_10
#define DEBUG_RX PC_11

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

  TOKEN_NULL = 0,                 // 0
  TOKEN_EOF,                      // 1
  TOKEN_CLEAR,                    // 2
  TOKEN_BAD,                      // 3
  TOKEN_BEGIN,                    // 4
  TOKEN_END,                      // 5
  TOKEN_SEC,                      // 6
  TOKEN_MSEC,                     // 7
  TOKEN_USEC,                     // 8
  TOKEN_NC,                       // 9

  TOKEN_PA_0,                     // 10
  TOKEN_PA_1,                     // 11
  TOKEN_PA_2,                     // 12
  TOKEN_PA_3,                     // 13
  TOKEN_PA_4,                     // 14
  TOKEN_PA_5,                     // 15
  TOKEN_PA_6,                     // 16
  TOKEN_PA_7,                     // 17
  TOKEN_PA_8,                     // 18
  TOKEN_PA_9,                     // 19
  TOKEN_PA_10,                    // 20
  TOKEN_PA_11,                    // 21
  TOKEN_PA_12,                    // 22
  TOKEN_PA_13,                    // 23
  TOKEN_PA_14,                    // 24
  TOKEN_PA_15,                    // 25

  TOKEN_PB_0,                     // 26
  TOKEN_PB_1,                     // 27
  TOKEN_PB_2,                     // 28
  TOKEN_PB_3,                     // 29
  TOKEN_PB_4,                     // 30
  TOKEN_PB_5,                     // 31
  TOKEN_PB_6,                     // 32
  TOKEN_PB_7,                     // 33
  TOKEN_PB_8,                     // 34
  TOKEN_PB_9,                     // 35
  TOKEN_PB_10,                    // 36
  TOKEN_PB_11,                    // 37
  TOKEN_PB_12,                    // 38
  TOKEN_PB_13,                    // 39
  TOKEN_PB_14,                    // 40
  TOKEN_PB_15,                    // 41
  TOKEN_PC_0,                     // 42
  TOKEN_PC_1,                     // 43
  TOKEN_PC_2,                     // 44
  TOKEN_PC_3,                     // 45
  TOKEN_PC_4,                     // 46
  TOKEN_PC_5,                     // 47
  TOKEN_PC_6,                     // 48
  TOKEN_PC_7,                     // 49
  TOKEN_PC_8,                     // 50
  TOKEN_PC_9,                     // 51
  TOKEN_PC_10,                    // 52
  TOKEN_PC_11,                    // 53
  TOKEN_PC_12,                    // 54
  TOKEN_PC_13,                    // 55
  TOKEN_PC_14,                    // 56
  TOKEN_PC_15,                    // 57
  TOKEN_PD_0,                     // 58
  TOKEN_PD_1,                     // 59
  TOKEN_PD_2,                     // 60
  TOKEN_PH_0,                     // 61
  TOKEN_PH_1,                     // 62
  TOKEN_CREATE_REG,               // 63
  TOKEN_PUT,                      // 64
  TOKEN_GET,                      // 65
  TOKEN_APUT,                     // 66
  TOKEN_AGET,                     // 67
  TOKEN_GEN_PWM,                  // 68
  TOKEN_PRINT,                    // 69
  TOKEN_SERIAL_GET,               // 70
  TOKEN_I2C_SEND,                 // 71
  TOKEN_I2C_GET,                  // 72
  TOKEN_SPI_SEND,                 // 73
  TOKEN_SIZE_OF_VAR,              // 74
  TOKEN_SIN,                      // 75
  TOKEN_COS,                      // 76
  TOKEN_TAN,                      // 77
  TOKEN_ASIN,                     // 78
  TOKEN_ACOS,                     // 79
  TOKEN_ATAN,                     // 80
  TOKEN_ABS,                      // 81
  TOKEN_EXP,                      // 82
  TOKEN_LOG,                      // 83
  TOKEN_LN,                       // 84
  TOKEN_ROUND,                    // 85
  TOKEN_SQRT,                     // 86
  TOKEN_MOD,                      // 87
  TOKEN_EXIT,                     // 88
  TOKEN_DELETE,                   // 89
  TOKEN_FUNCTION,                 // 90
  TOKEN_CALL,                     // 91
  TOKEN_DELAY,                    // 92
  TOKEN_BAUD_RATE,                // 93
  TOKEN_DEBUG_PORT,               // 94
  TOKEN_TICKER,                   // 95
  TOKEN_INTERRUPTIN,              // 96
  TOKEN_COMM_CHANNEL_SERIAL,      // 97
  TOKEN_COMM_CHANNEL_LCD,         // 98
  TOKEN_FOR,                      // 99
  TOKEN_WHILE,                    // 100
  TOKEN_IF,                       // 101
  TOKEN_ELSE,                     // 102
  TOKEN_GOTO,                     // 103
  TOKEN_EQUAL,                    // 104
  TOKEN_EQUAL_EQUAL,              // 105
  TOKEN_LESSTHAN,                 // 106
  TOKEN_LESSTHANEQUAL,            // 107
  TOKEN_GREATERTHAN,              // 108
  TOKEN_GREATERTHANEQUAL,         // 109
  TOKEN_NOTEQUAL,                 // 110
  TOKEN_AND,                      // 111
  TOKEN_OR,                       // 112
  TOKEN_INVERT,                   // 113
  TOKEN_PLUS,                     // 114
  TOKEN_PLUSPLUS,                 // 115
  TOKEN_MINUS,                    // 116
  TOKEN_MINUSMINUS,               // 117
  TOKEN_ASTR,                     // 118
  TOKEN_SLASH,                    // 119
  TOKEN_BACKSLASH,                // 120
  TOKEN_LEFT_PARENTHESIS,         // 121
  TOKEN_RIGHT_PARENTHESIS,        // 122
  TOKEN_LEFT_CURLY_BRACKET,       // 123
  TOKEN_RIGHT_CURLY_BRACKET,      // 124
  TOKEN_LEFT_SQUARE_BRACKET,      // 125
  TOKEN_RIGHT_SQUARE_BRACKET,     // 126
  TOKEN_COMMA,                    // 127
  TOKEN_SEMICOLON,                // 128
  TOKEN_CARET,                    // 129
  TOKEN_SHIFT_RIGHT,              // 130
  TOKEN_SHIFT_LEFT,               // 131
  TOKEN_LABEL,                    // 132
  TOKEN_HASH,                     // 133
  TOKEN_NUMBER,                   // 134
  TOKEN_STRING,                   // 135
  TOKEN_VARIABLE,                 // 136
  TOKEN_REM,                      // 137
  TOKEN_SPI_MODULE,               // 138
  TOKEN_SPI_SINGLE,               // 139
  TOKEN_CAN_SEND,                 // 140
  TOKEN_CAN_GET,                  // 141
  TOKEN_ENABLE_DEBUG,             // 142
  TOKEN_DISABLE_DEBUG,            // 143
  TOKEN_RETURN,                   // 144
  TOKEN_NEW_LINE                  // 145
};


/**These GPIO will be used for the CS-decoder of the SPI peripherals.
 */
#define LINE0 PC_3
#define LINE1 PC_2
#define LINE2 PH_1
#define LINE3 PH_0

/**
 * System's default SPI port.
 * */
#define SPI_1_MOSI PA_7
#define SPI_1_MISO PA_6
#define SPI_1_SCK PA_5
#define SPI_1_CS PB_6

/** SD block device variable */
extern SDBlockDevice sd;

extern Timer TestInstrSpee;
/**
 * Global pointer keeps the CAN variable.
 * */
extern canType *gCanVariable;

/**
 * Global variable keeps the GPIO variables
 * */
extern GlobalGP *globalGPIOvariable;

/** This function will initialize the script program*/
void system_script_init(void);

/**
 * This function will execute either the entire script program (if NULL is given
 * as a parameter), or it will run the script program from (p_prog) to the
 * toPosition pointer This should be quick
 * @param  toPosition:   if NULL is send to this function, all script program
 * line will be executed, otherwise the script will be executed to the position
 * where toPositio points to.
 */
int Execute_Instructions(instr_linkedList *toPosition);

/**
 * Checks if it is not the end of the script program,
 * and lunch Execute_Instructions with NULL argument (run the all lines in the
 * script program)
 */
int scriptSystem_script_run(void);

/**
 * This function will return the pinname of the given pinname in text
 *
 */
PinName retrievePinNameFromTOKEN_NAME(int TextEnum);

/** Creates a Reg from different GPIO pins*/
int token_create_reg(void);

/** Use this function to put data to a GPIO pin*/
int token_put(void);

/** Use this function to read data from a GPIO pin. The function will return
the value, therefore you need to use a variable to save the data.*/
int token_get(void);

/**Analog version of in/out voltage in / out ::
 *Note: This function will not accept REG.. It should be a single GPIO*/
int token_aput(void);
int token_aget(void);

/*Delay function.*/
int token_delay(void);

/** Function implementation*/
int token_function(void);

/** Variable definition function used by the script*/
int token_variable(void);

/** Change the baud_rate of the system*/
int token_baud_rate(void);
/** Send message to serial terminal*/
int token_print(void);
/** Get data from Serial terminal. You should use a variable to get the data*/
int token_serial_get(void);

/** Loop instructions. */
int token_for_loop();
int token_while_loop();

/** Remark instruction. The text after this instruction will be skipped*/
int token_rem();
/** SPI instruction*/
int token_spi_send();

/** CAN send instruction*/
int token_can_send();

/** CAN get instruction*/
int token_can_get();

/** Generate PWM instruction*/
int token_gen_pwm(void);
/** Internal function to fix pin name problem*/

/** Enable debug*/
int token_enable_debug(void);
/** disable debug*/
int token_disable_debug(void);

/** IF statement*/
int token_if(void);

/** GOTO to new position*/
int token_goto(void);

/** Clear the terminal screen.*/
unsigned char token_serial_screen_clear();
/** Create new bus reg*/
int createNewBus(int type);

/** Create a new CAN port definition
 *This is an internal function.
 */
int createNewCanDefinition();

/**
 * Send message to I2C
 */
int token_i2c_send(void);

/**
 * GET message to I2C
 */
int token_i2c_get(void);

/** exit/quit instruction
 *Syntax:
 *   exit;
 * or
 *   quit;
 */
int token_exit(void);

/** CALL of a function arg1, arg2 ..argn
 *Syntax:
 *   call function_name arg1, arg2 ..argn;
 *OR
 *   call function_name  //No arguments
 *Note:
 *       Keep in mind that you must define the function in the top side of the
 *script program before the executable codes. And the arguments are byValue ..
 *Values will be transferred to the function ..not variables.
 */
int token_call(void);

/** Delete global variable and free mem
 *Syntax:
 * delete VAR_NAME;
 */
int token_delete(void);

/**
 *
 * @return -1 if error , >0 if OK
 * Should run a function when interrupt occur
 * */
int token_interruptIn(void);

/** Check if the script program had a correct pinname
 *   Care needed for keeping this list correct
 */
inline int correctAnOutPinName(int mcurrentToken) {
  return (mcurrentToken == TOKEN_PA_4) || (mcurrentToken == TOKEN_PA_5);
}
/**
 *Correct analog IN pin name.
 */
inline int correctAnInPinName(int currentToken) {
  switch (currentToken) {
  // PA0 - PA7
  case TOKEN_PA_0:
  case TOKEN_PA_1:
  case TOKEN_PA_2:
  case TOKEN_PA_3:
  case TOKEN_PA_4:
  case TOKEN_PA_5:
  case TOKEN_PA_6:
  case TOKEN_PA_7:

  // PB0 - PB1
  case TOKEN_PB_0:
  case TOKEN_PB_1:

  // PC0 - PC5
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
}
/**
 * Check if the script program had a correct CAN pin name
 *
 * Care needed for keeping this list correct for STM32L476RG.
 *
 * This device has one CAN port.
 */
inline int correctCANpinName(int word) {
  switch (word) {
  // CAN1
  case TOKEN_PA_11: // CAN_RX
  case TOKEN_PA_12: // CAN_TX

  case TOKEN_PB_8: // CAN_RX
  case TOKEN_PB_9: // CAN_TX

  case TOKEN_PD_0: // CAN_RX
  case TOKEN_PD_1: // CAN_TX
    return 1;

  default:
    return 0;
  }
}

/**
 * Check if the script program had a correct PWM pin name
 *
 * Care needed for keeping this list correct for STM32L476RG.
 */
inline int correctPWMpinName(int currentToken) {
  switch (currentToken) {
  // Port A
  case TOKEN_PA_0:  // TIM2_CH1 / TIM5_CH1
  case TOKEN_PA_1:  // TIM2_CH2 / TIM5_CH2 / TIM15_CH1N
  case TOKEN_PA_2:  // TIM2_CH3 / TIM5_CH3 / TIM15_CH1
  case TOKEN_PA_3:  // TIM2_CH4 / TIM5_CH4 / TIM15_CH2
  case TOKEN_PA_5:  // TIM2_CH1 / TIM8_CH1N
  case TOKEN_PA_6:  // TIM3_CH1 / TIM16_CH1
  case TOKEN_PA_7:  // TIM1_CH1N / TIM3_CH2 / TIM8_CH1N / TIM17_CH1
  case TOKEN_PA_8:  // TIM1_CH1
  case TOKEN_PA_9:  // TIM1_CH2
  case TOKEN_PA_10: // TIM1_CH3
  case TOKEN_PA_11: // TIM1_CH4
  case TOKEN_PA_15: // TIM2_CH1

  // Port B
  case TOKEN_PB_0:  // TIM1_CH2N / TIM3_CH3 / TIM8_CH2N
  case TOKEN_PB_1:  // TIM1_CH3N / TIM3_CH4 / TIM8_CH3N
  case TOKEN_PB_3:  // TIM2_CH2
  case TOKEN_PB_4:  // TIM3_CH1
  case TOKEN_PB_5:  // TIM3_CH2
  case TOKEN_PB_6:  // TIM4_CH1 / TIM16_CH1N
  case TOKEN_PB_7:  // TIM4_CH2 / TIM17_CH1N
  case TOKEN_PB_8:  // TIM4_CH3 / TIM16_CH1
  case TOKEN_PB_9:  // TIM4_CH4 / TIM17_CH1
  case TOKEN_PB_10: // TIM2_CH3
  case TOKEN_PB_11: // TIM2_CH4
  case TOKEN_PB_13: // TIM1_CH1N / TIM15_CH1N
  case TOKEN_PB_14: // TIM1_CH2N / TIM8_CH2N / TIM15_CH1
  case TOKEN_PB_15: // TIM1_CH3N / TIM8_CH3N / TIM15_CH2

  // Port C
  case TOKEN_PC_6: // TIM3_CH1 / TIM8_CH1
  case TOKEN_PC_7: // TIM3_CH2 / TIM8_CH2
  case TOKEN_PC_8: // TIM3_CH3 / TIM8_CH3
  case TOKEN_PC_9: // TIM3_CH4 / TIM8_CH4

    return 1;

  default:
    return 0;
  }
}

#endif

#endif
