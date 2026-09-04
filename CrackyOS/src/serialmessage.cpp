/**
 ******************************************************************************
 * @file    serialmessage.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief This file contains Serial communications instructions and definitions.
 *******************************************************************************
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

#include "serialmessage.h"

FILE *testProgw = NULL;
/**
 * Change text color or background of the terminal
 * @param _color background or foreground color
 * FONT_BLACK_CODE
 *  FONT_RED_CODE)
 *  FONT_GREEN_CODE
 *  FONT_YELLOW_CODE
 *  FONT_BLUE_CODE
 *  FONT_MAGENTA_CODE
 *  FONT_CYNA_CODE)
 *  FONT_WHITE_CODE
 *  FONT_DEFAULT_CODE
 *
 *  BKG_BLACK_CODE
 *  BKG_RED_CODE
 *  BKG_GREEN_CODE
 *  BKG_YELLOW_CODE
 *  BKG_BLUE_CODE
 *  BKG_MAGENTA_CODE
 *  BKG_CYNA_CODE
 *  BKG_WHITE_CODE
 *  BKG_DEFAULT_CODE
 * */
void ChangeTerminalColor(int _color) {
    SERIAL_MESSAGE("%c", char(27));
    switch (_color) {
    case 0: SERIAL_MESSAGE(FONT_BLACK_CODE); break;
    case 1: SERIAL_MESSAGE(FONT_RED_CODE); break;
    case 2: SERIAL_MESSAGE(FONT_GREEN_CODE); break;
    case 3: SERIAL_MESSAGE(FONT_YELLOW_CODE); break;
    case 4: SERIAL_MESSAGE(FONT_BLUE_CODE); break;
    case 5: SERIAL_MESSAGE(FONT_MAGENTA_CODE); break;
    case 6: SERIAL_MESSAGE(FONT_CYNA_CODE); break;
    case 7: SERIAL_MESSAGE(FONT_WHITE_CODE); break;
    case 10:SERIAL_MESSAGE(FONT_DEFAULT_CODE); break;

    case 20: SERIAL_MESSAGE(BKG_BLACK_CODE); break;
    case 21: SERIAL_MESSAGE(BKG_RED_CODE); break;
    case 22: SERIAL_MESSAGE(BKG_GREEN_CODE); break;
    case 23: SERIAL_MESSAGE(BKG_YELLOW_CODE); break;
    case 24: SERIAL_MESSAGE(BKG_BLUE_CODE); break;
    case 25: SERIAL_MESSAGE(BKG_MAGENTA_CODE); break;
    case 26: SERIAL_MESSAGE(BKG_CYNA_CODE); break;
    case 27: SERIAL_MESSAGE(BKG_WHITE_CODE); break;
    case 30:SERIAL_MESSAGE(BKG_DEFAULT_CODE); break;
    }
}

/**
 * Serial printf function with formating arguments.
 * @param format  and other parameter..etc
 * @return -1 if failed or >=0 if OK.
 * */
int SERIAL_Print(const char *format, ...) {
    int len = 0;
    char *pstrBuf = NULL;
    char *scanBuffr = NULL;
    std::va_list arg;
    va_start(arg, format);
    std::va_list length_arg;
    va_copy(length_arg, arg);
    // ARMCC microlib does not properly handle a size of 0.
    // As a workaround supply a dummy buffer with a size of 1.
    char dummy_buf[1];
    len = vsnprintf(dummy_buf, sizeof(dummy_buf), format, length_arg);
    va_end(length_arg);
    pstrBuf = new char[len + 1];
    if (pstrBuf == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
        return quitScriptProgram(1);
    }
    vsnprintf(pstrBuf, len + 1, format, arg);
    scanBuffr = pstrBuf;
    while (*scanBuffr != '\0') {
        globalSerialCOMvar.Serial_pc->write(scanBuffr, 1);
        scanBuffr++;
    }
    // printf(pstrBuf); We avoid blocking the execution.  A thread will take care of this part.
    va_end(arg);
    delete[] pstrBuf;
    pstrBuf = NULL;
    return len;
}

