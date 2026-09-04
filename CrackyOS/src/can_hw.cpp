/**
 ******************************************************************************
 * @file    can_hw.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains CAN instructions.
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

#include "hardware_used.h"
#include "types.h"

 /** CAN send instruction
 * Important Note: Can devices must get isolated power supply. Don't connect GND of the CAN transmitter to the Nucleo GND.
 * Use external adapter (for TJA1051T VCC = +5V).
 *     The syntax of this script is as follow:
 *
 *   can_send PinName1, PinName2,ResultVariable ,FREQ, CANID, CAND_DATA0,...CAN_DATA8
 *   PinName1            1) CAN RD pin as defined in the manual.
 *   PinName2            2) CAN TD pin as defined in the manual.
 *   ResultVariable      3) A variable keep the results of the can write process( 0 if write failed, 1 if write was successful)
 *   FREQ                4) Frequency of the CAN bus
 *   CANID               5) Address of the can device
 *   CAN_DATA0..8        6) An array of 8 bytes representing the data to be sent via can bus.
 *
 */
int token_can_send()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        canType* pScan = gCanVariable;
    CANMessage msg;
    int results = 0;
    int counter = 0;
    int stringLength = 0;
    int varPosition = 0;
    //int varPosition2 ;
    //instr_linkedList* pScanProg;
    int found = 0;

    // reset the msg variable
    for (int s = 0; s < 8; s++) {
        msg.data[s] = 0;
    }

    char* var_buf = NULL;
    int freq = 0;
    process_token(TOKEN_CAN_SEND);

    if (correctCANpinName(current_token_word) != 1) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_WRONG_GPIO_CAN);
        return quitScriptProgram(1); //Exit the program.
    }

    if (pScan == 0) {
        pScan = new canType;
        if (pScan == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_OBJECT_ALLOCATION_ERROR_CAN);
            return quitScriptProgram(1); //Exit the program.
        }
        pScan->next = NULL;
        pScan->prev = NULL;

        pScan->current.CANRD = current_token_word;
        gCanVariable = pScan;
        process_token(current_token_word);
        if (current_token_word == TOKEN_COMMA) {
            process_token(TOKEN_COMMA);
        }
    }
    else {
        //Not zero we have a defined can.
        //We have CAN device defined. We must search for the device
        while (pScan->prev != NULL)
            pScan = pScan->prev;
        found = 0;
        while (1) {
            if (pScan->current.CANRD == current_token_word) {
                found = 1;
                break;
            }
            if (pScan->next == NULL)
                break;//Go out
            else
                pScan = pScan->next;
        }

        if (found) {
            //We have the CAN defined.
            process_token(current_token_word);
            if (current_token_word == TOKEN_COMMA) {
                process_token(TOKEN_COMMA);
            }
            if (pScan->current.CANTD != current_token_word) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_CANRD_WRONG);
                return quitScriptProgram(1);
            }
        }
        else {
            //Not found add new CAN.
            while (pScan->next != NULL)
                pScan = pScan->next;  // bring the last item.
            pScan->next = NULL;
            pScan->next = new canType;
            if (pScan->next == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_CANRD_WRONG);
                return quitScriptProgram(1); //Exit the program.
            }
            pScan->next->next = NULL;
            pScan->next->prev = pScan;
            pScan = pScan->next;
            pScan->current.CANRD = current_token_word;
            pScan->_CanDevice = NULL;
            if (current_token_word == TOKEN_COMMA) {
                process_token(TOKEN_COMMA);
            }
        }
    }
    if (found) {
        if (pScan->current.CANTD != current_token_word) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_CANTD_WRONG);
            return quitScriptProgram(1);
        }
        process_token(current_token_word);
    }
    else {
        //We haven't defined any can devices. Create a new can device.
        if (correctCANpinName(current_token_word) != 1) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_WRONG_GPIO_CAN);
            return quitScriptProgram(1); //Exit the program.
        }
        pScan->_CanDevice = NULL;
        pScan->current.CANTD = current_token_word;
        process_token(current_token_word);
    }

    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    /*
    //Here we have pinNames, can address
    if (pScan->_CanDevice == NULL) {
        pScan->_CanDevice = new CAN(retrievePinNameFromTOKEN_NAME(pScan->current.CANRD), retrievePinNameFromTOKEN_NAME(pScan->current.CANTD));
        if (pScan->_CanDevice == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_OBJECT_ALLOCATION_ERROR_CAN);
            return quitScriptProgram(1); //Exit the program.
        }
    }

    move this section as changing frequency causes problem.
*/
// 3) Here we have the device defined or found.. Pick the variable
    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    if (current_token_word != TOKEN_VARIABLE) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARGUMENT_VAR_NOT_FOUND_CANWRITE);
        return quitScriptProgram(1); //Exit the program.
    }
    stringLength = strlen(p_prog->txtString);

    var_buf = (char*)calloc(sizeof(char) * (stringLength + 1), sizeof(char));
    if (var_buf == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_VAR_BUF);
        return quitScriptProgram(1);//Exit the program
    }

    strncpy(var_buf, p_prog->txtString, stringLength); //We don't need to add '\0' as we uses calloc
    varPosition = TestSystem_get_variable(var_buf);
    process_token(TOKEN_VARIABLE);           // Get next token word.

    if (varPosition == -1) {
        //Variable not found .. save new variable
        varPosition = TestSystem_get_empty_variable_pos();
        if (varPosition < 0) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_SCRIPT_VARIABLE_OVERFLOW);
            return quitScriptProgram(1);
        }
    }
    else {
        //remove old variable and recreate it
        if (script_variables[varPosition].tag == T_INT && script_variables[varPosition].Value.pi!=NULL)   delete script_variables[varPosition].Value.pi;
        else if (script_variables[varPosition].tag == T_FLOAT && script_variables[varPosition].Value.pf!=NULL) delete script_variables[varPosition].Value.pf;
        else if (script_variables[varPosition].tag == T_STRING && script_variables[varPosition].Value.txtvalue!=NULL) delete script_variables[varPosition].Value.txtvalue;
        script_variables[varPosition].Value.pi = NULL;
        script_variables[varPosition].Value.pf = NULL;
        script_variables[varPosition].Value.txtvalue = NULL;
    }
    // Allocate the new variable.
    addNewScriptVariable(var_buf, T_INT, 1, varPosition);
    var_buf = NULL;

    if (var_buf != NULL) {
        delete[]var_buf;
        var_buf = NULL;
    }

    // We have the result variable  - Pick the MSG parameters
    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    // 4)     Frequency of the CAN bus
    if (current_token_word == TOKEN_NUMBER) {
        freq = convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }
    else if (current_token_word == TOKEN_VARIABLE) {
        freq = (int)RetrieveValueInGlobalVariable();
        process_token(TOKEN_VARIABLE);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_CAN_ADDRESS_NOT_FOUND);
        return quitScriptProgram(1); //Exit the program.
    }

    //5) Message ID

    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    if (current_token_word == TOKEN_NUMBER) {
        msg.id = (int)convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }
    else if (current_token_word == TOKEN_VARIABLE) {
        msg.id = (int)RetrieveValueInGlobalVariable();
        process_token(TOKEN_VARIABLE);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_CAN_ADDRESS_NOT_FOUND);
        return quitScriptProgram(1); //Exit the program.
    }

    //6) Message data
    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }

    if (current_token_word == TOKEN_VARIABLE) {
        int varPosition2;
        varPosition2 = TestSystem_get_variable(NULL);
        if (varPosition2 == -1) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
            return quitScriptProgram(1); //Exit the program.
        }
        process_token(TOKEN_VARIABLE);
        for (int j = 0; j < script_variables[varPosition2].sizeOfArray && j < 8; j++) {
            if (script_variables[varPosition2].tag == T_INT) {
                msg.data[j] = script_variables[varPosition2].Value.pi[j];
            }
            else if (script_variables[varPosition2].tag == T_FLOAT) {
                msg.data[j] = (int)script_variables[varPosition2].Value.pf[j];
            }
            else if (script_variables[varPosition2].tag == T_STRING) {
                msg.data[j] = script_variables[varPosition2].Value.txtvalue[j];
            }
            counter++;
        }
    }
    else {
        // We have only numbers
        //First find the end of the line
        //Find the end of the line first
        instr_linkedList* pScanProg;
        pScanProg = RetrieveEndOfInstruction();
        while (current_token_word == TOKEN_NUMBER && p_prog->index < pScanProg->index) {
            msg.data[counter] = (int)convert_text_to_number();     //Collect message data
            process_token(TOKEN_NUMBER);
            counter++;
            if (counter >= 8)
                break; //We shouldn't have more than 8 items.
            if (current_token_word == TOKEN_COMMA) {
                process_token(TOKEN_COMMA);
            }
        }
    }
    msg.len = counter;         //Length of the message
    msg.format = CANStandard;  //MSG format - standard

    //Here we have pinNames, can address
    if (pScan->_CanDevice == NULL) {
        pScan->_CanDevice = new CAN(retrievePinNameFromTOKEN_NAME(pScan->current.CANRD), retrievePinNameFromTOKEN_NAME(pScan->current.CANTD), freq);
        if (pScan->_CanDevice == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_OBJECT_ALLOCATION_ERROR_CAN);
            return quitScriptProgram(1); //Exit the program.
        }
    }

    /* This causes problem .. so we must put it inside the new CAN definition. We shouldn't allow frequency change.
     * pScan->_CanDevice->frequency(freq);
     * */
    results = pScan->_CanDevice->write(msg);      //Return 1 if sending succeed, 0 if failed.
    *script_variables[varPosition].Value.pi = results;
    return results;
}

