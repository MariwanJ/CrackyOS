/**
 ******************************************************************************
 * @file    arithmetic.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains the arithmetic operations functions and methods
 *           used by the interpreter
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

#include "arithmetic.h"
#include "variable.h"
static int noOfParenthesis = 0;

/**
*This function will be used to calculate different arithmetic equations.
*Use this function when you need to calculate (sin, cos, tan, ..etc)
*@param functionType    INT value represent the enum of the function
*@param InnerValue      A float value represent the degree value
*/
float CalculatSpecialFunction(int functionType, float InnerValue)
{
    //Notice that we accept only degree angles.
    const float gradeToDeg = 0.0174532925199433f;
    const float radToDegree = 57.2957795130823f;
    float results = 0.0;

    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        switch (functionType) {
        case  TOKEN_SIN: {
            results = sin(gradeToDeg * InnerValue);
            break;
        }
        case  TOKEN_COS: {
            results = cos(gradeToDeg * InnerValue);
            break;
        }
        case  TOKEN_TAN: {
            results = tan(gradeToDeg * InnerValue);
            break;
        }
        case  TOKEN_ASIN: {
            results = asin(InnerValue) * radToDegree;
            break;
        }
        case  TOKEN_ACOS: {
            results = acos(InnerValue) * radToDegree;
            break;
        }
        case  TOKEN_ATAN: {
            results = atan(InnerValue) * radToDegree;
            break;
        }
        case  TOKEN_ABS: {
            results = fabs(InnerValue);
            break;
        }
        case  TOKEN_EXP: {
            results = exp(InnerValue);
            break;
        }
        case  TOKEN_LOG: {
            results = log10(InnerValue);
            break;
        }
        case  TOKEN_LN: {
            results = log(InnerValue);
            break;
        }
        case  TOKEN_ROUND: {
            results = floor(InnerValue);    //Only round up supported. TODO : Do we need round down? ceil?
            break;
        }
        case  TOKEN_SQRT: {
            results = sqrt(InnerValue);
            break;
        }
        }
    return results;
}

/**
*This function will be used to do a comparison between two values and an operation.
*Use this function when you need to compare two values (grater, less, equal ..etc).
*@param Oldvalue        Value to be compared
*@param ComparisonValue Value will be compared to
*@param enumOperation   comparison type ( >, <, =<, =>, !=, ==)
*/

unsigned char GetLogicalOperationAnswer(int enumOperation, int Oldvalue, int ComparisionValue)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_NULL

        DEBUG_MESSAGE("INFO %i\n", DBG_MSG_COMPARISON_LOGIC_OPERATION_STARTED);
    switch (enumOperation) {
    case TOKEN_EQUAL_EQUAL: {
        return (Oldvalue == ComparisionValue);
    }
    case TOKEN_LESSTHAN: {
        return (Oldvalue < ComparisionValue);
    }
    case TOKEN_LESSTHANEQUAL: {
        return (Oldvalue <= ComparisionValue);
    }
    case TOKEN_GREATERTHAN: {
        return (Oldvalue > ComparisionValue);
    }
    case TOKEN_GREATERTHANEQUAL: {
        return (Oldvalue >= ComparisionValue);
    }
    case TOKEN_NOTEQUAL: {
        return(Oldvalue != ComparisionValue);
    }
    default: {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_LOGICAL_OPERATION_FOR_LOOP);
        quitScriptProgram(1);
    }
    }
    return 0;
}

/**
*This function will be used to do logical operation on two values (OR, AND, NOT, INVERT)
*@param val1        Value to be compared
*@param val2        Value will be compared to
*@param operation   Digital operation supported (OR, AND, NOT, INVERT, SHIFT LEFT & SHIFT RIGHT)
*/

