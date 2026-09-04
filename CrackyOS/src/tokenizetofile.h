/**
 ******************************************************************************
 * @file    tokenizetofile.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief This file contains percompiling mechanizim functions definitions.
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

#ifndef TOKINIZETOFILE_H
#define TOKINIZETOFILE_H

#include "hardware_used.h"

/**
 * Max line size inside the script program.
 */
#define MAX_LINE_SIZE 1024
/**
 * Max token size inside the script program.
 * */
#define MAX_TOKEN_SIZE 256
/**
 * Max INT size. This is used for a buffer to tokinize any decimal number
 * you cannot have a number in text bigger than 14 bytes ..et 123....
 * */
#define MAX_INT_SIZE 14

/**
 * add new item to the scriptProg linked list.
 * It take care of adding and initializing the new item.
 * */
void addNewItemToLinkedList();

/** TokenizeToFile class
 *
 *  TokenizeToFile class implementation
 *
 */
class TokenizeToFile {
public:
  /**
   * Class TokenizeToFile constructor
   *   @param inpuFileName      :    pointer to a file name
   *   @param outputFileName    :    pointer to the new generated file
   * containing the precompiled script
   *
   * */
  TokenizeToFile(const char *inpuFileName, const char *outputFileName);
  /**
   * Class TokinizeToFile destructor
   * */
  ~TokenizeToFile();

  /**
   * Tokinize the string.
   * This function is used for the sd-card script.dat file.
   * It analyze the text and devide it to enum and a text value
   * It saves the reuslt in the global linkedlist (p_prog)
   * */
  int run();
  /**
   * Tokinze a string
   * This function is used for the interactive mode
   * @param inputCommand instruction set that needs to be executed later.
   * */
  int runInteractive(char *inputCommand);
  /**
   * This function will check if there is a number in the text of the script
   * program It results in a TOKEN_NUMBER
   * */
  int checkNumber(void);
  /**
   * This function will check if there is a single variable ..
   * It could result in TOKEN_MINUS, TOKEN_COMMA, ..etc
   * */
  int checkSingelChar(void);
  /**
   *
   * This function will check if there is a string
   * It will result in TOKEN_STRING
   * */
  int checkString(void);
  /**
   * This function will serach for instruction ..
   * If it found it will result in a command ..for ex TOKEN_PUT, TOKEN_GO ..etc
   * */
  int checkInstruction(void);
  /**
   * This will check if the text is a variable.
   * It could result in a TOKEN_VARIABLE.
   * */
  int checkVariable(void);

private:
  /**
   * Internal function to save a line to the SD after analyzing.
   * */
  int saveLine();
  /**
   * Internal function to read a line from SD-Card
   * */

  int readLine();
  /**
   * Pointer that points to the current token word in the script program (before
   * analyzing) ..i.e. a long array
   * */
  const char
      *m_p_prog; // Don't mix with the global p_prog.. This is a local variable
  /**
   * Pointer that points to the next token word in the script program (before
   * analyzing) ..i.e. a long array
   * */
  const char *m_p_prog_next;
  /**
   * Pointer to the test program file on the SD card
   * */
  FILE *testProg;
  /**
   * Pointer to the test program file saves to the SD Card
   * */
  FILE *testProgW;
  /**
   * Pointer to the script program file name
   * */
  const char *inpuFileName;
  /**
   * Pointer to the script analyzed program file name
   * */
  const char *outputFileName;
};

/**
 * A function that counts the no. of lines in a file.
 * @param filename   pointer to a string (filename)
 * */
unsigned int countlines(const char *filename);
/**
 * A function that load a .dad file (analyzed token script program) to the
 * memory of the system
 * @param filename  file name
 * @param lines number of lines
 * */
int readTolinkedList(const char *filename, unsigned int lines);
/**
 * This function will erase the linked list from the memory and free up the RAM
 * */
void free_scriptProg(void);

#endif
