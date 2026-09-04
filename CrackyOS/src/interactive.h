/**
 ******************************************************************************
 * @file    interactive.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief
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
#ifndef INTERACTIVE_H
#define INTERACTIVE_H
#include "types.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
extern std::string SCRIPT_PROG_NAME;
extern std::string SCRIPT_PROG_NAME_DAD;

#define DEFAULT_DOT_DAT_FILENAME "/sd/script.dat";
#define DEFAULT_DOT_DAD_FILENAME "/sd/script.dad";

/**
 *  This function will read the configuration saved on the SD-Disk
 *  If the configuration file is not there it will return the following
 *  integer
 *   -1      : DefaultFile     is not found or bad
 *   -2      : BaudRate        is not found or bad
 *   -3      :InterActiveMode  is not found or bad
 *
 *   -10     : File not found
 *   -20     : File size is wrong. The file is corrupted.
 *
 *   In all these cases, default configuration file should be re-created.
 */

int readConfigFile(void);
/**
 * Max length of buffer used for config file
 * */
#define MAX_LENGTH_OF_CONF_FILE 256

/**
 * Max length of buffer used for file path
 * */
#define PATH_SIZE 1024
/**
 * Max length of buffer used for file name
 * */

#define FILE_NAME 1035

/**
 * Max length of buffer used for com port name (for ex. com1, com2 ..etc)
 * */
#define COMPORT_NAME_SIZE 10

/**
 * Default configuration file name - hard-coded
 * */

#define DEFAULT_CONFIG_FILE_NAME "/sd/config.ini"

/**
 * Create config.ini with default values.
 */
int writeConfigToFile();

/**
 * Used to run a known script file.
 * */
int RunScriptFile(void);
/**
 * Determines if the string is a comments.
 * */
inline bool IsComment(std::string &);
#endif
