/**
 ******************************************************************************
 * @file    tokens.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   All script program decoding and analyzing done here.
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

#ifndef TOKENS_H
#define TOKENS_H

#include "interactive.h"
#include "sourcecodeline.h"
#include "types.h"

/** Setup the pointers and initialize it to the start of the program.
 *@return -1 if something wrong happened otherwise >=0
 */
int token_initialize(void);

/**Read the code, find out the next part of the code, with syntax checking
 *@return -1 if error append, otherwise >=0
 */
int check_and_get_next_token(void);

/*
 * Goto a new position. Jumping the code to execute new place in the program.
 * Warning: Before calling this, make sure you have current_token_word=
 * TOKEN_BAD
 * */
inline void token_jump_to(instr_linkedList *newPosition) {
  p_prog = p_prog_next = newPosition;
  check_and_get_next_token();
}

/**Extract the string and return the pointer.*/
char *token_extract_string(void);

/** convert digital text to real decimal numbers.*/
float convert_text_to_number(void);

/**Change the program to upper text.*/
int change_program_to_lower_case(char *prog_pointer);

/**Take the name of the variable from the program script and find the position
of the variable in the global variables.
*
*@param varName is null pointer OR A NAME
*/
int TestSystem_get_variable(char *varName);
/**This function will try to find an empty position in the global variable*/
inline int TestSystem_get_empty_variable_pos(void) {
  for (int i = 0; i < SCRIPT_MAX_VARIABLE_NUMBERS; i++) {
    if (script_variables[i].varname == NULL)
      return i;
  }
  return -1; // Not found Overflow
}

/** Check if a byte is a symbol or not
 *param  a char to be checked
 *Return 1 if the char is a symbol, 0 if not.
 */
inline unsigned char isSpecialChar(char const *tempP_PROG) {
  if (tempP_PROG == NULL)
    return 0;
  switch (*tempP_PROG) {
  case ':':
  case ',':
  case ';':
  case '*':
  case '+':
  case '-':
  case '&':
  case '~':
  case '^':
  case '|':
  case '/':
  case '\\':
  case '%':
  case '(':
  case ')':
  case '<':
  case '>':
  case '=':
  case '{':
  case '}':
  case '[':
  case ']':
  case '#':
  case '!':
  case '\"':
  case '\n':
  case '\r':
  case '\t': {
    return 1;
  }
  default: {
    return 0;
  }
  }
}

/** If you give a name to the function it will delete only that variable, if you
 *give NULL it will delete all
 *@param  nameOfVariable : Variable to delete from memory, if =NULL, all
 *variables will be deleted
 */
void IntializeScriptVariable(char *nameOfVariable);
/**
 * Free up mem from PWM variable
 * */
void free_PWM_Var(void);
/**
 * Free up mem from forExtract variable
 * */

void free_forExtract(void);

/**
 * Free up mem from whileExtract variable
 * */

void free_whileExtract(void);

/**
 *  Free Label variable
 * */

void free_globallabelVar(void);

/**
 * Free up mem from all variables.
 * */
void CleanUpMemToOriginal(void);

/**
 * Free up mem from all variables.
 * */
void freeUp_ScriptVariable(); // ScriptVariable.

/** Initialize the script program for interactive mode*/
int token_initializeInterActive(void);

/**
 * Free up mem from GlobalGPIO_REG
 * */
int free_GlobalGPIO_REG();

/**
 * Free up mem for CAN variable
 * */
int freeUp_gCanVariable();
/** Change the string received as parameter to upper case
 *@param s  string to change the case to upper case.
 */
void capitalize(char *s);

/** Extract a float number from a string
 * @param stringToFloat   A string to convert to float.
 */
inline float extractFloat(char *stringToFloat) {
  // Stop running if we have error
  float Result = 0;
  float fact = 1;
  if (*stringToFloat == '-') {
    stringToFloat++;
    fact = -1;
  };
  for (int pointFound = 0; *stringToFloat; stringToFloat++) {
    if (*stringToFloat == '.') {
      pointFound = 1;
      continue;
    };
    int d = *stringToFloat - '0';
    if (d >= 0 && d <= 9) {
      if (pointFound)
        fact *= 0.1000000000f;
      Result = Result * 10.0000000000f + (float)d;
    };
  };
  return (Result * fact);
}

/**Find the end of the number inside a string.. could be hex, or dec, or binary
 *
 *@param testByte pointer to the script program
 */
inline int endOfNumber(char const *testByte) {
  if (isSpecialChar(testByte) || *testByte == ' ')
    return 1;
  else
    return 0;
}

/** This function will take care of removing extra '\' symbol from a string text
 * The problem is du to the fact that \n inside a text becomes \\n and it will
 *be counted as a normal text not a new line command.
 *@param inStr  A pointer to a string
 */
void FixStringIssue(char *inStr);

/** This function will save the position of the lable used in the script.
 *
 *\return size of the lable_name
 */
int SavelabelPositions(void);

/**
 *  \brief Function that retrieve a value from the global script variable
 *  @param  No parameter needed.
 *  \return Value from global script variables
 *  \details Use this function whenever you need to retrieve a value from the
 * script program.
 */
float RetrieveValueInGlobalVariable(void);

/**
 *  \brief Function that retrieve the last position in the current line of code
 *  @param  No parameter needed.
 *  \return pointer pointing to the last position in the current line code
 *  \details Use this function whenever you need to know the end of the current
 * command-line instruction
 */
