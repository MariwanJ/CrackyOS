/**
 ******************************************************************************
 * @file    function.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains function instruction.
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
GlobalfunList* functionList = NULL;

/** Function definition
*Syntax:
*   function myfunction arg1,arg2,...etc { do stuff here }
*OR
*   function myfunction arg1,arg2,...etc {      //Notice that the bracket MUST be after the arguments without having new line
*           do stuff here
*               }
*Note:
*       Keep in mind that this function will only save the information about the function. It doesn't run the function
*       To run the function, you need to execute "CALL .....function name......"
*/

int token_function()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        int leftBracket = 0;
    int strLength;
    process_token(TOKEN_FUNCTION);
    //Create a new item
    if (functionList == NULL) {
        //No item in the list .. Create A new list.
        functionList = new GlobalfunList;
        if (functionList == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FUNCTIONLIST);
            return quitScriptProgram(1);
        }
        functionList->prev = NULL;
        functionList->next = NULL;
    }
    else {
        while (functionList->next != NULL)
            functionList = functionList->next;
        functionList->next = new GlobalfunList;
        if (functionList->next == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FUNCTIONLIST);
            return quitScriptProgram(1);
        }
        functionList->next->prev = functionList;
        functionList->next->next = NULL;
        functionList = functionList->next;
    }
    //Here we have a ready to use functionList
    if (current_token_word != TOKEN_VARIABLE) {
        //Since the function name is a text, the system will count it as a TOKEN_VARIABLE. If not found we have an error
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FUNCTION_NAME_NOT_FOUND);
        return quitScriptProgram(1);
    }
    functionList->name = NULL; //Initialize the name to avoid error
    functionList->start = NULL;
    functionList->end = NULL;

    strLength = strlen(p_prog->txtString);

    functionList->name = (char*)calloc(strLength + 1, sizeof(char));
    if (functionList->name == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FUNCTION_NAME);
        return quitScriptProgram(1);//Exit the program
    }
    //Keep function name saved
    strncpy(functionList->name, p_prog->txtString, strLength); //We don't need to add '\0' as we uses calloc

    process_token(TOKEN_VARIABLE);  // Really here it is not a variable, but since it is a text, check_and_get_next_token count it as variable.
    /*Now we have the first arg or the '{'.
       1)Save this position; it is the start of the function arg/body
       2)Find the end of the function
    */

    functionList->start = p_prog->prev;         //Save the start point of the function. It can be the beginning of the arguments or the body of the function if there aren't any arguments.

    while (current_token_word != TOKEN_LEFT_CURLY_BRACKET) {
        check_and_get_next_token();
    }
    if (current_token_word != TOKEN_LEFT_CURLY_BRACKET) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_LEFT_CURLY_BRACKET_NOT_FOUND);
        return quitScriptProgram(1);//Exit the program
    }
    process_token(TOKEN_LEFT_CURLY_BRACKET);        //SKIP the bracket
    leftBracket++;
    //Now we have the first bracket(left) and we should continue searching for the last/right bracket
    while (current_token_word != TOKEN_EOF) {
        if (current_token_word == TOKEN_LEFT_CURLY_BRACKET) {
            leftBracket++; //second, third ..etc
        }
        else {
            if (current_token_word == TOKEN_RIGHT_CURLY_BRACKET) {
                if (leftBracket == 1)
                    break;               //We found the body of the function .. go out from the while
                else
                    leftBracket--;
            }
        }
        check_and_get_next_token();
    }

    if (end_of_program_token() || current_token_word != TOKEN_RIGHT_CURLY_BRACKET) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_LEFT_CURLY_BRACKET_NOT_FOUND);
        return quitScriptProgram(1);//Exit the program
    }

    //We must have the right bracket now ..
    functionList->end = p_prog->prev; //Keep in mind that 'end' is pointing to the end of right bracket.
    //We are done, make a jump to the new instruction and fetch next inst. It is enough to go one link next as it is on the text part already
    token_jump_to(p_prog->next); //This will fetch the next instruction after the function body.
    return 1;
}

