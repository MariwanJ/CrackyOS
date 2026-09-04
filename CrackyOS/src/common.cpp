/**
 ******************************************************************************
 * @file    common.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains some functions and definitions.
 * and important variables that are used by the interpreter
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


#include "common.h"

Timer TestInstrSpee;
/**
 * Use this macro to start profiling an instruction
 * */
inline void START_PROFILE() { TestInstrSpee.reset(); TestInstrSpee.start(); }
/**
 * Use this macro to stop profiling and showing the results.
 * */
inline void STOP_PROFILE_SHOW_RESULT(int val) {
                                                TestInstrSpee.stop();
                                                char tbuf[1024];
                                                memset(tbuf, 0, 1024);
                                                auto f = chrono::duration<float>(TestInstrSpee.elapsed_time()).count();
                                                snprintf(tbuf, 1023, "val=%i/%f\n", val, (1000000 * f) );
                                                globalSerialCOMvar.Serial_pc->write(tbuf,sizeof(tbuf));}


/**Main function for fetching/Executing the instructions found i each token
    Care must be taken with this piece of code
    The beginning of the script program will be decided by the p_prog..YOU NEED TO RUN "token_jump_to (.....)"
    before calling this function ...
*  @return -1 if error , >=0 if OK.
*/

int Execute_Instructions(instr_linkedList* toPosition)
{
    //Use this timer when you need calculate the speed of each instruction.Uncomment them
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_REACHED_INSTRU_EXECUTIUON);
    unsigned char testLog = (end_of_program_token() != 1);
    int result = 0;
    while (testLog == 1) {
        if (StopExecution == 1) {
            globalSerialCOMvar.Serial_pc->attach(NULL);
            return -1; //Go out from the while.
        }

        if (current_token_word == TOKEN_EOF) {
            return -1;
        }
        /**
         *              WARNING!!!  -1 should not be returned from any function if no error occurred.
         *              -1 means there was an error and it will terminate the execution 2020-02-21
         * */
        switch (current_token_word) {
            /*
            Be aware, this Switch take care of instructions only ..i.e. No variables will be returned here.
            When an instruction uses a variable,  you have to bring it inside each instruction from the global array.
            */
        case TOKEN_CREATE_REG: {
            result = token_create_reg();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_CREATE_REG_END);
            break;
        }
        case TOKEN_DELAY: {
            result = token_delay();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_DELAY_END);
            break;
        }
        case TOKEN_FUNCTION: {
            result = token_function();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_FUNCTION_END);
            break;
        }

        case TOKEN_BAD: {
            process_token(TOKEN_BAD);
            result = -1;
            break;
        }
        case TOKEN_CLEAR: {
            result = token_serial_screen_clear();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_CLEAR_END);
            break;
        }
        case TOKEN_GEN_PWM: {
            result = token_gen_pwm();
            break;
        }
        case TOKEN_PUT: {
            result = token_put();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_PUT_END);
            break;
        }
        case TOKEN_GET: {
            result = token_get();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_GET_INST_END);
            break;
        }
        case TOKEN_APUT: {
            result = token_aput();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_APUT_END);
            break;
        }
        case TOKEN_AGET: {
            result = token_aget();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_AGET_END);
            break;
        }
        case TOKEN_BAUD_RATE: {
            result = token_baud_rate();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_BAUD_RATE_END);
            break;
        }
        case TOKEN_VARIABLE: {
            //     START_PROFILE();              //Use this code when you need to calculate speed of instructions
            result = token_variable();
            //   STOP_PROFILE_SHOW_RESULT(TOKEN_VARIABLE);    //Stop time and show results
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_VARIABLE_END);
            break;
        }
        case TOKEN_REM: {
            result = token_rem();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_REM_END);
            break;
        }
        case TOKEN_IF: {
            result = token_if();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_IF_END);
            break;
        }
        case TOKEN_LABEL: {
            result = token_label();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_LABEL_END);
            break;
        }
        case TOKEN_GOTO: {
            result = token_goto();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_GOTO_END);
            break;
        }
        case TOKEN_EXIT: {
            // STOP EXECUTION OF THE SCRIPT PROGRAM
            result = token_exit();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_GOTO_END);
            break;
        }
        case TOKEN_DELETE: {
            result = token_delete();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_DELETE_END);
            break;
        }
        case TOKEN_CALL: {
            result = token_call();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_CALL_END);
            break;
        }
        case TOKEN_FOR: {
            result = token_for_loop();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_FOR_LOOP_END);
            break;
        }
        case TOKEN_I2C_SEND: {
            result = token_i2c_send();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_I2C_SEND_END);
            break;
        }
        case TOKEN_I2C_GET: {
            result = token_i2c_get();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_I2C_GET_END);
            break;
        }
        case TOKEN_CAN_SEND: {
            result = token_can_send();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_CAN_SEND_END);
            break;
        }
        case TOKEN_CAN_GET: {
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_CAN_GET_START);
            result = token_can_get();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_CAN_GET_END);
            break;
        }
        case TOKEN_SPI_SEND: {
            result = token_spi_send();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_SPI_SEND_END);
            break;
        }
        case TOKEN_WHILE: {
            result = token_while_loop();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_WHILE_LOOP_END);
            break;
        }
        case TOKEN_PRINT: {
            result = token_print();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_PRINT_END);
            break;
        }
        case TOKEN_DEBUG_PORT: {
            // TODO: Not implemented .. do it.

            break;
        }
        case TOKEN_SERIAL_GET: {
            result = token_serial_get();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_SERIAL_GET_END);
            break;
        }
        case TOKEN_ENABLE_DEBUG:
        case TOKEN_DISABLE_DEBUG: {
            result = token_enable_debug();
            break;
        }
        case TOKEN_SEMICOLON:
        case TOKEN_RETURN:
        case TOKEN_NEW_LINE: {
            //IN ALL CASES .. WE JUST SKIP THEM.
            while (current_token_word == TOKEN_NEW_LINE)  check_and_get_next_token();
            while (current_token_word == TOKEN_RETURN)  check_and_get_next_token();
            while (current_token_word == TOKEN_SEMICOLON)  check_and_get_next_token();
            break;
        }
        case TOKEN_TICKER: {
            result = token_ticker();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_TICKER_END);
            break;
        }

        case TOKEN_INTERRUPTIN: {
            result = token_interruptIn();
            DEBUG_MESSAGE("INFO %i\n", DBG_MSG_INPUT_INTERRUPT_END);
            break;
        }

        case TOKEN_EOF: {
            result = -1;      //END OF SCRIPT PROGRAM
            break;
        }
        default: {
            // We shouldn't be here
            //BAD CODE will come to here.
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_FATAL_WE_SHOULDNT_BE_HERE0);
            return quitScriptProgram(1); //Exit the program.
        }
        }
        if (toPosition != NULL)
            testLog = (p_prog->index < toPosition->index) && (current_token_word != TOKEN_EOF) && (result != -1);
        else {
            testLog = (end_of_program_token() != 1);
            if (testLog == 0)
                result = -1;      // It is the end of the loop .. return -1 to main
        }
    }
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_INSTRUCTION_EXECUTION_FINISHED);
    return result;
}