/** CAN get instruction
*     The syntax of this script is as follow:
*
*   can_get PinName1, PinName2, FREQ, CANID, VARIABLE
*   PinName1            1) CAN H pin as defined in the manual.
*   PinName2            2) CAN L pin as defined in the manual.
*   ResultVariable      3) A variable keep the results of the can write process( 0 if write failed, 1 if write was successful)
*   FREQ                4) Frequency of the CAN bus
*   VARIABLE            5) Data received by this device (an array of 8 bits)
*
*/
int token_can_get() {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        canType* pScan = gCanVariable;
    CANMessage msg;
    int results = 0;
    int stringLength = 0;
    int varPosition = 0;
    int found = 0;
    // reset the msg variable
    for (int s = 0; s < 8; s++) {
        msg.data[s] = 0;
    }

    char* var_buf = NULL;
    int freq = 0;
    process_token(TOKEN_CAN_GET);
    if (correctCANpinName(current_token_word) != 1) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_WRONG_GPIO_CAN);
        return quitScriptProgram(1); //Exit the program.
    }

    if (pScan == 0) {
        pScan = new canType;
        if (pScan == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_OBJECT_ALLOCATION_ERROR_CAN);
            return quitScriptProgram(1); //Exit the program.
        }
        pScan->next = NULL;
        pScan->prev = NULL;

        pScan->current.CANRD = current_token_word;
        gCanVariable = pScan;
        process_token(current_token_word);
        if (current_token_word == TOKEN_COMMA) {
            process_token(TOKEN_COMMA);
        }
    }
    else {
        //Not zero we have a defined can.
        //We have CAN device defined. We must search for the device
        while (pScan->prev != NULL)
            pScan = pScan->prev;
        found = 0;
        while (1) {
            if (pScan->current.CANRD == current_token_word) {
                found = 1;
                break;
            }
            if (pScan->next == NULL)
                break; //Go out
            else
                pScan = pScan->next;
        }

        if (found) {
            //We have the CAN defined.
            process_token(current_token_word);
            if (current_token_word == TOKEN_COMMA) {
                process_token(TOKEN_COMMA);
            }
            if (pScan->current.CANTD != current_token_word) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_CANRD_WRONG);
                return quitScriptProgram(1);
            }
        }
        else {
            //Not found add new CAN.
            while (pScan->next != NULL)
                pScan = pScan->next;  // bring the last item.
            pScan->next = NULL;
            pScan->next = new canType;
            if (pScan->next == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_OBJECT_ALLOCATION_ERROR_CAN);
                return quitScriptProgram(1); //Exit the program.
            }
            pScan->next->next = NULL;
            pScan->next->prev = pScan;
            pScan = pScan->next;
            pScan->current.CANRD = current_token_word;
            pScan->_CanDevice = NULL;
            if (current_token_word == TOKEN_COMMA) {
                process_token(TOKEN_COMMA);
            }
        }
    }
    if (found) {
        if (pScan->current.CANTD != current_token_word) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_CANRD_WRONG);
            return quitScriptProgram(1);
        }
        process_token(current_token_word);
    }
    else {
        //We haven't defined any can devices. Create a new can device.
        if (correctCANpinName(current_token_word) != 1) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_WRONG_GPIO_CAN);
            return quitScriptProgram(1); //Exit the program.
        }
        pScan->_CanDevice = NULL;
        pScan->current.CANTD = current_token_word;
        process_token(current_token_word);
    }

    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    /* We move this section to allow freq to be decided with the creation of the can object
    //Here we have pinNames, can address
    if (pScan->_CanDevice == NULL) {
        pScan->_CanDevice = new CAN(retrievePinNameFromTOKEN_NAME(pScan->current.CANRD), retrievePinNameFromTOKEN_NAME(pScan->current.CANTD));
        if (pScan->_CanDevice == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_OBJECT_ALLOCATION_ERROR_CAN);
            return quitScriptProgram(1); //Exit the program.
        }
    }
    */
    // 3) Here we have the device defined or found.. Pick the freq, ID, and variable save the results of the read operation
    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }

    // 4)     Frequency of the CAN bus
    if (current_token_word == TOKEN_NUMBER) {
        freq = convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }
    else if (current_token_word == TOKEN_VARIABLE) {
        freq = (int)RetrieveValueInGlobalVariable();
        process_token(TOKEN_VARIABLE);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_CAN_ADDRESS_NOT_FOUND);
        return quitScriptProgram(1); //Exit the program.
    }

    //5) Message ID

    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    if (current_token_word == TOKEN_NUMBER) {
        msg.id = (int)convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }
    else if (current_token_word == TOKEN_VARIABLE) {
        msg.id = (int)RetrieveValueInGlobalVariable();
        process_token(TOKEN_VARIABLE);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_CAN_ADDRESS_NOT_FOUND);
        return quitScriptProgram(1); //Exit the program.
    }

    //6) Message variable.
    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    if (current_token_word != TOKEN_VARIABLE) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARGUMENT_VAR_NOT_FOUND_CANWRITE);
        return quitScriptProgram(1); //Exit the program.
    }
    stringLength = strlen(p_prog->txtString);

    var_buf = (char*)calloc(sizeof(char) * (stringLength + 1), sizeof(char));
    if (var_buf == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_VAR_BUF);
        return quitScriptProgram(1); //Exit the program
    }

    strncpy(var_buf, p_prog->txtString, stringLength); //We don't need to add '\0' as we uses calloc
    varPosition = TestSystem_get_variable(var_buf);
    process_token(TOKEN_VARIABLE); // Get next token word.

    if (varPosition == -1) {
        //Variable not found .. save new variable
        varPosition = TestSystem_get_empty_variable_pos();
        if (varPosition < 0) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_SCRIPT_VARIABLE_OVERFLOW);
            return quitScriptProgram(1);
        }
    }
    else {
        //remove old variable and recreate it
        if (script_variables[varPosition].tag == T_INT)   delete[] script_variables[varPosition].Value.pi;
        else if (script_variables[varPosition].tag == T_FLOAT) delete[] script_variables[varPosition].Value.pf;
        else if (script_variables[varPosition].tag == T_STRING) delete[] script_variables[varPosition].Value.txtvalue;
        script_variables[varPosition].Value.pi = NULL;
        script_variables[varPosition].Value.pf = NULL;
        script_variables[varPosition].Value.txtvalue = NULL;
    }
    // Allocate the new variable.
    addNewScriptVariable(var_buf, T_INT, 1, varPosition);
    var_buf = NULL;

    if (var_buf != NULL) {
        delete[]var_buf;
        var_buf = NULL;
    }

    // We have everything.

        //Here we have pinNames, can address and freq
    if (pScan->_CanDevice == NULL) {
        pScan->_CanDevice = new CAN(retrievePinNameFromTOKEN_NAME(pScan->current.CANRD), retrievePinNameFromTOKEN_NAME(pScan->current.CANTD), freq);
        if (pScan->_CanDevice == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_OBJECT_ALLOCATION_ERROR_CAN);
            return quitScriptProgram(1); //Exit the program.
        }
    }

    /*
     * We don't allow frequency change after creation as it causes problem.
      pScan->_CanDevice->frequency(freq);
  */
  // Initialize the MSG variable
    msg.id = msg.len = 0;
    for (int s = 0; s < 8; s++) {
        msg.data[s] = 0;
    }
    results = pScan->_CanDevice->read(msg);
    if (results != 1) {
        return 0;
    }

    //return 0; ///We have not received anything .. just go out.. You shouldn't return -1
    script_variables[varPosition].Value.pi = new int[msg.len];
    if (script_variables[varPosition].Value.pi == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_CAN_SEND);
        return quitScriptProgram(1);
    }
    script_variables[varPosition].sizeOfArray = msg.len;
    for (int s = 0; s < msg.len; s++) {
        script_variables[varPosition].Value.pi[s] = msg.data[s];
    }
    return results;
}