//
/**
 * Get a line of instructions or string form terminal.
 * send a buffer buf[INTER_ACTIVE_MODE_MAX_LENGTH_OF_THE_STRING] to this function
 *
 * */
void GetStringFromSerial(char *stringBuf ) {
    int count = 0;
    char tempbuf[2];
    memset(stringBuf, '\0', INTER_ACTIVE_MODE_MAX_LENGTH_OF_THE_STRING);
    while (count < INTER_ACTIVE_MODE_MAX_LENGTH_OF_THE_STRING - 1) {
        if (globalSerialCOMvar.Serial_pc->readable()) {
            memset(tempbuf, 0, 2);
            globalSerialCOMvar.Serial_pc->read(tempbuf, 1);
            stringBuf[count] = tempbuf[0];
            if (stringBuf[count] == '\n') {
                return;
            }
            if (stringBuf[count] == '\b') {
                if (count >= 1) {
                    globalSerialCOMvar.Serial_pc->write("\b \b", 3);
                    stringBuf[count] = '\0';
                    count--;
                }
                stringBuf[count] = '\0';
            } else {
                memset(tempbuf, 0, 2);
                tempbuf[0] = stringBuf[count];
                globalSerialCOMvar.Serial_pc->write(tempbuf, 1);
                count++;
            }
        } else
            rtos::ThisThread::sleep_for(chrono::milliseconds(1));
        ;    //Wait 1  msec
    }
}

/**
 * Clean up the screen and go back to home
 * serial com port
 * */
void clearAndHomeSerial() {
    //Serial Terminal
    if (globalSerialCOMvar.Serial_pc != NULL) {
        static const char clearAndHomeSequence[] = "\033[2J\033[H";
        globalSerialCOMvar.Serial_pc->write(clearAndHomeSequence, sizeof(clearAndHomeSequence) - 1);
    }
}

/**
 * Clean up the screen and go back to home
 * debug com port
 * */
void clearAndHomeDEBUG() {
    //Debug Terminal
    if (globalSerialCOMvar.Debug_pc != NULL) {
        static const char clearAndHomeSequence[] = "\033[2J\033[H";
        globalSerialCOMvar.Debug_pc->write(clearAndHomeSequence, sizeof(clearAndHomeSequence) - 1);
    }
}

/**
 * Clear and go home for both terminals.
 * */
void clearAndHome() {
    //Serial Terminal
    clearAndHomeSerial();
    clearAndHomeDEBUG();
}
/**
 * Get a string from the terminal
 * @return the string got from the user.
 * */
char* GET_SERIAL_TEXT(void) {
    char *pstrTextMessage1 = NULL;
    char *pstrTextMessage2 = NULL;

    int counter = 0;
    int memPage = 1;
    pstrTextMessage2 = pstrTextMessage1 = (char*) calloc(
            MAX_ALLOWED_MESSAGELENGTH, sizeof(char));
    if (pstrTextMessage1 == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SERIAL_BUFF);
        quitScriptProgram(1);
    }
    while (1) {
        if (globalSerialCOMvar.Serial_pc->readable()) {
            char bufft[2];
            memset(bufft, 0, 2);
            globalSerialCOMvar.Serial_pc->read(bufft, 1);
            pstrTextMessage1[counter] = bufft[0];
            //  checksum+=pstrTextMessage1[counter];    //Calculate checksum
            if ((pstrTextMessage1[counter] != '#')) {
                counter++;
                if (counter + 1 >= MAX_ALLOWED_MESSAGELENGTH * memPage) {
                    memPage++;
                    char *newReallocatedPointer = (char*) realloc(
                            pstrTextMessage1,
                            MAX_ALLOWED_MESSAGELENGTH * memPage);
                    if (newReallocatedPointer == NULL) {
                        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GET_SERIAL);
                        quitScriptProgram(1);
                    }
                    pstrTextMessage1 = newReallocatedPointer;
                }
            } else {
                // We have the text now.
                pstrTextMessage1[counter] = '\0';   // remove # from the string.
                return pstrTextMessage2;
            }
            if (counter + 1 < MAX_ALLOWED_MESSAGELENGTH * memPage) {
                //This will replace memset as we cannot memset reallocated memory.
                pstrTextMessage1[counter + 1] = '\0'; //memset for reallocated places.
            }
        }
    }
}

