/**
 ******************************************************************************
 * @file    if_else.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains if ..else instruction.
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
/** IF instruction
 * Syntax:
 *  if(logical operation) {block of code} else { block of code}
 */
int token_if()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS;

    process_token(TOKEN_IF);
    ifInstruction struct_tokenIf;
    instr_linkedList* pSaveProg = NULL;
    struct_tokenIf.pIfEnd = NULL;
    struct_tokenIf.pElseEnd = NULL;
    struct_tokenIf.value1 = struct_tokenIf.value2 = -1;
    struct_tokenIf.operation = TOKEN_BAD;
    int varPosition = -1;
    instr_linkedList* pScanProg = NULL;
    int blockOfCode = -1;

    if (current_token_word != TOKEN_LEFT_PARENTHESIS) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_LEFT_PARENTHESIS_NOT_FOUND);
        return quitScriptProgram(1);
    }
    process_token(TOKEN_LEFT_PARENTHESIS);

    if (current_token_word == TOKEN_NUMBER) {
        // It is a simple if with a number ..ex if(1)
        struct_tokenIf.value1 = struct_tokenIf.value2 = convert_text_to_number();
        struct_tokenIf.operation = TOKEN_EQUAL_EQUAL;
        process_token(TOKEN_NUMBER);
        if (current_token_word != TOKEN_RIGHT_PARENTHESIS) {
            // Syntax error. you cannot put a number inside if statement and has other things)
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_IF);
            return quitScriptProgram(1);
        }
    }
    else if (current_token_word == TOKEN_VARIABLE) {
        //First/SECOND item not the logical operation
        varPosition = TestSystem_get_variable(NULL);
        process_token(TOKEN_VARIABLE);
        if (varPosition == -1) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND_IF);
            return quitScriptProgram(1);
        }
        if (script_variables[varPosition].tag == T_INT)
            struct_tokenIf.value1 = (float)*script_variables[varPosition].Value.pi;
        else if (script_variables[varPosition].tag == T_FLOAT)
            struct_tokenIf.value1 = (float)*script_variables[varPosition].Value.pf;
        else {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_NUMBER);
            return quitScriptProgram(1);
        }
    }
    // Find the logical operation now
    switch (current_token_word) {
    case TOKEN_EQUAL_EQUAL:
    case TOKEN_NOTEQUAL:
    case TOKEN_LESSTHAN:
    case TOKEN_LESSTHANEQUAL:
    case TOKEN_GREATERTHAN:
    case TOKEN_GREATERTHANEQUAL:
        struct_tokenIf.operation = current_token_word;
        break;
    default: {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_NOT_SUPPORTED);
        return quitScriptProgram(1);
    };
    }
    process_token(struct_tokenIf.operation);
    // We  need to get the value or the variable after the symbols.
    //TODO: We don't support arithmetic operation inside the IF statement
    if (current_token_word == TOKEN_VARIABLE) {
        varPosition = TestSystem_get_variable(NULL);
        process_token(TOKEN_VARIABLE);
        if (varPosition == -1) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
            return quitScriptProgram(1);
        }
        if (script_variables[varPosition].tag == T_INT)
            struct_tokenIf.value2 = (float)*script_variables[varPosition].Value.pi;
        else if (script_variables[varPosition].tag == T_FLOAT)
            struct_tokenIf.value2 = (float)*script_variables[varPosition].Value.pf;
        else {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_NUMBER);
            return quitScriptProgram(1);
        }
    }
    else if (current_token_word == TOKEN_NUMBER) {
        //We have a number after the logical operation symbol
        struct_tokenIf.value2 = convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_IF_UNKNOWN);
        return quitScriptProgram(1);
    }
    if (current_token_word != TOKEN_RIGHT_PARENTHESIS) {
        // Syntax error. you cannot put a number inside if statement and has other things)
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_IF);
        return quitScriptProgram(1);
    }
    process_token(TOKEN_RIGHT_PARENTHESIS);
    //Skip new lines if there are any
    while (current_token_word == TOKEN_NEW_LINE)
        process_token(TOKEN_NEW_LINE);
    //We have everything after the if statement. We should now find the block and put start/end inside the if struct.
    if (current_token_word == TOKEN_LEFT_CURLY_BRACKET) {
        /*  We have a block of instructions after if find start and end.    */
        blockOfCode = 1;
        process_token(TOKEN_LEFT_CURLY_BRACKET);
        pSaveProg = p_prog->prev;
        while (current_token_word != TOKEN_RIGHT_CURLY_BRACKET && current_token_word != TOKEN_EOF) {
            check_and_get_next_token();
        }

        if (current_token_word == TOKEN_EOF) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_MISSING);
            return quitScriptProgram(1);
        }
        else if (current_token_word == TOKEN_RIGHT_CURLY_BRACKET) {   //Last position of the block.
            struct_tokenIf.pIfEnd = p_prog->prev->prev;
        }
        p_prog = pSaveProg;
        token_jump_to(p_prog);
    }
    else {
        //We have only one line instruction after the if..
        blockOfCode = -1;
        struct_tokenIf.pIfEnd = RetrieveEndOfInstruction();
    }

    //Here, we have start/end and everything else in the logical operation.
    if (GetLogicalOperationAnswer(struct_tokenIf.operation, struct_tokenIf.value1, struct_tokenIf.value2) == 1) {
        //script-If statement is correct run the block
        Execute_Instructions(struct_tokenIf.pIfEnd); // Run the block after the if statement.
        if (blockOfCode > 0)
            if (current_token_word == TOKEN_RIGHT_CURLY_BRACKET)
                process_token(TOKEN_RIGHT_CURLY_BRACKET);
        // Here if we find an "else" instruction we should take care of it or we go out from this instruction.
    }
    else {
        token_jump_to(struct_tokenIf.pIfEnd);     //We must jump to here .. This position should be after the bracket.
        if (current_token_word == TOKEN_RIGHT_CURLY_BRACKET)
            process_token(TOKEN_RIGHT_CURLY_BRACKET);           //This might be the case after if {...}

        //We should run here the 'else' part of the IF-Instruction.
        if (current_token_word == TOKEN_ELSE) {
            process_token(TOKEN_ELSE);
            //Now we must find the block of the instructions
            if (current_token_word != TOKEN_LEFT_CURLY_BRACKET) {
                //We have only one instruction after else
                blockOfCode = -1;
                pScanProg = RetrieveEndOfInstruction();
                struct_tokenIf.pElseEnd = pScanProg->prev;
            }
            else {
                //We have a block of code after else
                process_token(TOKEN_LEFT_CURLY_BRACKET);
                pScanProg = p_prog->prev;
                while (current_token_word != TOKEN_RIGHT_CURLY_BRACKET && current_token_word != TOKEN_EOF) {
                    check_and_get_next_token();
                }

                if (current_token_word == TOKEN_EOF) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_MISSING);
                    return quitScriptProgram(1);
                }
                else if (current_token_word == TOKEN_RIGHT_CURLY_BRACKET) {
                    struct_tokenIf.pElseEnd = p_prog->prev; //Last position of the block.
                    token_jump_to(pScanProg);
                }
            }

            //We have everything to run the code.
            Execute_Instructions(struct_tokenIf.pElseEnd); // Run the block after the else statement.
            if (blockOfCode > 0)
                if (current_token_word == TOKEN_RIGHT_CURLY_BRACKET)
                    process_token(TOKEN_RIGHT_CURLY_BRACKET);
        }
    }
    return 1;
}