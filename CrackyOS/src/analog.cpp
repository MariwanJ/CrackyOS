/**
 ******************************************************************************
 * @file    analog.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains Analog IO instructions and definitions.
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
 /** Put analog voltage/value to the GPIO.
 *   Syntax of the instruction is like follow:
 *   aput GPIO_pin_name , variable             Variable or
 *   aput GPIO_pin_name,  value                Number (number is between 0 to 0xFFFF)
 *
 */
int token_aput()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        int value = 0;
    GlobalGP* ap_scan = NULL;
    GlobalGP* gap_scan = NULL;
    process_token(TOKEN_APUT);
    switch (current_token_word) {
    case TOKEN_VARIABLE: {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_BUS_DOES_NOT_SUPPORT_ANALOG_OUT);
        return quitScriptProgram(1);
    }
    default: {
        if (correctAnOutPinName(current_token_word) != 1) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_WRONG_GPIO);
            return quitScriptProgram(1);
        }
        ap_scan = findGPIOpin(-1);      //Find the enum pin in the global variable for GPIO
        if (ap_scan != NULL) {
            if (ap_scan->scripPin.tag != GPIO_OUT_ANALOG) {
                // We have a wrong type of pin. Not digital - remove it totally
                freeOneGPIOVariable(&ap_scan);
                ap_scan = NULL;
            }
        }

        if (ap_scan == NULL) {
            /* We have a new GPIO pin that is not in the global variable.
            We need to save it in the list*/

            gap_scan = globalGPIOvariable;
            if (gap_scan != NULL) {
                while (gap_scan->next != NULL)
                    gap_scan = gap_scan->next; //Find the last item
                gap_scan->next = new GlobalGP;
                if (gap_scan->next == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GAP_SCAN);
                    return quitScriptProgram(1);
                }

                gap_scan->next->prev = gap_scan;
                gap_scan->next->next = NULL;
                gap_scan = gap_scan->next;
                ap_scan = gap_scan;
            }
            else {
                //Here, Global variable is empty. add new item.
                gap_scan = new GlobalGP();
                if (gap_scan == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GAP_SCAN);
                    return quitScriptProgram(1);
                }
                gap_scan->prev = NULL;
                gap_scan->next = NULL;
                ap_scan = gap_scan;
                globalGPIOvariable = ap_scan;
            }
            //Fix the link
            //ap_scan->scripPin.gpio.busGpio=NULL;  // We don't have a bus
            ap_scan->scripPin.gpio.OnePinGPIO = new pinGPIO();
            ap_scan->scripPin.gpio.OnePinGPIO->pinNameEnum = current_token_word;
            ap_scan->scripPin.tag = GPIO_OUT_ANALOG;
            GlobalGPIO_REG->add(ap_scan->scripPin.gpio.OnePinGPIO->pinNameEnum, FTOKEN_DIGITAL_IN_OUT);
            ap_scan->scripPin.gpio.OnePinGPIO->genInOut.aOutPin = new AnalogOut(retrievePinNameFromTOKEN_NAME(current_token_word));
            if (ap_scan->scripPin.gpio.OnePinGPIO->genInOut.aOutPin == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GENINOUT_AOUTPIN);
                return quitScriptProgram(1);
            }
        }
        process_token(current_token_word);
        break;
    }
    }

    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);
    int sign = 1;
    if (current_token_word == TOKEN_MINUS) { sign = -1; process_token(TOKEN_MINUS); }
    switch (current_token_word) {
    case TOKEN_VARIABLE: {
        value = (int)RetrieveValueInGlobalVariable() * sign;
        break;
    }
    case TOKEN_NUMBER: {
        value = (int)convert_text_to_number() * sign;
        process_token(TOKEN_NUMBER);
        break;
    }
    default: {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARGUMENT_APUT);
        return quitScriptProgram(1);
    }
    }
    if (value < 0)
        value = 0;
    else if (value > 0xFFFF)
        value = 0xFFFF;
    ap_scan->scripPin.gpio.OnePinGPIO->genInOut.aOutPin->write_u16((uint16_t)value);
    return 0;
}