/** CALL of a function (arg1, arg2 ..argn)
*Syntax:
*   call function_name (arg1, arg2 ..argn);
*OR
*   call function_name ("STRING ", "STRING".. ,arg1..etc)
*OR
*   call function_name  ()//No arguments
*Note:
*       1)Keep in mind that you must define the function in the top side of the script program before the executable codes.
*         And the arguments are byValue .. Values will be transfered to the function ..not variables.
*       2)No global variables are reachable here.
*/

int token_call(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS
        /*
         * This code works like this
         * 1-backup global variables.
         * 2-create new global variables for the function
         * 3-
         *
         * */
        float element_value = 0;
    int OneElementVarPos = -1;
    process_token(TOKEN_CALL);
    int argv = 0;
    float result = 0;
    unsigned char parenthesis = 0;
    int varPosition = -1;
    GlobalVariables* arguments = new GlobalVariables[SCRIPT_MAX_VARIABLE_NUMBERS];      //new global variables.
    if (arguments == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
        return quitScriptProgram(1);
    }

    instr_linkedList* p_prog_backup = NULL;
    instr_linkedList* endofLine = NULL;
    char* buf_FunCalledName = NULL;
    GlobalfunList* pScanFunList = functionList;
    if (current_token_word != TOKEN_VARIABLE) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FUNCTION_NAME_NOT_FOUND);
        return quitScriptProgram(1);//Exit the program
    }
    // Initialize the new global variables
    for (int i = 0; i < SCRIPT_MAX_VARIABLE_NUMBERS; i++) {
        arguments[i].varname = NULL;
        arguments[i].tag = T_NULL;
        arguments[i].Value.pi = NULL;
        arguments[i].Value.pf = NULL;
        arguments[i].Value.txtvalue = NULL;
        arguments[i].sizeOfArray = 0;
        arguments[i].hashVarnme = 0;
    }

    int strLength = strlen(p_prog->txtString);

    buf_FunCalledName = (char*)calloc(strLength + 1, sizeof(char));    //Variable to keep the function name.
    if (buf_FunCalledName == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FUNCTION_NAME);
        return quitScriptProgram(1);//Exit the program
    }
    //Keep function name saved
    strncpy(buf_FunCalledName, p_prog->txtString, strLength); //We don't need to add '\0' as we uses calloc

    process_token(TOKEN_VARIABLE);

    //We have the name of the function .. Find it and run the code.
    if (pScanFunList != NULL) {
        while (pScanFunList->prev != NULL) {
            pScanFunList = pScanFunList->prev;
        }
        while (strcmp(pScanFunList->name, buf_FunCalledName) != 0) {
            if (pScanFunList->next == NULL) {
                //Name of the function couldn't be found inside the list.Go out.
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FUNCTION_NAME_NOT_FOUND);
                return quitScriptProgram(1);//Exit the program
            }
            pScanFunList = pScanFunList->next;
        }
    }
    else {
        //Function list was empty . Go out
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FUNCTION_NAME);
        return quitScriptProgram(1);//Exit the program
    }
    // We could find the function name. Start to execute the function.
    /*          Arguments and Variables in this function
        Since we always declare function as global variables, this could make
        a huge problem for us here. In this part if we don't change the name
        of the arguments variables.     */

    if(buf_FunCalledName!=NULL)
    	free(buf_FunCalledName);
    buf_FunCalledName = NULL;

    GlobalVariables* script_variables_backup = script_variables;  //Save the global variable here.

    unsigned char nestedForloop_backup = nestedForloop;
    nestedForloop = 0;
    /** This variable will count the no. of while-loop instructions inside a block*/
    unsigned char nestedWhileloop_backup = nestedWhileloop;
    nestedWhileloop = 0;
    /** This will be used when we have for & while loops used inside each other. */
    unsigned char nestedForWhileloop_backup = nestedForWhileloop;
    nestedForWhileloop = 0;

    /**FOR -LOOP INSTRUCTION WILL BE ACCEPTED ONLY IF THEY ARE '=SCRIPT_MAX_NESTED_FOR_WHILE_LOOP' NESTED FOR-LOOP. NO MORE. FOR SIMPLICITY*/
    token_for_object* forExtract_backup = forExtract;
    forExtract = NULL;
    forExtract = new token_for_object[SCRIPT_MAX_NESTED_FOR_WHILE_LOOP];
    if (forExtract == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FOREXTRACT);
        return quitScriptProgram(1);
    }

    /**WHILE -LOOP INSTRUCTION WILL BE ACCEPTED ONLY IF THEY ARE '=SCRIPT_MAX_NESTED_FOR_WHILE_LOOP' NESTED FOR-LOOP. NO MORE. FOR SIMPLICITY*/
    token_while_object* whileExtract_backup = whileExtract;
    whileExtract = NULL;
    whileExtract = new token_while_object[SCRIPT_MAX_NESTED_FOR_WHILE_LOOP];
    if (whileExtract == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_WHILEEXTRACT);
        return quitScriptProgram(1);
    }
    for (int j = 0; j < SCRIPT_MAX_NESTED_FOR_WHILE_LOOP; j++) {
        forExtract[j].p_StartForLoop = NULL;
        forExtract[j].p_EndForLoop = NULL;
        whileExtract[j].p_StartWhileLoop = NULL;
        whileExtract[j].p_EndWhileLoop = NULL;
        whileExtract[j].varIteration = 0;
        whileExtract[j].varLogicalRelation.variable = NULL;
    }

    if (current_token_word != TOKEN_LEFT_PARENTHESIS) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_LEFT_PARENTHESIS);
        return quitScriptProgram(1);
    }
    process_token(TOKEN_LEFT_PARENTHESIS);
    parenthesis++;
    //Move the pointer to the end of the line. This is the call function body .. Not the function itself. don't mix
    p_prog_backup = RetrieveEndOfInstruction();

    //Find the arguments.
    while (p_prog->index < p_prog_backup->index) {
        // Start taking the arguments if any
        switch (current_token_word) {
        case TOKEN_LEFT_PARENTHESIS: {
            process_token(TOKEN_LEFT_PARENTHESIS);
            parenthesis++;
        }  break;
        case TOKEN_RIGHT_PARENTHESIS: {
            process_token(TOKEN_RIGHT_PARENTHESIS);
            parenthesis--;
        } break;

        case TOKEN_STRING: { // Pure string sent to the function without variable
            strLength = strlen(p_prog->txtString) - 2;
            arguments[argv].sizeOfArray = strLength; // since we have "" we should take them out of the array.
            arguments[argv].tag = T_STRING;
            arguments[argv].Value.txtvalue = (char*)calloc(strLength + 2, sizeof(char));
            if (arguments[argv].Value.txtvalue == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                return quitScriptProgram(1);
            }

            strncpy(arguments[argv].Value.txtvalue, (p_prog->txtString + 1), strLength); //We don't need to add '\0' as we uses calloc

            arguments[argv].varname = new char[10];
            if (arguments[argv].varname == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                return quitScriptProgram(1);
            }
            //TODO Check this .. how big should be the varname?? if argv is bigger than 1 digit .. this will fail.2020-02-27
            snprintf(arguments[argv].varname, 6, "%s%d", "temp", argv);
            arguments[argv].hashVarnme = 0; //Not necessary to give a name as it is only a text not variable name.
            process_token(TOKEN_STRING);
            argv++;
        } break;
        case TOKEN_NUMBER: {
            result = convert_text_to_number();
            arguments[argv].sizeOfArray = 1;
            if (ceil(result) == result) {
                //we have an INT
                arguments[argv].tag = T_INT;
                arguments[argv].Value.pi = new int[1];
                if (arguments[argv].Value.pi == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                    return quitScriptProgram(1);
                }
                *arguments[argv].Value.pi = (int)result;
            }
            else {
                //we have an INT
                arguments[argv].tag = T_FLOAT;
                arguments[argv].Value.pf = new float[1];
                if (arguments[argv].Value.pf == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                    return quitScriptProgram(1);
                }
                *arguments[argv].Value.pf = result;
            }
            process_token(TOKEN_NUMBER);
            argv++;
        } break;
        case TOKEN_COMMA: {
            process_token(TOKEN_COMMA);
        }break;
        case TOKEN_VARIABLE: { // Variable sent to the function i.e  arra[myvar]
            varPosition = TestSystem_get_variable(NULL);
            process_token(TOKEN_VARIABLE);
            if (varPosition == -1) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARGUMENT_VAR_NOT_FOUND);
                return quitScriptProgram(1);
            }
            if (current_token_word == TOKEN_LEFT_SQUARE_BRACKET) {
                //We have an element of an array, not all the array. Try to find out which element of the array we send to the function.
                process_token(TOKEN_LEFT_SQUARE_BRACKET);
                if (current_token_word == TOKEN_VARIABLE) {
                    //Inside the array we have again a variable
                    OneElementVarPos = TestSystem_get_variable(NULL);
                    process_token(TOKEN_VARIABLE);
                    if (OneElementVarPos == -1) {
                        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARGUMENT_VAR_NOT_FOUND);
                        return quitScriptProgram(1);
                    }
                    int arrayIndex = *script_variables[OneElementVarPos].Value.pi;
                    if (arrayIndex < 0 || arrayIndex >= script_variables[varPosition].sizeOfArray) {
                        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_INVALID_ARRAY_INDEX);
                        return quitScriptProgram(1);
                    }
                    //  Here you must check if the variable is int, char or float
                    if (script_variables[OneElementVarPos].tag == T_INT) {
                        arguments[argv].sizeOfArray = 1;
                        switch (script_variables[varPosition].tag) {
                        case T_INT: {
                            arguments[argv].tag = T_INT;
                            arguments[argv].Value.pi = new int[1];
                            if (arguments[argv].Value.pi == NULL) {
                                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                                return quitScriptProgram(1);
                            }
                            *arguments[argv].Value.pi = script_variables[varPosition].Value.pi[arrayIndex];
                        }
                        break;
                        case T_FLOAT: {
                            arguments[argv].tag = T_FLOAT;
                            arguments[argv].Value.pf = new float[1];
                            if (arguments[argv].Value.pf == NULL) {
                                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                                return quitScriptProgram(1);
                            }
                            *arguments[argv].Value.pf = script_variables[varPosition].Value.pf[arrayIndex];
                        }
                                    break;

                        case T_STRING: {
                            arguments[argv].tag = T_STRING;
                            arguments[argv].Value.txtvalue = new char[2];
                            if (arguments[argv].Value.txtvalue == NULL) {
                                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                                return quitScriptProgram(1);
                            }
                            arguments[argv].Value.txtvalue[0] = script_variables[varPosition].Value.txtvalue[arrayIndex];
                            arguments[argv].Value.txtvalue[1] = '\0';
                        }
                        break;
                        default: {
                            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_INITIALIZED);
                            return quitScriptProgram(1);
                        }
                        }//Switch end.
                    }
                    else {
                        // You cannot have a float inside [] of an array.
                        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FLOAT_USED_AS_INDEX_FOR_ARRAY);
                        return quitScriptProgram(1);
                    }
                }
                else if (current_token_word == TOKEN_NUMBER) {
                    //Inside the array brackets there was a number
                    element_value = convert_text_to_number();
                    process_token(TOKEN_NUMBER);
                    if (ceil(element_value) != element_value) {
                        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FLOAT_USED_AS_INDEX_FOR_ARRAY);
                        return quitScriptProgram(1);
                    }
                }
                else {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_INT_OR_FLOAT_NOT_FOUND);
                    return quitScriptProgram(1);
                }

                if (current_token_word == TOKEN_RIGHT_SQUARE_BRACKET) {
                    process_token(TOKEN_RIGHT_SQUARE_BRACKET);
                }
            }
            else {
                //In this part, user send an variable. But it could be an array-variable(not an element of the array)
                arguments[argv].sizeOfArray = script_variables[varPosition].sizeOfArray;
                switch (script_variables[varPosition].tag) {
                case T_INT: {
                    arguments[argv].tag = T_INT;
                    arguments[argv].Value.pi = new int[script_variables[varPosition].sizeOfArray];
                    if (arguments[argv].Value.pi == NULL) {
                        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                        return quitScriptProgram(1);
                    }
                    memcpy(arguments[argv].Value.pi, script_variables[varPosition].Value.pi, script_variables[varPosition].sizeOfArray * (sizeof(int)));
                    break;
                }
                case T_FLOAT: {
                    arguments[argv].tag = T_FLOAT;
                    arguments[argv].Value.pf = new float[script_variables[varPosition].sizeOfArray];
                    if (arguments[argv].Value.pf == NULL) {
                        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                        return quitScriptProgram(1);
                    }
                    memcpy(arguments[argv].Value.pf, script_variables[varPosition].Value.pf, script_variables[varPosition].sizeOfArray * (sizeof(float)));
                    break;
                }
                case T_STRING: {
                    arguments[argv].tag = T_STRING;
                    arguments[argv].Value.txtvalue = new char[script_variables[varPosition].sizeOfArray + 1];
                    if (arguments[argv].Value.txtvalue == NULL) {
                        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                        return quitScriptProgram(1);
                    }
                    memcpy(arguments[argv].Value.txtvalue, script_variables[varPosition].Value.txtvalue, script_variables[varPosition].sizeOfArray * (sizeof(char)));
                    break;
                }
                default: {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_INITIALIZED);
                    return quitScriptProgram(1);
                }
                }
            }
            argv++;
            break;
        } //END TOKEN_VARIABLE // Variable sent to the function i.e  arra[myvar]
        } //END SWITCH
    } //END WHILE
    if (parenthesis != 0) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_LEFT_PARENTHESIS_MISSING);
        return quitScriptProgram(1);
    }

    /*We have all arguments .. start running the function.
    Match the arguments we picked from calling the function to the function's body.*/

    //argv--; //This is the last item
    token_jump_to(pScanFunList->start);
    /**
     *  RUNNING THE CODE  START HERE
     * */
     //Find out the end of the called-function body arguments. This is not the caller it is the function itself, don't mix.
    endofLine = RetrieveEndOfInstruction();
    if (current_token_word == TOKEN_EOF) {
        return quitScriptProgram(1);//Exit the program; // nothing to do
    }
    while (END_OF_LINE) {
        check_and_get_next_token();  //Skip new lines.
    }
    if (current_token_word != TOKEN_LEFT_PARENTHESIS) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_FUNCTION_DEF);
        return quitScriptProgram(1);//Exit the program;
    }

    process_token(TOKEN_LEFT_PARENTHESIS);
    argv = 0;
    if (current_token_word == TOKEN_RIGHT_PARENTHESIS) {
        //No arguments
        process_token(TOKEN_RIGHT_PARENTHESIS);
    }
    else { //We have arguments - endofLine should points to the newline.
        while (p_prog->index < endofLine->index && current_token_word != TOKEN_RIGHT_PARENTHESIS) {
            if (current_token_word == TOKEN_COMMA) {
                process_token(TOKEN_COMMA);
            }
            else if (current_token_word == TOKEN_VARIABLE) {
                strLength = strlen(p_prog->txtString);
                arguments[argv].varname = NULL;
                arguments[argv].varname = (char*)calloc(strLength + 1, sizeof(char));
                if (arguments[argv].varname == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FUNCTION_CALL);
                    return quitScriptProgram(1);//Exit the program
                }
                //Keep argument name
                strncpy(arguments[argv].varname, p_prog->txtString, strLength); //We don't need to add '\0' as we uses calloc
                arguments[argv].hashVarnme = CalculateHashOfString(arguments[argv].varname, strlen(arguments[argv].varname));
                argv++;
                process_token(TOKEN_VARIABLE);
            }
        }
        if (current_token_word == TOKEN_RIGHT_PARENTHESIS)
            process_token(TOKEN_RIGHT_PARENTHESIS);
    }
    argv--;
    script_variables = arguments;
    arguments = NULL;
    if (current_token_word == TOKEN_LEFT_CURLY_BRACKET)
        process_token(TOKEN_LEFT_CURLY_BRACKET);
    Execute_Instructions(pScanFunList->end);          //We shouldn't include the '}'
    //After coming back from the function, delete arguments script_variables, retrieve the backup and we are done.
  //  IntializeScriptVariable(NULL); //Delete all.
    if (script_variables != NULL) {
    	delete[](script_variables);
       script_variables = NULL;
     }

    script_variables = script_variables_backup;
    script_variables_backup = NULL;
    p_prog = p_prog_backup;

    //Remove all allocated mem one by one
    free_forExtract();
    free_whileExtract();

    nestedForloop = nestedForloop_backup;
    nestedWhileloop = nestedWhileloop_backup;
    nestedForWhileloop = nestedForWhileloop_backup;

    forExtract = forExtract_backup;
    whileExtract = whileExtract_backup;

    token_jump_to(p_prog);
    return 1;
}
