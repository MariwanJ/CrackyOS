/**
 *******************************************************************************
 * @file    cat.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief  This file contains definitions of file operations (create, cat,..etc
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
#ifndef _CAT_H
#define _CAT_H

#include "sdcard.h"
#include "tokens.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
/**
 * Print Help message
 * */
void printHelp();

/**
 * Convert a string to lower case.
 * @param me  a pointer to a string that should be converted to lower case.
 * */
void ToLower(char *me);
/*
 *
 * String inside script program needs to be fixed. \n will get extra \ ..we
 removed in this function.
 * @param me  a pointer to a string that should be fixed
 *
 char* fixString(const char* me);
 */

/**
 * Write the content of a file to terminal.
 * @param fname  file name which should be printed on the terminal.
 * */
void printfile(const char *fname);

/**
 * CAT command implementation'
 * @param argc  int value .. describes how many arguments are there
 * @param argv  two dimensional array contains the command, arguments.
 * */
void Do_Cat(int argc, char **argv);

#endif