/** Get digital input data. You must provide the instruction a variable name
*   Syntax of the instruction is like bellow:
*   aget GPIO_pin_name, variable     Variable is float and is a percentage of the real value.
*   Multiply with VCC to get the correct voltage
*
*   Note: Only one pin name should be used. No support for register.
*/
int token_aget()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

    GlobalGP* ag_scan = NULL;
    GlobalGP* gag_scan = NULL;
    int varPosition = 0;
    float value = 0.0;
    process_token(TOKEN_AGET);       // Get next token word.

    switch (current_token_word) {
    case TOKEN_VARIABLE: {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_BUS_DOES_NOT_SUPPORT_ANALOG_GET);
        return quitScriptProgram(1);
    }
    default: {
        if (correctAnInPinName(current_token_word) != 1) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_WRONG_GPIO);
            return quitScriptProgram(1);
        }
        ag_scan = findGPIOpin(-1);
        if (ag_scan != NULL) {
            if (ag_scan->scripPin.tag != GPIO_IN_ANALOG) {
                // Found GPIO has wrong type
                freeOneGPIOVariable(&ag_scan);
                ag_scan = NULL;
            }
        }
        if (ag_scan == NULL) {
            /* We have a new GPIO pin that is not in the global variable.
            We need to save it in the list*/

            gag_scan = globalGPIOvariable;
            if (gag_scan != NULL) {
                while (gag_scan->next != NULL)
                    gag_scan = gag_scan->next; //Find the last item
                gag_scan->next = new GlobalGP;
                gag_scan->next->prev = gag_scan;
                gag_scan->next->next = NULL;
                gag_scan = gag_scan->next;
                ag_scan = gag_scan;
            }
            else {
                gag_scan = new GlobalGP;
                if (gag_scan == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GET_INST);
                    return quitScriptProgram(1);
                }
                gag_scan->next = NULL;
                gag_scan->prev = NULL;
                ag_scan = gag_scan;
                globalGPIOvariable = gag_scan;
            }
            ag_scan->scripPin.tag = GPIO_IN_ANALOG;
            ag_scan->scripPin.gpio.busGpio = NULL;
            ag_scan->scripPin.gpio.OnePinGPIO = new pinGPIO();
            if (ag_scan->scripPin.gpio.OnePinGPIO == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                return quitScriptProgram(1);
            }
            ag_scan->scripPin.gpio.OnePinGPIO->pinNameEnum = current_token_word;
            GlobalGPIO_REG->add(ag_scan->scripPin.gpio.OnePinGPIO->pinNameEnum, FTOKEN_DIGITAL_IN_OUT);
            ag_scan->scripPin.gpio.OnePinGPIO->genInOut.aInPin = new AnalogIn(retrievePinNameFromTOKEN_NAME(current_token_word));
            if (ag_scan->scripPin.gpio.OnePinGPIO->genInOut.aInPin == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_INOUT);
                return quitScriptProgram(1);
            }
        }
        process_token(current_token_word);
        break;
    }
    }

    //"2nd argument
    value = ag_scan->scripPin.gpio.OnePinGPIO->genInOut.aInPin->read();
    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);
    if (current_token_word != TOKEN_VARIABLE) {
        //Syntax error .. No variable after the in function.
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARGUMENT_VAR_NOT_FOUND_AGET);
        return quitScriptProgram(1);
    }
    // We have everything save the value
    char* strnamebuf;
    int stringLength = strlen(p_prog->txtString);
    strnamebuf = (char*)calloc(stringLength + 1, sizeof(char));
    if (strnamebuf == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
        return quitScriptProgram(1);
    }

    //snprintf(strnamebuf,stringLength+1,"%s",p_prog->txtString);
    strncpy(strnamebuf, p_prog->txtString, stringLength); //We don't need to add '\0' as we uses calloc
    varPosition = TestSystem_get_variable(strnamebuf);
    if (varPosition == -1) {
        //No variable found .. create it.
        varPosition = TestSystem_get_empty_variable_pos();

        if (varPosition < 0) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_OVERFLOW);
            return quitScriptProgram(1);
        }
        addNewScriptVariable(strnamebuf, T_FLOAT, 1, varPosition);
        *script_variables[varPosition].Value.pf = value;
        strnamebuf = NULL;
    }
    else {
        if (script_variables[varPosition].tag == T_FLOAT) {
            *script_variables[varPosition].Value.pf = value;
        }
        else if (script_variables[varPosition].tag == T_INT) {
            script_variables[varPosition].tag = T_FLOAT;
            if(script_variables[varPosition].Value.pi!=NULL)
            	delete script_variables[varPosition].Value.pi;
            script_variables[varPosition].Value.pi = NULL;
            script_variables[varPosition].Value.pf = new float[1];
            if (script_variables[varPosition].Value.pf == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                return quitScriptProgram(1);
            }

            *script_variables[varPosition].Value.pf = value;
        }
        else {
            if (script_variables[varPosition].tag == T_STRING) {
                script_variables[varPosition].tag = T_FLOAT;
                if(script_variables[varPosition].Value.txtvalue!=NULL)
                    delete[] script_variables[varPosition].Value.txtvalue;


                script_variables[varPosition].Value.txtvalue = NULL;
                script_variables[varPosition].Value.pf = new float[1];
                script_variables[varPosition].sizeOfArray = 1;
                if (script_variables[varPosition].Value.pf == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                    return quitScriptProgram(1);
                }
                *script_variables[varPosition].Value.pf = value;
            }
        }
    }
    if (strnamebuf != NULL) {
        free(strnamebuf);
        strnamebuf = NULL;
    }
    process_token(TOKEN_VARIABLE);
    return 1;
}