void CalculateLogicalOperation(int& val1, int val2, int operation)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION

        switch (operation) {
        case TOKEN_AND: {
            val1 = val1 & val2;
            break;
        }
        case TOKEN_OR: {
            val1 = val1 | val2;
            break;
        }
        case TOKEN_INVERT: {
            /* Since for Invert it is only one variable, we will ignore val2.
               Send X to the function ..it will be ignored */
            val1 = ~val1;
            break;
        }
        case TOKEN_SHIFT_LEFT: {
            // The val2 is the no of bits to shift right/left depending on operation
            val1 = val1 << val2;
            break;
        }
        case TOKEN_SHIFT_RIGHT: {
            // The val2 is the no of bits to shift right/left depending on operation
            val1 = val1 >> val2;
            break;
        }
        }
}
/**
*Raise to power. This function will calculate x raised to the power y
*@param x       The base
*@param y       The exponent
*/
float CalculatePower(float x, float y)
{
    errno = 0;
    float results = 0.0;
    results = pow(x, y);
    if (errno == EDOM || errno == ERANGE || errno == EILSEQ) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_OVERFLOW);
        quitScriptProgram(1);
        return -1;
    }
    return results;
}

/**
*This function will determine if we have an logical arethmatic operation
*@param  m_Tokenword current token word
*@return results .. if we have any of them the result will be 1
*@date
*/
unsigned char CheckforLogicalArithmeticOperations(int m_Tokenword) {
    switch (m_Tokenword) {
    case TOKEN_ASTR:
    case TOKEN_SLASH:
    case TOKEN_CARET:
    case TOKEN_MOD:
    case TOKEN_AND:
    case TOKEN_OR:
    case TOKEN_INVERT:
    case TOKEN_SIN:
    case TOKEN_COS:
    case TOKEN_TAN:
    case TOKEN_ASIN:
    case TOKEN_ACOS:
    case TOKEN_ATAN:
    case TOKEN_ABS:
    case TOKEN_EXP:
    case TOKEN_LOG:
    case TOKEN_LN:
    case TOKEN_ROUND:
    case TOKEN_SQRT:
    case TOKEN_SHIFT_LEFT:
    case TOKEN_SHIFT_RIGHT:

    case TOKEN_LESSTHAN:
    case TOKEN_LESSTHANEQUAL:
    case TOKEN_GREATERTHAN:
    case TOKEN_GREATERTHANEQUAL:
    case TOKEN_NOTEQUAL:
        return 1;
        break;
    default:return 0;
    }
}

/**
*A function to calculate division or multiplication in the script program.
*Only PLUS/MINUS/MULTIPLICATION/DIVISION  and some logical operation are Implemented.
*@return answer for the division or multiplication section of the script program
*/

