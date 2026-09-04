/**
 ******************************************************************************
 * @file    error.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains the debug print function. It send debug messages to the terminal.
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

#include "types.h"
#include "ringbuffer.h"

//#define OS_MAINSTKSIZE          10*1024 //(10K)

/** This variable is used to keep the status of debug message enable
*   if DEBUG_MESSAGE_ENABLE=0   No message will be send to the debug-terminal
*      DEBUG_MESSAGE_ENABLE=0   Debug Message will be delivered to debug-terminal
*      Note: Code must be compiled with CXX_FLAGS += -D_DEBUG=1
*/
unsigned char DEBUG_MESSAGE_ENABLE = 0;       //by Default is disabled.
/**
 * Debug thread pointer
 * */
Thread* debugThread = 0;

/**
 * This function will enable the thread
 * It will create a new thread if the thread dosen't exists.
 * @param return -1 if error otherwise +1
 * */
int  enable_disableDebugThread(void) {
    /** Thread that take care of debug messages to terminal*/
    if (DEBUG_MESSAGE_ENABLE == 1) {
        if (debugThread == NULL) {
            debugThread = new Thread(osPriorityNormal7, 8 * 1024 /*32K stack size*/);
        }
        if (debugThread == NULL) {
            SERIAL_MESSAGE("FATAL ERROR: Couldn't start debug thread\n");
            quitScriptProgram(1);
            return -1;
        }
        debugThread->start(debugThreadFunction);
        return 1;
    }
    else
    {
        if (debugThread != NULL) {
            debugThread->terminate();
            if (debugThread!=NULL)
            	delete debugThread;
            debugThread = NULL;
        }
        else {
            //Nothing to do .. already no thread there.
        }
        return 1;
    }
}

/**
 * SpecialSerial constructor
 * @param tx  TX UART GPIO
 * @param rx  RX UART GPIO
 * @param baud
 * */
specialSerial::specialSerial(PinName tx, PinName rx/*, const char *name,*/, int baud) : UnbufferedSerial(tx, rx/*,name*/, baud),
RX(rx),
TX(tx)
{
}
/**
 * Get the pinName of the pinname used for TX
 * @return pinName of TX
 * */
PinName specialSerial::retrieveTX(void)
{
    return TX;
}
/**
 * Get the pinName of the pinname used for RX
 * @return pinName of RX
 * */
PinName specialSerial::retrieveRX(void)
{
    return RX;
}

/** Sub thread to take care of debug serial communications
 * This thread will print debug info to the debug terminal.
*/
void debugThreadFunction(void)
{
    const char* msg = "Debug Thread started\n";
    globalSerialCOMvar.Debug_pc->write(msg, sizeof(msg));
    char currentByte[1];
    while (1) {
        if (!(DebugSerialFIFO.checkEmpty())) {
            /* We have data inside the buffer. Write to serial terminal.
            */

            while (!(globalSerialCOMvar.Debug_pc->writeable()))
                rtos::ThisThread::sleep_for(chrono::milliseconds(1));;    //Wait 1  msec
            currentByte[0] = DebugSerialFIFO.get();
            globalSerialCOMvar.Debug_pc->write(currentByte, 1);
        }
        rtos::ThisThread::sleep_for(chrono::milliseconds(1));;    //Wait 1  msec
    }
}

/**
*   Actual debug print function. This function is a replacement of printf
*   it uses ringbuffer to print out debug info to terminal (It doesn't do
*   the actual sending. It puts the messages in to the ringbuffer
*   Later the thread will take care of sending the data to the terminal.
*   @param format   formating the string
*   @param ...  other parameter sens to the function.
*/
int DEBUG_Print(const char* format, ...)
{
    int len = 0;
    char lineBuf[20];

    char* pstrBuf = 0;

    if (DEBUG_MESSAGE_ENABLE == 1) {
        memset(lineBuf, '\0', 20);
        std::va_list arg;
        va_start(arg, format);
        // ARMCC microlib does not properly handle a size of 0.
        // As a workaround supply a dummy buffer with a size of 1.
        char dummy_buf[1];
        len = vsnprintf(dummy_buf, sizeof(dummy_buf), format, arg);
        pstrBuf = new char[len + 1];
        if (pstrBuf == NULL) {
            DEBUG_MESSAGE("ERROR: Failed to allocate mem pstrBuf\n");
            quitScriptProgram(1);
        }
        vsprintf(pstrBuf, format, arg);
        if (pScriptIndex != NULL && p_prog != NULL) {
            listObj* currentLineInfo = pScriptIndex->findPointer(p_prog);
            if (currentLineInfo != NULL) {
                snprintf(lineBuf, 20, "#%u:", currentLineInfo->lineNo);
            }
            else
                snprintf(lineBuf, 20, "%s", "N.A:");
        }
        char* scanBuffr = lineBuf;
        if (scanBuffr != NULL) {
            while (*scanBuffr != '\0') {
                DebugSerialFIFO.put(*scanBuffr);
                scanBuffr++;
            }
            scanBuffr = pstrBuf;
            while (*scanBuffr != '\0') {
                DebugSerialFIFO.put(*scanBuffr);
                scanBuffr++;
            }
            // printf(pstrBuf); We avoid blocking the execution.  A thread will take care of this part.
            va_end(arg);
        }
    }
    if(pstrBuf!=NULL)
    	delete[] pstrBuf;
    pstrBuf = NULL;
    return len;
}

/* This function will remove the extra '\' from the following commands that found in the script program sent to the system.
    Without that, they will be shown as they are and the commands they represent will not be executed by printf.
*        \'  single quote
*        \"  double quote
*        \?  question mark
*        \\  backslash
*        \a  audible bell
*        \b  backspace
*        \f  form feed -
*        \n  line feed -
*        \r  carriage return
*        \t  horizontal tab
*        \v  vertical tab
*@param  InputString : message should be fixed
*/
char* correctString(const char* InputString)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_NULL

        unsigned int i = 0;
    unsigned int length = strlen(InputString);
    char* result = (char*)calloc(length + 1, sizeof(char));
    unsigned int offset = 0;
    while ((i + offset) <= length) {
        if ((offset + i + 1) <= length) {
            if ((InputString[i] == '\\') && (InputString[i + 1] == 't')) {
                result[i] = '\t';
                offset++;
            }
            else if ((InputString[i] == '\\') && (InputString[i + 1] == 'n')) {
                result[i] = '\n';
                offset++;
            }
            else if ((InputString[i] == '\\') && (InputString[i + 1] == 'r')) {
                result[i] = '\r';
                offset++;
            }
            else if ((InputString[i] == '\\') && (InputString[i + 1] == 'v')) {
                result[i] = '\v';
                offset++;
            }
            else
            {
                result[i] = InputString[offset + i];
            }
        }
        i++;
    }
    return result;
}