/** Generate PWM signal instruction
*       Syntax: pwm PinName, period, pulswidth
*      Note: period and pulswidth are in sec and should be float numbers.
*/
int token_gen_pwm(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        float Period = 0;
    float PulsWidth = 0;
    PinName tmpPinname;
    specialPwmOut currentPWM;
    process_token(TOKEN_GEN_PWM);
    if (correctPWMpinName(current_token_word) != 1) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_PWM_PINNAME_WRONG);
        return quitScriptProgram(1); //Exit the program.
    }

    tmpPinname = retrievePinNameFromTOKEN_NAME(current_token_word);
    GlobalGPIO_REG->add(current_token_word, FTOKEN_PWM);
    if (PWM_Var.pwm1.pwm == NULL) {
        /*When pwm1 is =NULL, it means that we don't have any PWM generator.
         We need to shift the pointers whenever a pwm generator disables*/

        PWM_Var.pwm1.pwm = new PwmOut(tmpPinname);
        if (PWM_Var.pwm1.pwm == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_PWM_VAR);
            return quitScriptProgram(1);
        }
        PWM_Var.pwm1.enum_pinname = current_token_word;
        currentPWM.pwm = PWM_Var.pwm1.pwm;
        currentPWM.enum_pinname = current_token_word;
    }
    else if (PWM_Var.pwm1.enum_pinname == current_token_word) {
        currentPWM.pwm = PWM_Var.pwm1.pwm;
        currentPWM.enum_pinname = current_token_word;
    }
    else if (PWM_Var.pwm2.pwm == NULL) {
        /*When pwm2 is =NULL, it means that we don't have any PWM generator.
         We need to shift the pointers whenever a pwm generator disables*/
        PWM_Var.pwm2.pwm = new PwmOut(tmpPinname);
        if (PWM_Var.pwm2.pwm == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_PWM_VAR);
            return quitScriptProgram(1);
        }
        PWM_Var.pwm2.enum_pinname = current_token_word;
        currentPWM.pwm = PWM_Var.pwm1.pwm;
        currentPWM.enum_pinname = current_token_word;
    }
    else if (PWM_Var.pwm2.enum_pinname == current_token_word) {
        currentPWM.pwm = PWM_Var.pwm2.pwm;
        currentPWM.enum_pinname = current_token_word;
    }
    else if (PWM_Var.pwm3.pwm == NULL) {
        /*When pwm3 is =NULL, it means that we don't have any PWM generator.
         We need to shift the pointers whenever a pwm generator disables*/

        PWM_Var.pwm3.pwm = new PwmOut(tmpPinname);
        if (PWM_Var.pwm3.pwm == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_PWM_VAR);
            return quitScriptProgram(1);
        }
        PWM_Var.pwm3.enum_pinname = current_token_word;
        currentPWM.pwm = PWM_Var.pwm3.pwm;
        currentPWM.enum_pinname = current_token_word;
    }
    else if (PWM_Var.pwm3.enum_pinname == current_token_word) {
        currentPWM.pwm = PWM_Var.pwm3.pwm;
        currentPWM.enum_pinname = current_token_word;
    }
    else if (PWM_Var.pwm4.pwm == NULL) {
        /*When pwm4 is =NULL, it means that we don't have any PWM generator.
         We need to shift the pointers whenever a pwm generator disables*/

        PWM_Var.pwm4.pwm = new PwmOut(tmpPinname);
        if (PWM_Var.pwm4.pwm == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_PWM_VAR);
            return quitScriptProgram(1);
        }
        PWM_Var.pwm4.enum_pinname = current_token_word;
        currentPWM.pwm = PWM_Var.pwm4.pwm;
        currentPWM.enum_pinname = current_token_word;
    }
    else if (PWM_Var.pwm4.enum_pinname == current_token_word) {
        currentPWM.pwm = PWM_Var.pwm4.pwm;
        currentPWM.enum_pinname = current_token_word;
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ONLY_4_PWM_ALLOWED);
        return quitScriptProgram(1); //Exit the program.
    }
    /* Here we know the pwm signal or we have generated*/
    process_token(current_token_word);

    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);

    int sign = 1;
    if (current_token_word == TOKEN_MINUS) { sign = -1; process_token(TOKEN_MINUS); }
    if (current_token_word == TOKEN_VARIABLE) {
        // We have a variable, we need to retrieve the value.
        Period = RetrieveValueInGlobalVariable() * sign;
    }
    else if (current_token_word == TOKEN_NUMBER) {
        Period = convert_text_to_number() * sign;
        process_token(TOKEN_NUMBER);
    }

    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_PWM_PERIOD_NOT_FOUND);
        return quitScriptProgram(1); //Exit the program.
    }

    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);
    // We search for the plus width now .
    sign = 1;
    if (current_token_word == TOKEN_MINUS) { process_token(TOKEN_MINUS); sign = -1; }
    if (current_token_word == TOKEN_VARIABLE) {
        // We have a variable, we need to retrieve the value.
        PulsWidth = RetrieveValueInGlobalVariable() * sign;
    }
    else if (current_token_word == TOKEN_NUMBER) {
        PulsWidth = convert_text_to_number() * sign;
        process_token(TOKEN_NUMBER);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_PWM_PULSWIDTH_NOT_FOUND);
        return quitScriptProgram(1); //Exit the program.
    }
    currentPWM.pwm->period(Period);
    currentPWM.pwm->pulsewidth(PulsWidth);  //Float numbers
    return 0; //Everything is OK.
}
