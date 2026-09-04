/**
 ******************************************************************************
 * @file    ticker.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains ticker instruction.
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
 *
 * TICKER Instruction. It creates a ticker object with specific definition
 * Syntax: ticker PinName mypin , TimeInterval inUsec, Start/STOP =1/0
 *
 **/

#include "ticker_hw.h"
#include "types.h"

  /**
   * Free memory from Ticker objects
   * @param enumNumber : Which object. If enumNumber==-1 remove all objects.
   *  TODO: Check this code !!!!  2020-04-22
   * */
void FreeUp_Tickers(int enumNumber) {
    tobj* pScanTicker = GTickerObject;
    while (pScanTicker != NULL) {
        tobj* next = pScanTicker->next;
        if (enumNumber == -1 || (pScanTicker->mToggelPin != NULL
            && pScanTicker->mToggelPin->enumPINNAME() == enumNumber)) {
            if (pScanTicker->mToggelPin != NULL) {
                pScanTicker->mToggelPin->__detach();
                delete pScanTicker->mToggelPin;
            }
            if (pScanTicker->prev != NULL)
                pScanTicker->prev->next = pScanTicker->next;
            else
                GTickerObject = pScanTicker->next;
            if (pScanTicker->next != NULL)
                pScanTicker->next->prev = pScanTicker->prev;
            delete pScanTicker;
            if (enumNumber != -1)
                return;
        }
        pScanTicker = next;
    }
    if (enumNumber == -1)
        GTickerObject = NULL;
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_TICKER_OBJECT_DELETION_NOT_FOUND);
    }
}

Flipper::Flipper(int m_en) : m_pin(NULL), m_enumPINNAME(m_en) {
    mInterval = 0.0;
}
Flipper::~Flipper() {
    freeOneGPIOVariable(&m_pin);
}
void Flipper::Toggle(void) {
    if (m_pin != NULL) {
        if (m_pin->scripPin.gpio.OnePinGPIO->genInOut.InOutPin != NULL)
            m_pin->scripPin.gpio.OnePinGPIO->genInOut.InOutPin->write(!(m_pin->scripPin.gpio.OnePinGPIO->genInOut.InOutPin->read()));
    }
}
int Flipper::enumPINNAME(void) {
    return m_enumPINNAME;
}

void Flipper::__attach() {
    mticker.attach(callback(this, &Flipper::Toggle), chrono::microseconds(mInterval));
}
/**
 * This will not stop the timer. It stops only the interrupt.
 * */
void Flipper::__detach() {
    mticker.detach();
}

void Flipper::setInterval(uint32_t g) {
    mInterval = g;
}

/**
 * Global ticker listedlink pointer.
 * */
tobj* GTickerObject = NULL;

/**
 * Find an object in the linked list.
 *
 */
tobj* TickerObject_find(int enumPinName) {
    STOP_CODE_EXECUTION_NULL
        tobj* scanObj;
    scanObj = GTickerObject;

    if (scanObj == NULL) return NULL; //Nothing there.

    if (scanObj->next == NULL && scanObj->prev == NULL) {
        //We have only one value
        if (enumPinName == scanObj->mToggelPin->enumPINNAME())
            return scanObj;
        else
            return NULL;
    }
    while (scanObj->next != NULL) {
        scanObj = scanObj->next;
    }

    while (scanObj->prev != NULL)
    {
        if (enumPinName == scanObj->mToggelPin->enumPINNAME())
            return scanObj;
        scanObj = scanObj->prev;
    }
    if (enumPinName == scanObj->mToggelPin->enumPINNAME())
        return scanObj;
    else
        return NULL;//not found.
}

//Ticker object impl.
/**
 * TickerObject add new object, create the GPIO DigitalOut object
 * */

void TickerObject_add(int enumPinName, uint32_t uSecTime, bool DefaultValue) {
    tobj* myObj, * oldObj;
    myObj = GTickerObject;

    oldObj = TickerObject_find(enumPinName);
    if (oldObj == NULL) {
        if (myObj == NULL) {
            //No object defined .. create the first
            myObj = new tobj;
            if (myObj == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_TICKER);
                quitScriptProgram(1);
                return;
            }
            //Reset the pointers.
            myObj->next = myObj->prev = NULL;
            GTickerObject = myObj;
        }
        else
        {
            while (myObj->next != NULL) myObj = myObj->next;
            myObj->next = new tobj;
            if (myObj->next == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_TICKER);
                quitScriptProgram(1);
                return;
            }
            myObj->next->prev = myObj;
            myObj->next->next = NULL;
            myObj = myObj->next;
        }
        //Create new digital out pin.
        myObj->mToggelPin = new Flipper(enumPinName);
        if (myObj->mToggelPin == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_TICKER_DIGITALOUT);
            quitScriptProgram(1);
            return;
        }

        myObj->mToggelPin->m_pin = addGPIOpin_DIGITAL(myObj->mToggelPin->enumPINNAME(), GPIO_TICKER);
        if (myObj->mToggelPin->m_pin == NULL) {
            //We have a bad instruction.
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_TICKER);
            quitScriptProgram(1);
            return;
        }
    }
    else {
        myObj = oldObj; //We found the old object.
    }
    myObj->mToggelPin->setInterval(uSecTime);
    myObj->mToggelPin->__detach();// We do this always as we might need to update the interval
    if (DefaultValue) {
        myObj->mToggelPin->__attach();
    }
}

/**
 * TICKER Instruction. It creates a ticker object with specific definition
 * Syntax: ticker PinName mypin , TimeInterval inUsec, Start/STOP =1/0
 * */

int token_ticker() {
    float interval = 0.0;
    bool start = 1;
    process_token(TOKEN_TICKER);
    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);
    //1) We should take the pin name here.

    int tempenumPin = current_token_word;
    PinName tempPin = retrievePinNameFromTOKEN_NAME(current_token_word);
    if (tempPin == NC) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_TICKER_PINNAME);
        return quitScriptProgram(1);
    }

    process_token(current_token_word);
    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);

    //2) Interval
    if (current_token_word == TOKEN_NUMBER) {
        interval = convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }
    else if (current_token_word == TOKEN_VARIABLE) {
        int varPosition = TestSystem_get_variable(NULL); // we don't need to send the name .. the function will do the job.
        process_token(TOKEN_VARIABLE);
        if (varPosition < 0) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_TICKER);
            quitScriptProgram(1); //Exit the program.
        }
        if (script_variables[varPosition].tag != T_INT) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_TICKER);
            quitScriptProgram(1);
        }
        interval = *script_variables[varPosition].Value.pi;
    }
    else {
        //Syntax error. .only number is allowed
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_TICKER_INTERVAL);
        return quitScriptProgram(1);
    }

    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);
    //3)Start/Stop
        //This is optional. Default is the ticker will start immediately
    if (current_token_word == TOKEN_NEW_LINE ||
        current_token_word == TOKEN_SEMICOLON ||
        current_token_word == TOKEN_EOF) {
        start = 1;  //We are done.
    }
    else
    {
        //4) Start or Stop

        if (current_token_word != TOKEN_NUMBER) {
            //Syntax error
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_TICKER_START_STOP);
            return quitScriptProgram(1);
        }
        start = convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }
    //We have everything. Create the object and register the GPIO.
    TickerObject_add(tempenumPin, interval, start);
    return 0;
}
