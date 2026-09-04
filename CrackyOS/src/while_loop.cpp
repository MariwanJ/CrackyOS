/**
 * @file    while_loop.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains the implementation of while loop function
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
#include "arithmetic.h"
 /**WHILE LOOP
  *      Syntax for this loop is as follow
  *      while(Variable logical_condition variable/number)
  *             {
  *
  *              block of code to be executed
  *             }
  *
  * @return -1 if error , >=0 if OK
  */
int token_while_loop() {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        char* bufNewTemp = NULL;
    nestedWhileloop++;          // First while-statement found.
    nestedForWhileloop = nestedForWhileloop + 1;
    //   DEBUG_MESSAGE("ERROR %i\n",DBG_MSG_SYNTAX_ERROR_ONLY_2_NESTED_LOOP_ALLOWED);
    process_token(TOKEN_WHILE);
    if (nestedWhileloop > SCRIPT_MAX_NESTED_FOR_WHILE_LOOP) {
        //We don't allow more than 2 while-loop
        DEBUG_MESSAGE("SYNTAX ERROR, ONLY 2 Nested while-LOOP is allowed\n");
        return quitScriptProgram(1);
    }
    whileExtract[nestedWhileloop - 1].varIterationPosition = -1;
    whileExtract[nestedWhileloop - 1].varLogicalRelation.position = -1;
    //1)  Start first part of the for instruction
    if (current_token_word == TOKEN_LEFT_PARENTHESIS)
        process_token(TOKEN_LEFT_PARENTHESIS);
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_LEFT_PARENTHESIS);
        return quitScriptProgram(1);
    }
    if (current_token_word == TOKEN_NUMBER) {
        //Here we have a While(1) .. infinite loop.

        whileExtract[nestedWhileloop - 1].enumLogicalOperation = TOKEN_EQUAL_EQUAL;
        whileExtract[nestedWhileloop - 1].varLogicalRelation.value = (int)convert_text_to_number();
        /*  whileExtract[nestedWhileloop - 1].varIteration = new (int);
          if (whileExtract[nestedWhileloop - 1].varIteration == NULL) {
              DEBUG_MESSAGE("ERROR %i\n",DBG_MSG_MEM_ALLOCATION_ERROR_WHILEEXTRACT);
              return quitScriptProgram(1);
          }
  */
        whileExtract[nestedWhileloop - 1].varIteration = 1;

        process_token(TOKEN_NUMBER);

        if (whileExtract[nestedWhileloop - 1].varLogicalRelation.value != 1) {
            //We support only the format while(1) ..nothing else .. all other values will generate errors
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_WHILE);
            return quitScriptProgram(1);
        }
        whileExtract[nestedWhileloop - 1].varLogicalRelation.value = 1;
        whileExtract[nestedWhileloop - 1].varLogicalRelation.variable = NULL; // No variable .. Only number
    }
    else {
        // WE should get now the variable name. we have while(variable ..etc)
        if (current_token_word != TOKEN_VARIABLE) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
            return quitScriptProgram(1);
        }

        int varPosition = TestSystem_get_variable(NULL);
        process_token(TOKEN_VARIABLE);

        if (varPosition != -1) {
            whileExtract[nestedWhileloop - 1].varIterationPosition = varPosition;
            if (script_variables[varPosition].tag == T_INT) {
                whileExtract[nestedWhileloop - 1].varIteration = *script_variables[varPosition].Value.pi;
            }
            else {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FLOAT_INSIDE_LOOP);
                return quitScriptProgram(1);
            }
        }
        else {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_USED_INSIDE_WHILE);
            return quitScriptProgram(1);
        }

        //2)  Find the logical operation now
        switch (current_token_word) {
        case TOKEN_EQUAL_EQUAL:
        case TOKEN_LESSTHAN:
        case TOKEN_LESSTHANEQUAL:
        case TOKEN_GREATERTHAN:
        case TOKEN_GREATERTHANEQUAL:
        case TOKEN_NOTEQUAL:
            whileExtract[nestedWhileloop - 1].enumLogicalOperation = current_token_word;
            break;
        default: {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_NOT_SUPPORTED);
            return quitScriptProgram(1);
        }
        }
        process_token(whileExtract[nestedWhileloop - 1].enumLogicalOperation);
        // We  need to get the value or the variable after the symbols.
        //TODO: We don't support arithmetic operation inside the while instruction to be implemented later.

        if (current_token_word == TOKEN_VARIABLE) {
            if (bufNewTemp != NULL) {
                delete[] bufNewTemp;        // Free the old buffer to use it again
                bufNewTemp = NULL;
            }
            varPosition = TestSystem_get_variable(NULL);
            process_token(TOKEN_VARIABLE);
            if (varPosition != -1) {
                if (script_variables[varPosition].tag == T_INT) {
                    whileExtract[nestedWhileloop - 1].varLogicalRelation.variable = script_variables[varPosition].Value.pi;
                    whileExtract[nestedWhileloop - 1].varLogicalRelation.position = varPosition;
                }
                else {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_WRONG);
                    return quitScriptProgram(1);
                }
            }
            else {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_USED_INSIDE_WHILE);
                return quitScriptProgram(1);
            }
        }
        else if (current_token_word == TOKEN_NUMBER || current_token_word == TOKEN_MINUS) {
            //give support to negative numbers.
            if (current_token_word == TOKEN_MINUS) {
                process_token(TOKEN_MINUS);
                whileExtract[nestedWhileloop - 1].varLogicalRelation.value = -1 * (int)convert_text_to_number();
            }
            else {
                whileExtract[nestedWhileloop - 1].varLogicalRelation.value = (int)convert_text_to_number();
            }
            whileExtract[nestedWhileloop - 1].varLogicalRelation.variable = NULL;         // No variable .. Only number
            whileExtract[nestedWhileloop - 1].varLogicalRelation.position = -1;
            process_token(TOKEN_NUMBER);
        }
    } // END searching for first part of while .. we have 'while(....), we need the block of code {...}
