/**
 ******************************************************************************
 * @file    serialmessage.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   Serial communication interface
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

#ifndef SERIALMESSAGE_H
#define SERIALMESSAGE_H

#include "hardware_used.h"
#include "interactive.h"

/** Max size of the ring buffer used by debug printf function*/
#define INTER_ACTIVE_MODE_MAX_LENGTH_OF_THE_STRING 2 * 1024 // 2k bytes

/** Font Color code for terminal*/
#define FONT_BLACK_CODE "[30m"   // 0
#define FONT_RED_CODE "[31m"     // 1
#define FONT_GREEN_CODE "[32m"   // 2
#define FONT_YELLOW_CODE "[33m"  // 3
#define FONT_BLUE_CODE "[34m"    // 4
#define FONT_MAGENTA_CODE "[35m" // 5
#define FONT_CYNA_CODE "[36m"    // 6
#define FONT_WHITE_CODE "[37m"   // 7
#define FONT_DEFAULT_CODE "[39m" // 10

#define BKG_BLACK_CODE "[40m"   // 0
#define BKG_RED_CODE "[41m"     // 1
#define BKG_GREEN_CODE "[42m"   // 2
#define BKG_YELLOW_CODE "[43m"  // 3
#define BKG_BLUE_CODE "[44m"    // 4
#define BKG_MAGENTA_CODE "[45m" // 5
#define BKG_CYNA_CODE "[46m"    // 6
#define BKG_WHITE_CODE "[47m"   // 7
#define BKG_DEFAULT_CODE "[49m" // 10

/**
 * This enum is used to give color to the text printed on the terminal.
 * */
enum {
  FONT_BLACK = 0,
  FONT_READ = 1,
  FONT_GREEN = 2,
  FONT_YELLOW = 3,
  FONT_BLUE = 4,
  FONT_MAGNETA = 5,
  FONT_CYNA = 6,
  FONT_WHITE = 7,
  FONT_DEFAULT = 10,

  BKG_BLACK = 20,
  BKG_READ = 21,
  BKG_GREEN = 22,
  BKG_YELLOW = 23,
  BKG_BLUE = 24,
  BKG_MAGNETA = 25,
  BKG_CYNA = 26,
  BKG_WHITE = 27,
  BKG_DEFAULT = 30
};

/** Thread that take care of serial interrupt to show the Menu on terminal
 * YOU MUST KEEP IT SHORT .. NO SD-Card activity allowed here. */
void ChangeInterActiveModeStatus();

/** clear ALL screen of the terminal*/
void clearAndHome();

/** clear screen of the Serial-terminal*/
void clearAndHomeSerial();

/** clear screen of the DEBUG-terminal*/
void clearAndHomeDEBUG();

/** Get a text from the serial terminal (not debug)*/
char *GET_SERIAL_TEXT(void);

/** Recieve the script program using serial terminal*/
void getScriptProgramFile();

/** User interface - Serial terminal*/
void RUN_MENU(void);

/** Draw/write the interface menu- Serial terminal*/
void print_help(void);

/** Get text of size [MAX_LENGTH_OF_THE_STRING] from the serial*/
void GetStringFromSerial(char *stringBuf);

/** Thread to run the Menu*/
void SerialThreadFunction(void);
/** Folder operation */
void do_FolderOperations(void);
/** give color to text on terminal*/
void ChangeTerminalColor(int color);

/** This function will allow the user to interactively send instruction to the
 * system*/
int interactive_mode(void);
#endif