float divide_multiply(void)
{
    float value = 0;              // Somewhere to accumulate the result
    int  intValue = 0;                //Int version of value
    int tokenValue = 0;
    value = get_next_number_or_variable_value();    // Get the first get_next_number_or_variable_value in the divide_multiply
    float tempValue = 0.0;

    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        // Loop as long as we have a multiplication or division.
        while (CheckforLogicalArithmeticOperations(current_token_word)) {
            switch (current_token_word) {
            case TOKEN_ASTR: {
                process_token(TOKEN_ASTR);
                tempValue = get_next_number_or_variable_value();
                if (current_token_word == TOKEN_CARET) {
                    process_token(TOKEN_CARET);
                    tempValue = CalculatePower(tempValue, get_next_number_or_variable_value());
                }
                value = value * tempValue;
                //process_token(TOKEN_NUMBER);      // we do that inside the get_next_number_or_variable_value() function
                break;
            }
            case TOKEN_SLASH: {
                process_token(TOKEN_SLASH);
                tempValue = get_next_number_or_variable_value();
                if (current_token_word == TOKEN_CARET) {
                    process_token(TOKEN_CARET);
                    tempValue = CalculatePower(tempValue, get_next_number_or_variable_value());
                }
                value = value / tempValue;
                //process_token(TOKEN_NUMBER);
                break;
            }
            case TOKEN_MOD: {
                //Syntax    value1 MOD value 2
                process_token(TOKEN_MOD);
                tempValue = get_next_number_or_variable_value();
                if (current_token_word == TOKEN_CARET) {
                    process_token(TOKEN_CARET);
                    tempValue = fmod(tempValue, get_next_number_or_variable_value());
                }
                value = fmod(value, tempValue);
                //process_token(TOKEN_NUMBER);
                break;
            }
            case TOKEN_CARET: {
                process_token(TOKEN_CARET);
                value = CalculatePower(value, get_next_number_or_variable_value());
                break;
            }
                            /* Here we will start supporting logical operations*/
            case TOKEN_AND: {
                process_token(TOKEN_AND);
                intValue = (int)value;
                CalculateLogicalOperation(intValue, (int)get_next_number_or_variable_value(), TOKEN_AND);
                value = intValue;
                break;
            }
            case TOKEN_SHIFT_LEFT: {
                process_token(TOKEN_SHIFT_LEFT);
                intValue = (int)value;
                CalculateLogicalOperation(intValue, (int)get_next_number_or_variable_value(), TOKEN_SHIFT_LEFT);
                value = intValue;
                break;
            }
            case TOKEN_SHIFT_RIGHT: {
                process_token(TOKEN_SHIFT_RIGHT);
                intValue = (int)value;
                CalculateLogicalOperation(intValue, (int)get_next_number_or_variable_value(), TOKEN_SHIFT_RIGHT);
                value = intValue;
                break;
            }
            case TOKEN_OR: {
                process_token(TOKEN_OR);
                intValue = (int)value;
                CalculateLogicalOperation(intValue, (int)get_next_number_or_variable_value(), TOKEN_OR);
                value = intValue;
                break;
            }
            case TOKEN_INVERT: {
                process_token(TOKEN_INVERT);
                intValue = (int)get_next_number_or_variable_value();
                // We have only one value.
                CalculateLogicalOperation(intValue, 0, TOKEN_INVERT);
                value = intValue;
                break;
            }
            case TOKEN_LESSTHAN:
            case TOKEN_LESSTHANEQUAL:
            case TOKEN_GREATERTHAN:
            case TOKEN_GREATERTHANEQUAL:
            case TOKEN_NOTEQUAL: {
                int tempTokenword = current_token_word;
                process_token(current_token_word);
                int temI = (int)get_next_number_or_variable_value();
                value = (int)GetLogicalOperationAnswer(tempTokenword, (int)value, temI);
                break;
            }

            case TOKEN_SIZE_OF_VAR: {
                //We need to get the variable name and return the sizeof array.
                process_token(TOKEN_SIZE_OF_VAR);
                if (current_token_word != TOKEN_LEFT_PARENTHESIS) {
                    //SYNTAX ERROR AS SIZEOF(VARIABLENAME) SHOULD BE USED.
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SIZE_OF_VAR);
                    quitScriptProgram(1);
                    return -1;
                }
                process_token(TOKEN_LEFT_PARENTHESIS);
                if (current_token_word != TOKEN_VARIABLE) {
                    //SYNTAX ERROR AS SIZEOF(VARIABLENAME) SHOULD BE USED.
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SIZE_OF_VAR);
                    quitScriptProgram(1);
                }
                int varPosition = TestSystem_get_variable(NULL);
                if (varPosition == -1) {
                    //SYNTAX ERROR AS SIZEOF(VARIABLENAME) SHOULD BE USED.
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SIZE_OF_VAR);
                    quitScriptProgram(1);
                }
                value = script_variables[varPosition].sizeOfArray;
                process_token(TOKEN_VARIABLE);
                if (current_token_word != TOKEN_RIGHT_PARENTHESIS) {
                    //SYNTAX ERROR AS SIZEOF(VARIABLENAME) SHOULD BE USED.
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SIZE_OF_VAR);
                    quitScriptProgram(1);
                    return -1;
                }
                process_token(TOKEN_RIGHT_PARENTHESIS);
            }break;

                //ALL Special functions must be here.
            case  TOKEN_SIN:
            case  TOKEN_COS:
            case  TOKEN_TAN:
            case  TOKEN_ASIN:
            case  TOKEN_ACOS:
            case  TOKEN_ATAN:
            case  TOKEN_ABS:
            case  TOKEN_EXP:
            case  TOKEN_LOG:
            case  TOKEN_LN:
            case  TOKEN_ROUND:
            case  TOKEN_SQRT: {
                tokenValue = current_token_word;
                process_token(current_token_word);
                value = CalculatSpecialFunction(tokenValue, get_next_number_or_variable_value());
                break;
            }
            }
        }
    //You don't need to use process_token(TOKEN_NUMBER) as inside get_next_number_or_variable_value() we do that
    return value;
}

