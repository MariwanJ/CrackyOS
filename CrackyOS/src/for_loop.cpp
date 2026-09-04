/**
 ******************************************************************************
 * @file    for_loop.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains for_loop instruction.
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
 // This variable will count the no. of for-loop instructions inside a block
unsigned char nestedForloop = 0;
// This variable will count the no. of while-loop instructions inside a block
unsigned char nestedWhileloop = 0;
// This will be used when we have for & while loops used inside each other.
unsigned char nestedForWhileloop = 0;

/**For loop
*Syntax for this loop is as follow
*for (VAR1=NO, var2=?CONDITION , VAR3++/--/..etc)
*{    block of code to be executed    }
*NOTE:Please notice that between each part there is a comma ..not a semicolon.
**/
int token_for_loop()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        /* var1: is just the string, we save the value in the global variables.
           var2: is a struct .. it keep the value and the name.
           var3: is a struct also as above.
        */
        instr_linkedList* SaveProg = NULL;
    int varPosition;
    nestedForWhileloop++;    // One step up .. If we have a combination of two while and for instructions, this will result in a value of 2. or more.
    nestedForloop++;         // First For statement found.

    char* nameOfVariable = NULL;
    if (nestedForloop > SCRIPT_MAX_NESTED_FOR_WHILE_LOOP) {        //We don't allow more than 2 For-loop
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ONLY_2_NESTED_LOOP_ALLOWED);
        return quitScriptProgram(1);
    }
    //We must initialize them to avoid error .. if varIteration3 is - so we don't have any logical comparision with a variable.
    forExtract[nestedForloop - 1].varIteration1 = -1; // First variable  (here ...
    forExtract[nestedForloop - 1].varIteration2 = -1; // Initiator of the first variable    (i=here
    forExtract[nestedForloop - 1].varIteration3 = -1; // Variable for the logical operation (...,i<here
  //  forExtract[nestedForloop - 1].varIteration4=-1;// variable after the logical operation(...,..., here ..)

    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_FOR_LOOP_START);
    process_token(TOKEN_FOR);
    int tempVariable = 0;
    //1) Start first part of the 'for' instruction
    if (current_token_word == TOKEN_LEFT_PARENTHESIS) {
        process_token(TOKEN_LEFT_PARENTHESIS);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_LEFT_PARENTHESIS);
        return quitScriptProgram(1);
    }

    // WE should get now the variable name
    if (current_token_word == TOKEN_VARIABLE) {
        tempVariable = strlen(p_prog->txtString);
        nameOfVariable = (char*)calloc(tempVariable + 1, sizeof(char));
        if (nameOfVariable == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FOREXTRACT);
            return quitScriptProgram(1);
        }

        //snprintf(nameOfVariable, tempVariable,"%s", p_prog->txtString);
        strncpy(nameOfVariable, p_prog->txtString, tempVariable); //We don't need to add '\0' as we uses calloc

        process_token(TOKEN_VARIABLE);
        if (current_token_word == TOKEN_EQUAL) {
            process_token(TOKEN_EQUAL);
        }
        else {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_EQUAL_NOT_FOUND);
            free(nameOfVariable);
            nameOfVariable = NULL;
            return quitScriptProgram(1);
        }
        int sign = 1;
        //The number could be a negative number .. so we have to give support for negative number.
        if (current_token_word == TOKEN_MINUS) {
            process_token(TOKEN_MINUS);
            sign = -1;
        }
        float value;
        if (current_token_word == TOKEN_NUMBER) {
            value = convert_text_to_number() * sign;
            process_token(TOKEN_NUMBER);
        }
        else if (current_token_word == TOKEN_VARIABLE) {
            //Initialization of the variable inside for is by a variable.  i.e. ( ex for(i=MYVAR, ...etc
            int varPosition2 = TestSystem_get_variable(NULL) * sign;
            process_token(TOKEN_VARIABLE);

            if (varPosition2 < 0) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_INIT_VAR_FOR_LOOP_NOT_FOUND);
                return quitScriptProgram(1);
            }
            if (script_variables[varPosition2].sizeOfArray > 1) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARRAY_USED_INITIALIZE_FOR_LOOP);
                return quitScriptProgram(1);
            }
            // save the initiator value in value variable and use it later
            if (script_variables[varPosition2].tag == T_INT)
                value = *script_variables[varPosition2].Value.pi;
            else if (script_variables[varPosition2].tag == T_FLOAT)
                value = *script_variables[varPosition2].Value.pf;
            else {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FOR_LOOP_INITIALIZE);
                return quitScriptProgram(1);
            }
        }
        else {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_INIT_VAR_NOT_FOUND);
            return quitScriptProgram(1);
        }
        // Save the variable name used inside the for{i.e. for(VARIABLE=...)} it as a global variable to be used later
        varPosition = TestSystem_get_variable(nameOfVariable);
        if (varPosition == -1) {
            //Create the variable.
            varPosition = TestSystem_get_empty_variable_pos();
            if (varPosition < 0) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_OVERFLOW);
                return quitScriptProgram(1);
            }

            addNewScriptVariable(nameOfVariable, T_INT, 1, varPosition);
            nameOfVariable = NULL;
            //Give it the value we picked up either from a number or from a variable.
            *script_variables[varPosition].Value.pi = (int)value;
        }
        else {
            script_variables[varPosition].tag = T_INT;
            *script_variables[varPosition].Value.pi = (int)value;
        }
        forExtract[nestedForloop - 1].varIteration1 = varPosition;
    }

    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FOR_LOOP_VARIABLE_NOT_FOUND);
        return quitScriptProgram(1);
    }// End of first part of the for instruction
    if (nameOfVariable != NULL) {
        free(nameOfVariable);
        nameOfVariable = NULL;
    }

    //  HERE, WE SAVED THE FIRST PART OF THE FOR INST BODY
    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_NO_COMMA_FOR_LOOP);
        return quitScriptProgram(1);
    }
    //2) Start taking middle part. (It should be the same variable as in the initialization  ..ie  for(i=0, i <--- this should be the same.. ie you should use i again).
    if (current_token_word == TOKEN_VARIABLE) {
        //TODO: Could be wrong. You must support having different Comparison. At the moment we must have the same variable used in the first part.
        if (script_variables[varPosition].hashVarnme != CalculateHashOfString(p_prog->txtString, strlen(p_prog->txtString))) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
            return quitScriptProgram(1);
        }
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
        return quitScriptProgram(1);
    }
    //Keep the position of the second parts variable. But at the moment must be the same as varIteration1
    forExtract[nestedForloop - 1].varIteration2 = varPosition;
    process_token(TOKEN_VARIABLE);

    // Find the logical operation now
    switch (current_token_word) {
    case TOKEN_EQUAL_EQUAL:
    case TOKEN_LESSTHAN:
    case TOKEN_LESSTHANEQUAL:
    case TOKEN_GREATERTHAN:
    case TOKEN_GREATERTHANEQUAL:
    case TOKEN_NOTEQUAL:
        forExtract[nestedForloop - 1].enumLogicalOperation = current_token_word;
        break;
    default: {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_NOT_SUPPORTED);
        return quitScriptProgram(1);
    };
    }

    process_token(forExtract[nestedForloop - 1].enumLogicalOperation);
    // We  need to get the value or the variable after the symbols. (This variable is not the same as in the initialization .. it is another "if used"
    //TODO: We don't support arithmetic operation inside the for-loop instruction to be implemented later.
    if (current_token_word == TOKEN_VARIABLE) { // So we have after the logical operation a variable ..ie   for(..., ...< VARIABLE, .....)
        tempVariable = strlen(p_prog->txtString);
        nameOfVariable = (char*)calloc(tempVariable + 1, sizeof(char));
        if (nameOfVariable == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FOREXTRACT);
            return quitScriptProgram(1);
        }

        //snprintf(nameOfVariable, tempVariable ,"%s", p_prog->txtString);
        strncpy(nameOfVariable, p_prog->txtString, tempVariable); //We don't need to add '\0' as we uses calloc

        if (CalculateHashOfString(p_prog->txtString, strlen(p_prog->txtString)) == script_variables[forExtract[nestedForloop - 1].varIteration1].hashVarnme) {
            // We have the same variable. This shouldn't happen. You cannot use the initializer variable in the Comparison.
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_USE_ANOTHER_VARIABLE);
            return quitScriptProgram(1);
        }
        process_token(TOKEN_VARIABLE);   // Here we are finished .. We have a variable inside the conditional section   for (...,..LogicaOperation variable)