/**
 * This function will take care of getting a script program from terminal and save it to the SD-Disk<
 * */
void getScriptProgramFile() {
    int counter = 0;
    unsigned char sbuff;
    SERIAL_MESSAGE("Send the file please and quit with '#' symbol\n");

    select_spi_channel (CHANNEL0);
    testProgw = initialize_progFile(1);     // Open file in WRITE MODE.
    if (testProgw == NULL) { //If we didn't open the file for 'write' quit and you could run interactive mode again.
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_CANNOT_CREATE_FILE);
        quitScriptProgram(1);
    }
    while (1) {
        if (globalSerialCOMvar.Serial_pc->readable()) {
            char buff[2];
            memset(buff, 0, 2);

            globalSerialCOMvar.Serial_pc->read(buff, 1);

            //   checksum+=sbuff;    //Calculate checksum
            if (((char) buff[0]) != '#') {
                globalSerialCOMvar.Serial_pc->write(buff, 1);
                sbuff = buff[0];
                fputc(sbuff, testProgw);
                counter++;
            } else {
                fclose(testProgw);
                rtos::ThisThread::sleep_for(chrono::seconds(1)); //Wait 1000  msec
                quitScriptProgram(1);
                return;
            }
        }
    }
}

unsigned int checksum = 0;

/**
 * The function will print a help message to the terminal.
 */
void print_help(void) {
    clearAndHome();

    SERIAL_MESSAGE(
            "\
    \033[5m\033[7m\033[1m\t\tCrackyOS Scripting OS Version 1.0:\033[0m \t\n\
    1) dir: List the directories available on the SD disk\n\
    2) cd dirName: Change current directory \n\
    3) copy oldfilename newfilename: Copy a file \n\
    4) del fName: delete the file from SD\n\
    5) mkdir name: Make a directory\n\
    6) move: oldfilename newfilename\n\
    7) format: Erase SD Disk\n\
    8) run: filename: Run a script program\n\
    9) rename: oldfilename newfilename: Rename a file name\n\
   10) clear: Clear terminal screen\n\
   11) cls: Clear terminal screen\n\
   12) mount: mount the SD card\n\
   13) umount/unmount: Unmount the SD Card\n\
   14) reset: Reset the system\n\
   15) help: print help\n\
   16) InstructionSet: list down the instruction sets\n\
   17) cat: Show content of a file or create a file\n\
   18) send_script: Send program script file via serial\n\
   19) or Instruction sets separated by semicolon .. Please read the documentation\n\
   ");
}

/**
 * Change the baud rate instruction
 * Syntax
 *  baud_rate xxxxxx
 * Note: After reset, baud rate will return to the default.
 * @return -1 if failed, >=0 if OK
 */
int token_baud_rate() {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_BAUD_RATE_COMMAND_EXECUTED);
    process_token (TOKEN_BAUD_RATE);
    if (current_token_word != TOKEN_NUMBER) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_BAUDRATE_NUMBER_NOT_FOUND);
        quitScriptProgram(1);
    }
    baud_rate = convert_text_to_number();
    globalSerialCOMvar.Serial_pc->baud(baud_rate);
    process_token (TOKEN_NUMBER);
    return 1;
}
/**
 *Enable/Disable Debug instructions.
 * Syntax : enable_debug
 *          disable_debug
 * @return -1 if failed, >=0 if OK
 */
int token_enable_debug() {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

    if (current_token_word == TOKEN_ENABLE_DEBUG)
        DEBUG_MESSAGE_ENABLE = 1;
    else
        DEBUG_MESSAGE_ENABLE = 0;

    process_token(current_token_word);
    return enable_disableDebugThread();
}

/**
 *Syntax of the instruction is like follow:
 *   print "TEXT TO BE SENT"
 *   print VARIABLE
 *    You should use '"' for start/end of the string
 * @return -1 if failed, >=0 if OK
 */