/**
* A function that returns the next found variable/value inside the script.
* This function is used by the interpreter for calculating arithmetic expression
*@return next number/variable inside the script program
*/
float get_next_number_or_variable_value(void)
{
    STOP_CODE_EXECUTION_MINUS
        if (current_token_word == TOKEN_LEFT_PARENTHESIS) {
            return get_next_number_or_variable_value_Parenthesis();
        }// Return substring value
    if (current_token_word == TOKEN_NUMBER) { return get_next_number_or_variable_value_Number(); }// Not a digit when we get to here
    //Stop running if we have error
    switch (current_token_word) {      //ALL Special functions must be here.
    case  TOKEN_SIN:
    case  TOKEN_COS:
    case  TOKEN_TAN:
    case  TOKEN_ASIN:
    case  TOKEN_ACOS:
    case  TOKEN_ATAN:
    case  TOKEN_ABS:
    case  TOKEN_EXP:
    case  TOKEN_LOG:
    case  TOKEN_LN:
    case  TOKEN_ROUND:
    case  TOKEN_SQRT: {
        process_token(current_token_word);
        return (CalculatSpecialFunction(current_token_word, get_next_number_or_variable_value()));
    }
    }
    // if we have sizeof instruction.
    if (current_token_word == TOKEN_SIZE_OF_VAR) { return get_next_number_or_variable_value_SizeOfVar(); }
    if (current_token_word == TOKEN_VARIABLE) {
        return get_next_number_or_variable_value_Variable();       //return value .. zero if the variable not found.
    }
    return 0;
}
/**
*This function evaluates any arithmetic expression
*@param p_start   a pointer to the BEGIN place of the expression in the script program
*@param p_end     a pointer to the END place of the expression in the script program
*@return the result of the expression executed
*/
float arithmetic_expression(instr_linkedList* p_start, instr_linkedList* p_end)
{
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_ARITHMETIC_EXP_EXECUTED);
    float value = 0;          // Store result here
    token_jump_to(p_start);
    value = divide_multiply();  // Get first divide_multiply

    while (p_prog->index < p_end->index) {                   // Infinite loop, all exits inside
        //Stop running if we have error
        STOP_CODE_EXECUTION_MINUS

            switch (current_token_word) {
            case TOKEN_PLUS: {
                process_token(TOKEN_PLUS);
                value = value + divide_multiply();
            }
                           break;
            case TOKEN_MINUS: {
                process_token(TOKEN_MINUS);
                value = value - divide_multiply();
                break;
            }
            case TOKEN_RIGHT_PARENTHESIS:
                process_token(TOKEN_RIGHT_PARENTHESIS);
                break;
            case TOKEN_RIGHT_CURLY_BRACKET:
                process_token(TOKEN_RIGHT_CURLY_BRACKET);//It is important to notice that This will be treated here
                break;
            default:
                //DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_SIDE_OF_VAR_EQUATION);
                //We shouldn't be here .. Don't know when?
                DEBUG_MESSAGE("NO + -  Current_Token VAR=%i", current_token_word);
                process_token(current_token_word);
            }
    }
    return value;
}

/**
*This function find out BEGIN/END positions of the arithmetic operation inside the script program.
*The function is used by the interpreter.
*@return the result of the arithmetic operations done in the right side of the equal sign.
*/