/**
* Create the Debug terminal
*/

int createDebugPC(void) {
    globalSerialCOMvar.Debug_pc = new specialSerial(DEBUG_TX, DEBUG_RX,/* NULL,*/ baud_rate);  //SERIAL 4
    if (globalSerialCOMvar.Debug_pc == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
        return quitScriptProgram(1);
    }
    return 1;
}

/** Clear screen (terminal) instruction*/
unsigned char token_serial_screen_clear() {
    process_token(TOKEN_CLEAR);
    clearAndHome();
    return 0;
}

/**Execute the script program. Important things happen here.
*  @return -1 if error , >=0 if OK.
*/
int scriptSystem_script_run()
{
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_RUN_SCRIPT_PROG);
    return Execute_Instructions(NULL);
}


/**Free up the memory.
 *
 * Remove all GPIO variables.
 * */
void free_scriptGPIOVariable(void)
{
    //Clear all
    GlobalGP* scanGlGPIO = globalGPIOvariable;
    if (scanGlGPIO == NULL)
        return; //Nothing to do
    while (scanGlGPIO->next != NULL)
        scanGlGPIO = scanGlGPIO->next; //Go to the end of the list

    while (scanGlGPIO != NULL) {
        GlobalGP* temp = scanGlGPIO;
        scanGlGPIO = scanGlGPIO->prev;
        freeOneGPIOVariable(&temp);
    }
    return; //Job finished
}