//TODO: Make sure that the variable for for-while loop is int .. or it will crash!!!
        varPosition = TestSystem_get_variable(nameOfVariable);
        if (varPosition == -1) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FOR_LOOP_COMPARISION_BETWEEN_UNAVAILABLE_VAR);
            return quitScriptProgram(1);
        }
        //This is used as a logical part of the comparison to the right side ..ie <__here____
        forExtract[nestedForloop - 1].varIteration3 = varPosition;  //Save the position of the variable (ex for(i=myvar1, i<myvar2,...)
        forExtract[nestedForloop - 1].var3 = (int)*script_variables[varPosition].Value.pi; //Keep in mind you have to update this
    }
    else if (current_token_word == TOKEN_NUMBER || current_token_word == TOKEN_MINUS) {
        //give support to negative numbers.
        // We have only a value after the conditional operation.
        // This value will be used in the logical Comparison.
        if (current_token_word == TOKEN_MINUS) {
            process_token(TOKEN_MINUS);
            forExtract[nestedForloop - 1].var3 = -1 * convert_text_to_number();
        }
        else {
            forExtract[nestedForloop - 1].var3 = convert_text_to_number();
        }

        process_token(TOKEN_NUMBER);
    }
    if (nameOfVariable != NULL) {
        free(nameOfVariable);
        nameOfVariable = NULL;
    }

    // We have the logical operation parameters. Now we need to take the last part of the for
    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_NO_COMMA_FOR_LOOP);
        return quitScriptProgram(1);
    }
    // Get last part of for-loop instruction
    if (current_token_word == TOKEN_VARIABLE) {
        //We don't allow any other variable than the iterator
        if (CalculateHashOfString(p_prog->txtString, strlen(p_prog->txtString)) == script_variables[forExtract[nestedForloop - 1].varIteration1].hashVarnme) {
            process_token(TOKEN_VARIABLE);
            //We don't need to keep the value
            //forExtract[nestedForloop - 1].varIteration4 = forExtract[nestedForloop - 1].varIteration1;
        }
        else {// We must have the same variable. as for the iterator
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
            return quitScriptProgram(1);
        }
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
        return quitScriptProgram(1);
    }

    //3)  Here is the operation that will be done to the variable . In the last part of the for
        // To make it simple, we will support only ++, and --
    if (current_token_word == TOKEN_PLUSPLUS)
        forExtract[nestedForloop - 1].var4 = 1;             //This is for the PLUS PLUS
    else if (current_token_word == TOKEN_MINUSMINUS)
        forExtract[nestedForloop - 1].var4 = -1;                // This is for MINUS MINUS
    else {
        //
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FOR_LOOP_THIRDPART_MISSING);
        return quitScriptProgram(1);
    }

    process_token(current_token_word);
    if (current_token_word == TOKEN_RIGHT_PARENTHESIS) {
        process_token(TOKEN_RIGHT_PARENTHESIS);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_PARENTHESIS_MISSING);
        return quitScriptProgram(1);
    }
    while (END_OF_LINE)
        check_and_get_next_token();

    if (current_token_word == TOKEN_LEFT_CURLY_BRACKET) {
        process_token(TOKEN_LEFT_CURLY_BRACKET);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_LEFT_CURLY_BRACKET_NOT_FOUND);
        return quitScriptProgram(1);
    }

    forExtract[nestedForloop - 1].p_StartForLoop = p_prog->prev;     //Save location of the first line after the for-loop start instruction. This will be used to re-run the code
    forExtract[nestedForloop - 1].p_EndForLoop = p_prog->prev;
    SaveProg = p_prog->prev;          //One step back to point to the enum value
    while (current_token_word != TOKEN_EOF && current_token_word != TOKEN_RIGHT_CURLY_BRACKET) {
        if (current_token_word == TOKEN_LEFT_CURLY_BRACKET) {
            //We have nested for
            while (current_token_word != TOKEN_EOF && current_token_word != TOKEN_RIGHT_CURLY_BRACKET) {
                check_and_get_next_token();
            }

            if (current_token_word != TOKEN_RIGHT_CURLY_BRACKET) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_MISSING);
                return quitScriptProgram(1);
            }
        }
        check_and_get_next_token();
    }

    if (current_token_word != TOKEN_RIGHT_CURLY_BRACKET) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_MISSING);
        return quitScriptProgram(1);
    }
    p_prog = p_prog->prev; // Point to the enum value.
    forExtract[nestedForloop - 1].p_EndForLoop = p_prog->prev->prev; //Save the end of the for-loop block. avoid having the bracket.
    p_prog = SaveProg;
    //   token_jump_to(p_prog);
       /*
       We are ready to run the block of instructions. We know which code is the start of the for loop
       and we know also the last for statement.
       TODO: The script program support only two for-loop. Make it more general.
       */

    while (GetLogicalOperationAnswer(forExtract[nestedForloop - 1].enumLogicalOperation,
        *script_variables[forExtract[nestedForloop - 1].varIteration1].Value.pi,
        forExtract[nestedForloop - 1].var3) != 0) {
        token_jump_to(forExtract[nestedForloop - 1].p_StartForLoop);
        if (Execute_Instructions(forExtract[nestedForloop - 1].p_EndForLoop) == -1) { return quitScriptProgram(1); }
        if (forExtract[nestedForloop - 1].varIteration3 != -1) {
            if (script_variables[forExtract[nestedForloop - 1].varIteration3].tag != T_INT) {
                //Only integer allowed here
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FLOAT_INSIDE_LOOP);
            }
            forExtract[nestedForloop - 1].var3 = *script_variables[forExtract[nestedForloop - 1].varIteration3].Value.pi;
        }
        //We need to update the value
        if ((forExtract[nestedForloop - 1].var4) == 1)
            *(script_variables[forExtract[nestedForloop - 1].varIteration1].Value.pi) = *(script_variables[forExtract[nestedForloop - 1].varIteration1].Value.pi) + 1;
        else
            *(script_variables[forExtract[nestedForloop - 1].varIteration1].Value.pi) = *(script_variables[forExtract[nestedForloop - 1].varIteration1].Value.pi) - 1;
        //DEBUG_MESSAGE("RETURN AFTER FOR_LOOP_EXE EXECUTION loopnr=%d\n", nestedForloop);
    }//END FOR LOOP
    while (END_OF_LINE) {
        process_token(current_token_word);
    }

    if (current_token_word == TOKEN_RIGHT_CURLY_BRACKET) {
        //DEBUG_MESSAGE("FOUND TOKEN_RIGHT_CURLY_BRACKET FOR-LOOP NESTED %d\n", nestedForloop);
        check_and_get_next_token();
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_MISSING);
        return quitScriptProgram(1);
    }
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_FOR_LOOP_END);
    while (END_OF_LINE) {
        process_token(current_token_word);
    }
    //end of for-loop program  :: TODO :: I don't know if this code must be there .. Maybe when it is "two" for loops??

    nestedForloop--;
    nestedForWhileloop--;
    token_jump_to(p_prog->prev);
    if (nestedForloop >= 1 || nestedForWhileloop >= 1) {
        // We had nested for-loop. Don't return the the main loop. Return to the for loop calling this for loop.
        // Simply, we have to let this for loop acts like other function. They return the results only .. they don't jump to other functions.
        return 0;
    }
    else if (nestedForloop == 0) {
        return 0;// Execute_Instructions(NULL); //RUN Rest the code as normal.
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_NESTEDFORLOOP_NESTEDWHILELOOP_NOT_ZERO);
        return quitScriptProgram(1);
    }
    return 0;
}