float CalculateRightSideofVariableEquation(void)
{
    //This function should take care of Right } .. No where else shoudl this appear
    instr_linkedList* p_start = NULL;
    instr_linkedList* p_end = NULL;

    p_start = p_prog->prev;
    p_end = NULL;

    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        //Find the end of the line.
        p_end = RetrieveEndOfInstruction();
    p_end = p_end->prev->prev;
    p_prog = p_start;
    token_jump_to(p_prog);
    // Find the end of the line.
    DEBUG_MESSAGE(p_start->next->txtString);
    DEBUG_MESSAGE(p_end->next->txtString);
    float res = arithmetic_expression(p_start, p_end);
    return res;
}
/**
 * Determine if the right side of the array is string.
 * */
unsigned char CheckIfTheRightSideIsVarOfArrayString(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_NULL

        int varPosition = TestSystem_get_variable(NULL);// get the variable position.
    if (varPosition == -1) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
        quitScriptProgram(1);
    }
    return ((unsigned char)(script_variables[varPosition].tag == T_STRING));
}

/**
*This function will make all string operation needed after the equal sign,
*Warning: Mixing string and numeric operation is not allowed
*@param  varPosition INT value which is the position of the global script variable
*/
void CalculateRightSideofVariableEquation_TEXT(int varPosition) {
    //Stop running if we have error
    STOP_CODE_EXECUTION

        /***/
        instr_linkedList* endOFline = RetrieveEndOfInstruction();
    //endOfLine is pointing now to either NewLine, or Semicolon, Or EOF Go back one step
    endOFline = endOFline->prev->prev;                 //Twice since we have ext and number.

    int varPositionNew = 0;
    int strlength = 0;
    std::string strResults;

    if (script_variables[varPosition].tag != T_STRING) {
        //Variable changed it's nature. Free the old values
        script_variables[varPosition].tag = T_STRING;
        if (script_variables[varPosition].Value.pi != NULL) {
            delete script_variables[varPosition].Value.pi;
            script_variables[varPosition].Value.pi = NULL;
        }

        if (script_variables[varPosition].Value.pf != NULL) {
            delete script_variables[varPosition].Value.pf;
            script_variables[varPosition].Value.pf = NULL;
        }
    }
    while (p_prog->index <= endOFline->index) {
        switch (current_token_word) {
        case TOKEN_STRING: {
            //We have only a string ..or maybe a string calculation.
            strlength = strlen(p_prog->txtString) - 2;  //The string will contains both "" and \n
            if (p_prog->txtString[strlength + 1]) p_prog->txtString[strlength + 1] = '\0'; //Remove \n
            if (p_prog->txtString[strlength] == '\"') p_prog->txtString[strlength] = '\0'; //Remove \"
            strResults.append(p_prog->txtString + 1, strlength);
            process_token(TOKEN_STRING);
            break;
        }
        case TOKEN_VARIABLE: {
            varPositionNew = TestSystem_get_variable(NULL);
            if (varPositionNew == -1) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
                quitScriptProgram(1);
                return;
            }
            strResults.append(script_variables[varPositionNew].Value.txtvalue);
            process_token(TOKEN_VARIABLE);
            break;
        }
        case TOKEN_PLUS: {
            /** When you have a plus, you have either addition of two variable, or addition of variable with a new string.*/
            process_token(TOKEN_PLUS);
            if (current_token_word == TOKEN_STRING) {
                //We add a new string to the old variable.
                strlength = strlen(p_prog->txtString) - 3; /// We have 3 extra char .. [0], [strlen], [strlen-1]
                if (p_prog->txtString[strlength + 2] == '\n') p_prog->txtString[strlength + 1] = '\0'; //Remove \n"
                if (p_prog->txtString[strlength + 1] == '\"') p_prog->txtString[strlength] = '\0'; //Remove \"

                strlength = script_variables[varPosition].sizeOfArray + strlength;  //we don't need to add more '\0'
                strResults.append(p_prog->txtString + 1);
                process_token(TOKEN_STRING);
            }
            else if (current_token_word == TOKEN_VARIABLE) {
                //We add a new variable to the old variable
                varPositionNew = TestSystem_get_variable(NULL);
                if (varPositionNew == -1) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
                    quitScriptProgram(1);
                    return;
                }
                strlength = script_variables[varPosition].sizeOfArray + script_variables[varPositionNew].sizeOfArray;
                strResults.append(script_variables[varPositionNew].Value.txtvalue);
                process_token(TOKEN_VARIABLE);
            }
            break;
        }
        default: {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_DEFINE_ADD_ALLOWED_STRING_VAR);
            quitScriptProgram(1);
            return;
        }
        }
    }
    if (script_variables[varPosition].Value.txtvalue != NULL) {
        delete[]script_variables[varPosition].Value.txtvalue;
        script_variables[varPosition].Value.txtvalue = NULL;
    }
    script_variables[varPosition].Value.pf = NULL;
    script_variables[varPosition].Value.pi = NULL;
    script_variables[varPosition].tag = T_STRING;
    script_variables[varPosition].Value.txtvalue = (char*)calloc(strResults.size() + 2, sizeof(char));
    if (script_variables[varPosition].Value.txtvalue == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_CALCULATE_LEFTSIDE);
        quitScriptProgram(1);
        return;
    }
    strncpy(script_variables[varPosition].Value.txtvalue, strResults.c_str(), strResults.size()); //We don't need to add '\0' as we uses calloc
    script_variables[varPosition].sizeOfArray = strResults.size() + 2;
}

