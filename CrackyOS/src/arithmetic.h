/**
 ******************************************************************************
 * @file    arithmetic.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains the arithmetic operations functions and methods
 *           used by the interpreter
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
#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include "tokenizetofile.h"
#include "tokens.h"
#include "types.h"

/**
 *This function will be used to do a comparison between two values and an
 *operation. Use this function when you need to compare two values (grater,
 *less, equal ..etc).
 *@param Oldvalue        Value to be compared
 *@param ComparisonValue Value will be compared to
 *@param enumOperation   comparison type ( >, <, =<, =>, !=, ==)
 */
unsigned char GetLogicalOperationAnswer(int enumOperation, int Oldvalue,
                                        int ComparisionValue);

/**
*This function will be used to do logical operation on two values (OR, AND, NOT,
INVERT)

*@param val1        Value to be compared
*@param val2        Value will be compared to
*@param operation   Digital operation supported (OR, AND, NOT, INVERT, SHIFT
RIGHT & SHIFT LEFT)
*@date
*/
void CalculateLogicalOperation(int &val1, int val2, int operation);

/**
 *Raise to power. This function will calculate x raised to the power y
 *@param x       The base
 *@param y       The exponent
 */
float CalculatePower(float x, float y);

/**A function to calculate division or multiplication in the script program.
 *Only PLUS/MINUS/MULTIPLICATION/DIVISION  and some logical operation are
 *Implemented.
 *@return answer for the division or multiplication section of the script
 *program
 */

float divide_multiply(void);

/**
*A function that returns the next found variable/value inside the script.
This function is used by the interpreter for calculating arithmetic expression
*@param No parameter is given to the function since it reads the script program.
*@return next number/variable inside the script program
*@date
*/
inline float get_next_number_or_variable_value();

/**
 *This function evaluates any arithmetic expression
 *@param p_strat   a pointer to the BEGIN place of the expression in the script
 *program
 *@param p_end     a pointer to the END place of the expression in the script
 *program
 *@return the result of the expression executed
 *@date
 */

float arithmetic_expression(instr_linkedList *p_strat, instr_linkedList *p_end);

/**
 *This function find out BEGIN/END positions of the arithmetic operation inside
 *the script program. The function is used by the interpreter.
 *@param     No parameter needed since it reads the script program.
 *@return the result of the arithmetic operations done in the right side of the
 *equal sign.
 *@date
 */

float CalculateRightSideofVariableEquation(void);
float CalculatSpecialFunction(void);

/**
 *This function will determine if the variable is a string variable or not.
 *@param     No parameter needed since it reads the script program.
 *@return 1 if the variable was string, 0 if not.
 *@date
 */

unsigned char CheckIfTheRightSideIsVarOfArrayString(void);

/**
 *This function will make all string operation needed after the equal sign,
 *Warning: Mixing string and numeric operation is not allowed
 *@param  INT value which is the position of the global script variable
 *@return nothing.
 *@date
 */
void CalculateRightSideofVariableEquation_TEXT(int varPosition);

/**
 *This function will determine if we have an logical arithmetic operation
 *@param  INT current token word
 *@return results .. if we have any of them the result will be 1
 *@date
 */

unsigned char CheckforLogicalArithmeticOperations(int m_Tokenword);

// Functions to make it easier to read the code

float get_next_number_or_variable_value_Parenthesis();
float get_next_number_or_variable_value_Number();
float get_next_number_or_variable_value_SizeOfVar();
float get_next_number_or_variable_value_Variable();

#endif