instr_linkedList *RetrieveEndOfInstruction(void);

/** TOKEN Label. This function do nothing*/
int token_label(void);
/** add two strings and return the pointer to the new string.*/
char *concat(int count, ...);

/**
 * Initialize the default file names for the system.
 * */
void InitializeDefaultScriptFileName();

/** Calculate hash value*/
/**
 * This will calculate the has of a string.
 * Thinking is to provide hash name for variables
 * to make it faster when searching for them.
 * */
// TODO Make sure that this function make Unique ID.2020-04-15
inline uint32_t CalculateHashOfString(const char* pText, unsigned int length)
{
    if (length > MAX_HASH_STRING_LENGTH) {
        StopExecution = 1; // MAX number of bytes is 16 bytes.
        return 0;
    }

    if (pText == NULL) {
        return 0; // ERROR
    }

    uint32_t h = 2166136261UL; // FNV-1a offset basis

    for (unsigned int i = 0; i < length; i++) {

        if (pText[i] == '\0') {
            return 0; // ERROR
        }

        h ^= (uint8_t)pText[i];
        h *= 16777619UL; // FNV-1a prime
    }

    return h;
}

/**
 * This version decide how long the string should be taken.
 * */

// Return 1 if the execution of the program is reached the end otherwise 0
inline int end_of_program_token(void) {
  // check ONLY p_prog ... p_prog_next could reach the EOF before p_prog.
  if (p_prog == NULL) {
    return 1;
  }
  return ((p_prog->txtString == NULL) || (current_token_word == 1));
}

/**
 * convert digital text to real decimal numbers.
 * Find out and convert the numbers to a normal decimal number from text
 * */
inline float convert_text_to_number(void) {
  // Stop running if we have error
  float result;

  /*Possible combination of text and numbers in a string (it should be at least
   3 char and max 10) 0x00       --->      0xFF 0B00000000 --->      0B11111111
   0O00 --->            0O56
   000        --->      255
   any thing else will generate an error!
   */
  switch (*(p_prog->txtString + 1)) {
  case 'x': {
    // Hex number
    result = (float)strtol(p_prog->txtString, NULL,
                           16); // Convert a hex number to long
    break;
  }
  case 'b': {
    // Binary number
    result = (float)strtol((p_prog->txtString + 2), NULL,
                           2); // Convert a binary number to long.. strtol
                               // cannot take 0b .. remove it from the buffer
    break;
  }
  case 'o': {
    // octal number
    result = (float)strtol((p_prog->txtString + 2), NULL,
                           8); // Convert a octal number to long.. strtol cannot
                               // take 0o .. remove it from the buffer
    break;
  }

  default: {
    result =
        extractFloat(p_prog->txtString); // Convert the first digits found in
                                         // the string to decimal numbers.
    break;
  }
  }
  return result;
}

inline int check_and_get_next_token(void) {
  /* New way to do this job is like this:
   1-get the new position from p_prog_next
   2-give the next position to p_prog_next
   3-Check if it is TOKEN_NEW_LINE ..
   4-Check if the new token is enum of the TOKEN_WORD
   5-If so - take the number and save it in current_token_word
   6-give the get the new position from p_prog_next and put it in p_prog
   7-give the next position to p_prog_next
   */

  p_prog = p_prog_next; // If it is not TOKEN_NEW_LINE, this will bring only the
                        // enum part of the token.
  if (end_of_program_token() == 1) {
    current_token_word = 1;    // TOKEN_EOF=1
    return current_token_word; // We return 0 whenever we are done EOF
  }
  // New .dad file shouldn't have any space, neither new line at the end of the
  // instructions or arguments.
  if (*p_prog->txtString == '\n') {
    // \n causes to have two '\n\n .. since the text is equal to '\n' Skip it so
    // we don't have a problem later.
    p_prog = p_prog->next;
  }
  p_prog_next = p_prog->next;
  // Now we have to check if the token is digits (numbers) : Dec, Hex, bin are
  // supported.
  if (isdigit(*p_prog->txtString)) {
    // Convert the token-text to int
    current_token_word = (int)convert_text_to_number();

    p_prog = p_prog_next;
    p_prog_next =
        p_prog->next; // Since we have always 2 records per each token ..
    // we need to move the next cursor to the new position one more time.
    return current_token_word;
  } else {
    // We shouldn't be here .. Each time when a token saves in the dad file ..
    // first is the token_enum then it is the real text
    // DEBUG_MESSAGE("ERROR %i\n",DBG_MSG_DAD_FILE_CORRUPTED);
    return quitScriptProgram(1);
  }
  return 0;
}

/**
 * Allocate a new variable
 * @param name Variable name or NULL
 * @param tag  Type of variable
 * @param arraySize Array Size
 * @param Position Index position of the variable
 * return -1 if error, >=0 if OK
 * */
int addNewScriptVariable(char *name, int tag, int arraySize, int Position);

/**
 * Allocate a new variable
 * @param name Variable name or NULL
 * @param Position Index position of the variable
 * return -1 if error, >=0 if OK
 * */
int addNewScriptVariable(char *name, int Position);

#define END_OF_LINE                                                            \
  (current_token_word == TOKEN_NEW_LINE ||                                     \
   current_token_word == TOKEN_SEMICOLON)

/** Check and accept/process the token
 *@param token_word to be accepted
 */
void process_token(int token_word);
#endif