/** Delete global variable and free mem
*Syntax:
* delete VAR_NAME;
* Warning: Don't mix this with del FN/filename/dir
* @return -1 if error , >=0 if OK.
*/
int token_delete()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS;
    process_token(TOKEN_DELETE);
    if (current_token_word != TOKEN_VARIABLE) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
        return quitScriptProgram(1);
    }
    if (deleteReg(p_prog->txtString) == 1) {
        process_token(TOKEN_VARIABLE);
        return 1;
    }
    IntializeScriptVariable(p_prog->txtString);
    process_token(TOKEN_VARIABLE);
    return 1;
}


/** goto instruction
*Syntax:
*   goto name : where name is the label in the script program defined as :name:
*  @return -1 if error , >=0 if OK.
*/
int token_goto(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS;
    process_token(TOKEN_GOTO);
    glLBL* glScan = globallabelVar;
    if (glScan == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_LABEL_NOT_FOUND);
        return quitScriptProgram(1);
    }
    while (glScan->prev != NULL) {
        glScan = glScan->prev;
    }
    while (1) {
        if (strcmp(glScan->label.name, p_prog->txtString) == 0) {
            //We found the label. Jump the program
            token_jump_to(glScan->label.end->next->next);
            return 1;
        }
        if (glScan->next != NULL)
            glScan = glScan->next;
        else
            break;
    }
    // We shouldn't be here
    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_GOTO_WRONG);
    return quitScriptProgram(1);//Exit the program;
}


/**
*   REMARK instruction
*   Syntax : REM <REM BODY>
*   This should take care of the text found after the REM. Skip the line
*   NOTE:  Inside REM you shouldn't use symbols like { } since it is used for while & for loops. This will cause problem for while and loop code.
*   @return 1 if no error or -1 if error
*/
int token_rem(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS;

    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_REM_START);
    process_token(TOKEN_REM);
    //p_prog_next should point the the new line after the rem -... line

    /*This means that the REM should be a separate line in the program.
     * Any text after REM will be counted as REM body if you don't put \n or ;
     * */
    return 1;
}



/** Delay instruction
*syntax:
*Delay instruction should be like that delay  NUMBER, TYPE
*  delay 500, msec
*  delay 100, usec
*  @return -1 if error , >=0 if OK.
*/
int token_delay()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS;

    int value = 0;
    char time_buf[10];
    memset(time_buf, '\0', 10);
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_DELAY_START);
    process_token(TOKEN_DELAY);

    switch (current_token_word) {
    case TOKEN_VARIABLE: {
        // We have a variable, we need to retrieve the value.
        value = RetrieveValueInGlobalVariable();
        break;
    }
    case TOKEN_NUMBER: {
        value = convert_text_to_number();
        process_token(TOKEN_NUMBER);
        break;
    }

    default: {
        //Bad token ..
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_DELAY_VALUE_NOT_FOUND);
        return quitScriptProgram(1);
    }
    }

    if (current_token_word != TOKEN_COMMA) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_COMMA_NOT_FOUND);
        return quitScriptProgram(1);
    }
    process_token(TOKEN_COMMA);
    switch (current_token_word) {
    case TOKEN_SEC: {
        rtos::ThisThread::sleep_for(chrono::seconds((int)value));
        process_token(TOKEN_SEC);
        break;
    }
    case TOKEN_MSEC: {
        rtos::ThisThread::sleep_for(chrono::milliseconds(value));
        process_token(TOKEN_MSEC);
        break;
    }
    case TOKEN_USEC: {
        wait_us((uint32_t)value);
        process_token(TOKEN_USEC);
        break;
    }
    default: {
        // We shouldn't be here .. Bad instruction but we make a delay of 1msec
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_DELAY_NOT_DEFINED);
        return quitScriptProgram(1);
    }
    }
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_DELAY_END);
    return 1;
}



/** exit/quit instruction
*Syntax:
*   exit;
* or
*   quit;
*  @return -1 if error , >=0 if OK.
*/

int token_exit(void) {
    /* We simply stop the execution of the program.*/
    process_token(TOKEN_EXIT);
    StopExecution = 1;
    globalSerialCOMvar.Serial_pc->attach(&ChangeInterActiveModeStatus);
    InterActiveMode = 1;
    return quitScriptProgram(1);
}