int token_print(void) {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

    //TODO: Make this function to be able to take even the format of normal printf ..ie ("%i, text , ..etc",var1, var2..etc) 2018-04-01
    char *pserBuff = NULL;
    ;
    int strLength = 0;

    process_token (TOKEN_PRINT);
    switch (current_token_word) {
    case TOKEN_STRING: {
        strLength = strlen(p_prog->txtString) - 1;
        pserBuff = (char*) calloc(strLength + 1, sizeof(char));
        if (pserBuff == NULL) {
            free(pserBuff);
            return quitScriptProgram(1);
        }

        strncpy(pserBuff, (p_prog->txtString + 1), strLength - 1); //We don't need to add '\0' as we uses calloc

        FixStringIssue(pserBuff);
        // It is only a string ...Just send it to the serial terminal
        SERIAL_MESSAGE(pserBuff);
        process_token (TOKEN_STRING);
        free(pserBuff);
        pserBuff = NULL;
        return 0;
    }
    case TOKEN_VARIABLE: {
        strLength = strlen(p_prog->txtString);
        pserBuff = (char*) calloc(strLength + 1, sizeof(char));
        if (pserBuff == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
            return quitScriptProgram(1);
        }

        strncpy(pserBuff, p_prog->txtString, strLength); //We don't need to add '\0' as we uses calloc

        // Here we have a variable
        //the variable could be either numerical or string. We have to separate that.

        int varPosition = TestSystem_get_variable(pserBuff);
        if (varPosition == -1) {
            //It is an error since the variable was used with another instruction
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
            return quitScriptProgram(1);
        }

        process_token (TOKEN_VARIABLE);
        if (pserBuff != NULL)
            free(pserBuff);
        pserBuff = NULL;
        //Find out if the variable is an array and we have only a value of the array to be printed
        if (current_token_word == TOKEN_LEFT_SQUARE_BRACKET) {
            //We have a value of an array that must be printed ..i.e a[No.]
            process_token (TOKEN_LEFT_SQUARE_BRACKET);
            if (current_token_word != TOKEN_NUMBER) {
                if (current_token_word==TOKEN_VARIABLE){
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_PRINT_SYNTAX_NOT_SUPPORTED);
                    return quitScriptProgram(1);
                }
                else {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_PRINT0);
                return quitScriptProgram(1);
                }

            }
            int value = (int) convert_text_to_number();
            process_token (TOKEN_NUMBER);
            if (current_token_word != TOKEN_NUMBER) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_PRINT1);
                return quitScriptProgram(1);

            }
            process_token(TOKEN_NUMBER);
            if (current_token_word != TOKEN_RIGHT_SQUARE_BRACKET) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_PRINT2);
                return quitScriptProgram(1);
            }
            process_token (TOKEN_RIGHT_SQUARE_BRACKET);
            switch (script_variables[varPosition].tag) {
            case T_STRING:
                SERIAL_MESSAGE("%c",
                        script_variables[varPosition].Value.txtvalue[value]);
                return 0;
            case T_FLOAT:
                SERIAL_MESSAGE("%f",script_variables[varPosition].Value.pf[value]);
                return 0;
            case T_INT:
                SERIAL_MESSAGE ("%i",script_variables[varPosition].Value.pi[value]);
                return 0;
            default: {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_WRONG_VAR);
            }
            }
        } else {

            switch (script_variables[varPosition].tag) {
            case T_STRING: {
                char *testString = script_variables[varPosition].Value.txtvalue;
                if (testString != NULL) {
                    //We need to check if the array has got ASCII code or Hex numbers.
                    FixStringIssue(testString);
                    SERIAL_MESSAGE(testString);
                }
                break;
            }

            case T_FLOAT: {
                int sizeOfFloatArray = script_variables[varPosition].sizeOfArray;
                if (sizeOfFloatArray > 1) {
                    for (int i = 0; i < sizeOfFloatArray; i++) {
                        std::string st;
                        st = std::to_string(
                                script_variables[varPosition].Value.pf[i]);
                        SERIAL_MESSAGE(st.c_str());
                    }
                } else {
                    std::string st;
                    st = std::to_string(
                            *script_variables[varPosition].Value.pf);
                    SERIAL_MESSAGE(st.c_str());
                }
                break;
            }

            case T_INT: {
                int sizeOfFloatArray = script_variables[varPosition].sizeOfArray;
                if (sizeOfFloatArray > 1) {
                    for (int i = 0; i < sizeOfFloatArray; i++) {
                        std::string st;
                        st = std::to_string(
                                script_variables[varPosition].Value.pi[i]);
                        SERIAL_MESSAGE(st.c_str());
                    }
                } else {
                    std::string st;
                    st = std::to_string(
                            *script_variables[varPosition].Value.pi);
                    SERIAL_MESSAGE(st.c_str());
                }
                break;
            }
            default: {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_WRONG_VAR);
            }
            }
        }
        //SERIAL_MESSAGE("\n");  We can not have this as user must decide if they want to do so.
        return 0;
    }
    default: {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_FATAL_WE_SHOULDNT_BE_HERE1);
        return quitScriptProgram(1);
    }
    }
    return 0;
}

