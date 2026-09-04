/**
 *******************************************************************************
 * @file    interruptin.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief  This file contains implementation for interruptIn instruction
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

#include "interrupt_in.h"
/**
  * syntax :
  * Interrupting pinname, function_name
  * Warning: function_name MUST be defined or this code will fails.
  *
  * */

/**Global function that will put the function
   * that must be run if interrupt occur
   *  WARNING: YOU MUST REMOVE THE FUNCTION NAME FROM GlobActiveInterruptFunc WHEN YOU RUN THE FUNCTION.
   * */

void GeneralInterruptInCallback(int* enumNumber) {
    int enumvalue = *enumNumber;
    GlobalGP* interruptPIN = findGPIOpin(enumvalue);
    if (interruptPIN == NULL) {
        //We cannot call quitScriptProgram as it uses serial message. just stop execution.
        InterActiveMode = 1;
        StopExecution = 1;
        return;
    }
    varGlobActiveInterruptFunc = interruptPIN->scripPin.fun;
}
/**
 * InterruptIn implementation.
 * Syntax :
 *   interruptin PinName, functionName
 *              PinName : Any GPIO
 *              functionName: Any defined function. Note: Function must be defined before defining interruptin.
 *
 * */
int token_interruptIn(void) {
    STOP_CODE_EXECUTION_MINUS
        process_token(TOKEN_INTERRUPTIN);

    int InterruptInEnumName = 0;

    /* We simply save the interrupt function inside the pinname ..
       -pinName must be an DigitalIn pin
       -Function name must be correct and valid
       -GlobActiveInterruptFunc will keep the function name when it activates.
       -Only one interrupt should be active at each time.
       -Function MUST be short.. Or other interrupts will not be captured.
    */

    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);
    //1) Save enumname
    InterruptInEnumName = current_token_word;
    process_token(InterruptInEnumName);

    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);
    //2) Get Function Name

    if (current_token_word != TOKEN_VARIABLE) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_INTERRUPT_IN_FUNCTION_MISSING);
        return quitScriptProgram(1);
    }
    GlobalGP* interruptPIN = addGPIOpin_DIGITAL(InterruptInEnumName, GPIO_INTERRUPTIN);
    if (interruptPIN == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_INTERRUPT_IN_PINNAME_WRONG);
        return quitScriptProgram(1);
    }
    if (functionList == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_INTERRUPT_IN_FUNCTION_MISSING);
        return quitScriptProgram(1);
    }
    GlobalfunList* pfunScan = functionList;

    while (pfunScan->next != NULL)
        pfunScan = pfunScan->next;
    while (1) {
        if (strcmp(pfunScan->name, p_prog->txtString) == 0) {
            interruptPIN->scripPin.fun = pfunScan;
            break;
        }
        if (pfunScan->prev != NULL)
            pfunScan = pfunScan->prev;
        else
            break;
    }
    process_token(TOKEN_VARIABLE);
    varGlobActiveInterruptFunc = NULL;
    if (GlobLinkedListInterruptIn == NULL) {
        //Initialize the global active interrupt function
        GlobLinkedListInterruptIn = new(gINTstruct);

        if (GlobLinkedListInterruptIn == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_INTERRUPT_MEM_ALLOCATION_FAILED);
            return quitScriptProgram(1);
        }

        GlobLinkedListInterruptIn->enumPinName = -1;
        GlobLinkedListInterruptIn->function = NULL;
        GlobLinkedListInterruptIn->prev = NULL;
        GlobLinkedListInterruptIn->next = NULL;
    }
    else {
        //Check if we have the function already in the global variable list
        while (GlobLinkedListInterruptIn->next != NULL)
            GlobLinkedListInterruptIn = GlobLinkedListInterruptIn->next; //Go to the end.
        int found = 0;
        while (GlobLinkedListInterruptIn->prev != NULL) {
            if (GlobLinkedListInterruptIn->function == pfunScan) {
                found = 1;
                break;
            }
            GlobLinkedListInterruptIn = GlobLinkedListInterruptIn->prev;
        }
        if (found != 1) {
            while (GlobLinkedListInterruptIn->next != NULL)
                GlobLinkedListInterruptIn = GlobLinkedListInterruptIn->next;

            GlobLinkedListInterruptIn->next = new(gINTstruct);
            if (GlobLinkedListInterruptIn->next == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_INTERRUPT_MEM_ALLOCATION_FAILED);
                return quitScriptProgram(1);
            }
            GlobLinkedListInterruptIn->next->enumPinName = -1;
            GlobLinkedListInterruptIn->next->function = NULL;
            GlobLinkedListInterruptIn->next->prev = GlobLinkedListInterruptIn;
            GlobLinkedListInterruptIn->next->next = NULL;
            GlobLinkedListInterruptIn = GlobLinkedListInterruptIn->next;
        }
    }
    //if not found ..we create the linkedlist otherwise we just update it.
    GlobLinkedListInterruptIn->enumPinName = InterruptInEnumName;
    GlobLinkedListInterruptIn->function = pfunScan;
    interruptPIN->scripPin.gpio.OnePinGPIO->genInOut.interrupt_in->rise(callback(&GeneralInterruptInCallback, &GlobLinkedListInterruptIn->enumPinName));
    return 0;
}