//3 Search for code block
    if (current_token_word == TOKEN_RIGHT_PARENTHESIS)
        process_token(TOKEN_RIGHT_PARENTHESIS);
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_PARENTHESIS_MISSING);
        return quitScriptProgram(1);
    }
    while (END_OF_LINE) {
        //Skip new lines
        check_and_get_next_token();
    }

    //Start extracting the block-code/start-end
    if (current_token_word == TOKEN_LEFT_CURLY_BRACKET) {
        process_token(TOKEN_LEFT_CURLY_BRACKET);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_LEFT_CURLY_BRACKET_NOT_FOUND);
        return quitScriptProgram(1);
    }

    /* We collected the While(....) line , find the block of the code we need to run now
     We should find the first '{'
     */
    whileExtract[nestedWhileloop - 1].p_StartWhileLoop = p_prog->prev; //Save location of the first line after the for-loop start instruction. This will be used to re-run the code

    while (current_token_word != TOKEN_RIGHT_CURLY_BRACKET && current_token_word != TOKEN_EOF) {
        if (current_token_word == TOKEN_LEFT_CURLY_BRACKET) {
            //We have a nested {} , second left '{' found
            while (current_token_word != TOKEN_RIGHT_CURLY_BRACKET && current_token_word != TOKEN_EOF) {
                check_and_get_next_token();
            }

            if (current_token_word != TOKEN_RIGHT_CURLY_BRACKET) {
                //It will fail if this search the code and cannot find the right }
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_MISSING);
                return quitScriptProgram(1);
            }
        }
        check_and_get_next_token();            // continue searching to the last instruction.
    }
    if (current_token_word != TOKEN_RIGHT_CURLY_BRACKET) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_MISSING);
        return quitScriptProgram(1);
    }

    /*
     We are ready to run the block of instructions. We know which code is the start of the while loop
     and we know also the last for statement.
     TODO: The script program support only two while-loop. Make it more general.
     */
    p_prog = p_prog->prev;    //Return back to the enum value.
    whileExtract[nestedWhileloop - 1].p_EndWhileLoop = p_prog->prev->prev; //We remove the curly bracket

    if (bufNewTemp != NULL) {
        delete[]bufNewTemp;
    }

    /*
     We have everything, start to run the while loop.
     */

    while (1) {
        int leftValue = whileExtract[nestedWhileloop - 1].varIteration;
        if (whileExtract[nestedWhileloop - 1].varIterationPosition >= 0) {
            int leftPosition = whileExtract[nestedWhileloop - 1].varIterationPosition;
            if (script_variables[leftPosition].tag != T_INT)
                return quitScriptProgram(1);
            leftValue = *script_variables[leftPosition].Value.pi;
        }
        int rightValue = whileExtract[nestedWhileloop - 1].varLogicalRelation.value;
        if (whileExtract[nestedWhileloop - 1].varLogicalRelation.position >= 0) {
            int rightPosition = whileExtract[nestedWhileloop - 1].varLogicalRelation.position;
            if (script_variables[rightPosition].tag != T_INT)
                return quitScriptProgram(1);
            rightValue = *script_variables[rightPosition].Value.pi;
        }
        if (!GetLogicalOperationAnswer(whileExtract[nestedWhileloop - 1].enumLogicalOperation, leftValue, rightValue))
            break;
        token_jump_to(whileExtract[nestedWhileloop - 1].p_StartWhileLoop);
        if (Execute_Instructions(whileExtract[nestedWhileloop - 1].p_EndWhileLoop) == -1)
            break;
    }
    // DEBUG_MESSAGE("-----------WHILE LOOP END---------\n",nestedWhileloop);
     //This is necessary if the while doesn't run at all.
    if (p_prog < whileExtract[nestedWhileloop - 1].p_EndWhileLoop) {
        token_jump_to(whileExtract[nestedWhileloop - 1].p_EndWhileLoop);
    }

    while (END_OF_LINE) {
        //Skip new lines
        check_and_get_next_token();
    }

    if (current_token_word == TOKEN_RIGHT_CURLY_BRACKET) {
        process_token(TOKEN_RIGHT_CURLY_BRACKET);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_MISSING);
        return quitScriptProgram(1);
    }
    token_jump_to(p_prog->prev);
    nestedWhileloop--;
    nestedForWhileloop--;           // Check if we don't have a while-for loop

    if (nestedWhileloop >= 1 || nestedForWhileloop >= 1) {
        // We had nested while-loop. Don't return the the main loop. Return to the while loop calling this while loop.
        // Simply, we have to let this while loop acts like other function. They return the results only .. they don't jump to other functions.
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_NESTED_WHILE_LOOP);
        return 0;
    }
    else if (nestedWhileloop == 0) {
        DEBUG_MESSAGE("INFO %i\n", DBG_MSG_WHILE_LOOP_END);
        return 0;   //Execute_Instructions(NULL); //RUN Rest the code as normal.
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_NESTEDFORLOOP_NESTEDWHILELOOP_NOT_ZERO);
        return quitScriptProgram(1);
    }
    return 0;
}