/** Get message from terminal instruction
 * Syntax of the instruction is like follow:
 *  serial_get variable
 * Note: You should provide always a variable to this function
 * @return -1 if failed, >=0 if OK
 */

int token_serial_get(void) {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

    process_token (TOKEN_SERIAL_GET);
    int varPosition;
    if (current_token_word != TOKEN_VARIABLE) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND_SERIAL_GET);
        return quitScriptProgram(1);
    }
    int stringLength = (strlen(p_prog->txtString));
    char *Serialvar_buf = (char*) calloc(stringLength, sizeof(char));
    if (Serialvar_buf == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_VAR);
        return quitScriptProgram(1);
    }
    strncpy(Serialvar_buf, p_prog->txtString, stringLength); //We don't need to add '\0' as we uses calloc

    process_token (TOKEN_VARIABLE);           // Get next token word.

    //Be aware, the function GET_SERIAL_TEXT return an allocated mem. Memory should be cleared if the variable dies.
    //But at the moment we don't care since the global variable has only a pointer and will keep this memory location.
    char *getStr = GET_SERIAL_TEXT();

    varPosition = TestSystem_get_variable(Serialvar_buf);
    if (varPosition == -1) {
        //create the variable
        varPosition = TestSystem_get_empty_variable_pos();
        if (varPosition < 0) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_SCRIPT_VARIABLE_OVERFLOW);
            return quitScriptProgram(1);
        }
        addNewScriptVariable(Serialvar_buf, T_STRING, strlen(getStr) + 1,
                varPosition);
        script_variables[varPosition].Value.txtvalue = getStr;
        Serialvar_buf = NULL;
        getStr = NULL;
    } else {
        if (script_variables[varPosition].tag == T_STRING) {
            delete[] script_variables[varPosition].Value.txtvalue;
            script_variables[varPosition].Value.txtvalue = NULL;
            script_variables[varPosition].Value.txtvalue = getStr;
            script_variables[varPosition].sizeOfArray = strlen(getStr) + 1;
        } else {
            //Variable has changed it's type
            if (script_variables[varPosition].tag == T_INT) {
                delete (script_variables[varPosition].Value.pi);
                script_variables[varPosition].Value.pi = NULL;
            } else if (script_variables[varPosition].tag == T_FLOAT) {
                delete (script_variables[varPosition].Value.pf);
                script_variables[varPosition].Value.pf = NULL;
            }
        }
        script_variables[varPosition].tag = T_STRING;
        script_variables[varPosition].Value.txtvalue = getStr;
        script_variables[varPosition].sizeOfArray = strlen(getStr) + 1;
    }
    return 0;
}

/** Thread that take care of serial interrupt to show the Menu on terminal
 * YOU MUST KEEP IT SHORT .. NO SD-Card activity allowed here. */
void ChangeInterActiveModeStatus() {
    char buff[2];
    memset(buff, 0, 2);
    globalSerialCOMvar.Serial_pc->read(buff, 1);
    if (buff[0] == 03) {
        StopExecution = 1;
        BreakPressed = 1;
    }
}
