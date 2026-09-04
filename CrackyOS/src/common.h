/**
 ******************************************************************************
 * @file    common.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains some functions and definitions.
 * and important variables that are used by the interpreter.
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

#ifndef _COMMON_H_
#define _COMMON_H_

#include "SD/SDBlockDevice.h"
#include <math.h>
#include <mbed.h>

#include <ctype.h> /* toupper */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strcpy */

#include <cstdarg>
#include <stdarg.h>

#include "interrupt_in.h"
#include "ringbuffer.h"

#include "arithmetic.h"
#include "i2c_hw.h"
#include "sdcard.h"
#include "serialmessage.h"
#include "spi_hw.h"
#include "ticker_hw.h"
#include "tokenizetofile.h"
#include "tokens.h"
#include "types.h"

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

/** IF statment*/
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

#endif /* SRC_COMMON_H_ */