float get_next_number_or_variable_value_Parenthesis() {
    float value = 0.0;              // save the resulting value
    instr_linkedList* saveBefLast = 0;

    process_token(TOKEN_LEFT_PARENTHESIS);
    // calculate inside the parenthesis. We must go back one step to get the enum
    instr_linkedList* tempstr = p_prog->prev;   //Save the position first.
    noOfParenthesis++;

    while (current_token_word != TOKEN_EOF && noOfParenthesis >= 1) {
        if (current_token_word == TOKEN_LEFT_PARENTHESIS) {
            noOfParenthesis++;
        }
        if (current_token_word == TOKEN_RIGHT_PARENTHESIS) {
            noOfParenthesis--;
            if (noOfParenthesis == 0) break; //go out from here.
        }

        check_and_get_next_token();
    }

    if (current_token_word != TOKEN_RIGHT_PARENTHESIS) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_PARENTHESIS_MISSING);
        return quitScriptProgram(1);
    }
    saveBefLast = p_prog->prev;
    p_prog = tempstr;

    token_jump_to(p_prog);
    value = arithmetic_expression(tempstr, saveBefLast);
    if (current_token_word == TOKEN_RIGHT_PARENTHESIS)
        process_token(TOKEN_RIGHT_PARENTHESIS);
    return value;
}
float get_next_number_or_variable_value_Number() {
    float value;
    value = convert_text_to_number();
    process_token(TOKEN_NUMBER);
    return value;
}
float get_next_number_or_variable_value_SizeOfVar() {
    //We need to get the variable name and return the sizeof array.
    float value = 0.0;              // save the resulting value
    int varPosition = 0;

    process_token(TOKEN_SIZE_OF_VAR);
    if (current_token_word != TOKEN_LEFT_PARENTHESIS) {
        //SYNTAX ERROR AS SIZEOF(VARIABLENAME) SHOULD BE USED.
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SIZE_OF_VAR);
        quitScriptProgram(1);
        return -1;
    }
    process_token(TOKEN_LEFT_PARENTHESIS);
    if (current_token_word != TOKEN_VARIABLE) {
        //SYNTAX ERROR AS SIZEOF(VARIABLENAME) SHOULD BE USED.
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SIZE_OF_VAR);
        quitScriptProgram(1);
    }
    varPosition = TestSystem_get_variable(NULL);
    if (varPosition == -1) {
        //SYNTAX ERROR AS SIZEOF(VARIABLENAME) SHOULD BE USED.
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SIZE_OF_VAR);
        quitScriptProgram(1);
    }
    value = script_variables[varPosition].sizeOfArray;
    process_token(TOKEN_VARIABLE);
    if (current_token_word != TOKEN_RIGHT_PARENTHESIS) {
        //SYNTAX ERROR AS SIZEOF(VARIABLENAME) SHOULD BE USED.
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SIZE_OF_VAR);
        quitScriptProgram(1);
        return -1;
    }
    process_token(TOKEN_RIGHT_PARENTHESIS);
    return value;
}
//The next calculation number was a variable.
float get_next_number_or_variable_value_Variable() {
    int varPosition = TestSystem_get_variable(NULL);
    process_token(TOKEN_VARIABLE);
    if (varPosition == -1) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_VARIABLE_NOT_FOUND);
        return quitScriptProgram(1);
    }
    float value = 0.0;    // save the resulting value
    //The variable is found and it is single variable not an array
    if (script_variables[varPosition].sizeOfArray == 1) {
        if (script_variables[varPosition].tag == T_FLOAT)    value = *script_variables[varPosition].Value.pf;
        else if (script_variables[varPosition].tag == T_INT)  value = (float)*script_variables[varPosition].Value.pi;
    }//END get variable  - No array
    else {
        int element = 0;
        //We have an array. Try to find out the value correctly.
        if (current_token_word != TOKEN_LEFT_SQUARE_BRACKET) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND0);
            return quitScriptProgram(1);
        }
        process_token(TOKEN_LEFT_SQUARE_BRACKET);
        if (current_token_word == TOKEN_VARIABLE) {
            //We have a variable inside the brackets main variable .. i.e. a[g]=...etc
            int varPosition2 = TestSystem_get_variable(NULL);
            if (varPosition2 == -1) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_INDEX_INSIDE_ARRAY_WRONG);
                return quitScriptProgram(1);
            }
            process_token(TOKEN_VARIABLE);
            if (script_variables[varPosition2].tag == T_FLOAT && script_variables[varPosition2].sizeOfArray == 1) element = (unsigned int)*script_variables[varPosition2].Value.pf;
            else if (script_variables[varPosition2].tag == T_INT && script_variables[varPosition2].sizeOfArray == 1) element = *script_variables[varPosition2].Value.pi;
            else {
                //We shouldn't be here .. this action is not supported
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_INDEX_INSIDE_ARRAY_WRONG);
                return quitScriptProgram(1);
            }

            if (element >= script_variables[varPosition].sizeOfArray) {
                //User entered a wrong element.. It doesn't exist
                DEBUG_MESSAGE("size of array %i\n ", script_variables[varPosition].sizeOfArray);                  //<------
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ELEMENT_NOT_FOUND);
                return quitScriptProgram(1);
            }

            if (script_variables[varPosition].tag == T_FLOAT)  value = script_variables[varPosition].Value.pf[element];
            if (script_variables[varPosition].tag == T_INT)  value = script_variables[varPosition].Value.pi[element];
            else {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND1);
                return quitScriptProgram(1);
            }
            if (current_token_word != TOKEN_RIGHT_SQUARE_BRACKET) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_SQUARE_BRACKET_MISSING);
                return quitScriptProgram(1);
            }
            process_token(TOKEN_RIGHT_SQUARE_BRACKET);
            return value;
        }
        if (current_token_word == TOKEN_NUMBER) {
            element = (unsigned int)convert_text_to_number();
            process_token(TOKEN_NUMBER);
            if (current_token_word != TOKEN_RIGHT_SQUARE_BRACKET) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_SQUARE_BRACKET_MISSING);
                return quitScriptProgram(1);
            }
        }
        process_token(TOKEN_RIGHT_SQUARE_BRACKET);
        if (element >= script_variables[varPosition].sizeOfArray) {
            //User entered a wrong element.. It doesn't exist
            DEBUG_MESSAGE("size of array %i\n ", script_variables[varPosition].sizeOfArray);                 //<------
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ELEMENT_NOT_FOUND);
            return quitScriptProgram(1);
        }
        if (script_variables[varPosition].tag == T_FLOAT)    value = script_variables[varPosition].Value.pf[element];
        else if (script_variables[varPosition].tag == T_INT)  value = (float)script_variables[varPosition].Value.pi[element];
    }
    return value;
}
