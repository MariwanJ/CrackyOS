/**
 ******************************************************************************
 * Variable definition (inside the script program)instruction
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief  This function will take care of the variables and the rest of the line.
 *         Note: All variables are global. You can change the nature of the variable at
 *         any time from text to int or to float vice versa
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
#include "variable.h"
 /**
  *Syntax:
  *        VARIABLE_NAME = NUMBER/VARIABLE/Expression ;
  *     OR VARIABLE_NAME ++;
  *        VARIABLE_NAME --;
  *     OR
  *     VARIABLE_NAME[] = {NUM1,NUMB2,MUB3,..ETC};
  *     VARIABLE_NAME[i]= NUMBER/VARIABLE/Expression ;
  *     @return -1 if error , >=0 if OK
  */

  /*                          var
                             /  \
                           /      \
                           [       = -->nr -->save value
                         / \
                       /     \
                       ]        nr
                      / \       /  \
                    /     \    /     \
                  =    ERROR   ]      = -->ERROR
                 / \           NR OR
               /     \        {NR,NR}
              nr     {n2,2,2}   \--> SAVE VALUE
             /         \
          ERROR         SAVE VALUE

  */

int token_variable(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        DEBUG_MESSAGE("INFO %i\n", DBG_MSG_VARIABLE_START);
    int varPosition;
    int stringLength = 0;
    char* var_buf = NULL;
    int newVariable = 0;
    stringLength = (strlen(p_prog->txtString));

    var_buf = (char*)calloc(sizeof(char) * (stringLength + 1), sizeof(char));
    if (var_buf == NULL)
    {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_VAR_BUF);
        return quitScriptProgram(1);//Exit the program
    }

    strncpy(var_buf, p_prog->txtString, stringLength); //We don't need to add '\0' as we uses calloc

    varPosition = TestSystem_get_variable(var_buf);
    process_token(TOKEN_VARIABLE);           // Get next token word.

    if (varPosition == -1) {
        //Variable not found .. save new variable
        varPosition = TestSystem_get_empty_variable_pos();
        newVariable = 1;  //We have new variable.
        if (varPosition < 0) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_SCRIPT_VARIABLE_OVERFLOW);
            return quitScriptProgram(1);
        }
        //Add a new variable .. but still not totally defined.
        addNewScriptVariable(var_buf, varPosition);
        var_buf = NULL;
    }
    if (var_buf != NULL)
        free(var_buf);
    var_buf = NULL;

    switch (current_token_word) {
    case TOKEN_PLUSPLUS:
    case TOKEN_MINUSMINUS: {
        token_plusminus(varPosition, newVariable);
    }    break;
    case TOKEN_LEFT_SQUARE_BRACKET: {
        toke_leftSquareBracket(varPosition, newVariable);
    }break;
    case TOKEN_EQUAL: {
        token_equial(varPosition, newVariable);
    }break;
    default: {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_BAD_INSTRUCTION_EQUAL_NOT_FOUND);
        // IT wasn't a variable, seems to be a bad instruction ..
        return quitScriptProgram(1);// Nothing to do more
    }
    }
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_VARIABLE_START);
    return 1;
}

int token_plusminus(const int& varPosition, int& newVariable) {
    if (varPosition == -1) {
        //ERROR .. We shouln't be here. When the variable is new .. this will happen.
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_NOT_INT_NUMBER_INC);
        return quitScriptProgram(1);
    }
    // We have increment/decrement
    if (script_variables[varPosition].tag == T_FLOAT) {
        if (current_token_word == TOKEN_PLUSPLUS)
            (*script_variables[varPosition].Value.pf)++;
        else
            (*script_variables[varPosition].Value.pf)--;
    }
    else if (script_variables[varPosition].tag == T_INT) {
        if (current_token_word == TOKEN_PLUSPLUS)
            (*script_variables[varPosition].Value.pi)++;
        else
            (*script_variables[varPosition].Value.pi)--;
    }
    else
    {
        //ERROR .. We shouln't be here. When the variable is new .. this will happen.
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_NOT_INT_NUMBER_INC);
        return quitScriptProgram(1);
    }

    process_token(current_token_word);
    return 1;
}
int toke_leftSquareBracket(int& varPosition, int& newVariable) {
    // We have an array. Deal with the array correctly. Either it is a Array[]={...}, Array[nr]={value},  OR Array[nr]= ...
    process_token(TOKEN_LEFT_SQUARE_BRACKET);
    switch (current_token_word) {
        //We have Array[]={...}
        // No number should have been specified  so we have only var[]={nr,nr,nr..etc}
    case TOKEN_RIGHT_SQUARE_BRACKET: {
        toke_leftSquareBracket_token_rightSquareBracket(varPosition, newVariable);
        return 1;
    } //end case Array Square brackets.
    case TOKEN_NUMBER: {
        toke_leftSquareBracket_token_Number(varPosition, newVariable);
        //Here we have an array, one/several value(s) will be updated .or a new array will be created.
        return 1;
    }
    default: {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_SQUARE_BRACKET_MISSING);
        return quitScriptProgram(1);
    }
    }
    return 1;
}
int token_equial(int& varPosition, int& newVariable) {
    float results = 0.0;
    // This is wrong  2021-02-19
    process_token(TOKEN_EQUAL);
    if ((current_token_word == TOKEN_VARIABLE && CheckIfTheRightSideIsVarOfArrayString() == 1) || current_token_word == TOKEN_STRING) {
        //We have a string equation .. Calculate this part separately
        CalculateRightSideofVariableEquation_TEXT(varPosition);
        return 1;
    }
    // We found '=' , get the value and save the variable.
    // We have either two variable that they are equal or an arithmetic operation.
    /*
                 code for arithmetic operation should be here.
    */
    /**
     *  var = Array variable is not implemented TODO : Write code to support it 2021-02-15
     *
     * */
    results = CalculateRightSideofVariableEquation();       //This is correct ONLY for numerical expressions. If you have a text manipulation .. this will fails.
    script_variables[varPosition].sizeOfArray = 1;
    if (newVariable == 1) {
        if (ceil(results) == results) {
            script_variables[varPosition].tag = T_INT;
            script_variables[varPosition].Value.pi = new int[1];
            if (script_variables[varPosition].Value.pi == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                return quitScriptProgram(1);
            }
            *script_variables[varPosition].Value.pi = (int)results;
        }
        else {
            script_variables[varPosition].tag = T_FLOAT;
            script_variables[varPosition].Value.pf = new float[1];
            if (script_variables[varPosition].Value.pf == NULL)
            {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                return quitScriptProgram(1);
            }
            *script_variables[varPosition].Value.pf = results;
        }
    }
    else { //Update the value.
        if (script_variables[varPosition].tag == T_FLOAT) {
            *script_variables[varPosition].Value.pf = results;
        }
        else {
            if (script_variables[varPosition].tag == T_INT) {
                if (ceil(results) == results) {
                    *script_variables[varPosition].Value.pi = (int)results;
                }
                else {
                    //Value has changed it's nature .. from INT to Float.
                    script_variables[varPosition].tag = T_FLOAT;
                    delete (script_variables[varPosition].Value.pi);
                    script_variables[varPosition].Value.pi = NULL;
                    script_variables[varPosition].Value.pf = new float[1];
                    if (script_variables[varPosition].Value.pf == NULL)
                    {
                        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                        return quitScriptProgram(1);//Exit the program
                    }
                    *script_variables[varPosition].Value.pf = results;
                }
            }
        }
    }
    return 1;
}

/**We have an var[]
 * either it is var[] = nr
 * or var[]={nr1,nr2,..etc}
 * but both the left side is without number.
 * */
int toke_leftSquareBracket_token_rightSquareBracket(int& varPosition, int& newVariable) {
    int NoLeftCurly = 0;
    //float value = 0;
        //We should have a list of elements here {NR1,NR2,..ETC }. We don't allow other syntax
    process_token(TOKEN_RIGHT_SQUARE_BRACKET);
    if (current_token_word != TOKEN_EQUAL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_NO_EQUAL_SIGN);
        return quitScriptProgram(1);
    }
    //We have equal sign .. collect the items.
    process_token(TOKEN_EQUAL);
    if (current_token_word != TOKEN_LEFT_CURLY_BRACKET) {
        DEBUG_MESSAGE("Warning %i\n", DBG_MSG_SYNTAX_ERROR_NO_LCURLY_FOUND);
        NoLeftCurly = 1;
    }
    else
    {
        process_token(TOKEN_LEFT_CURLY_BRACKET);
        NoLeftCurly = 0;
    }

    if (current_token_word != TOKEN_NUMBER) { //After the TOKEN_LEFT_CURLY_BRACKET, no numbers found ..go out.
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_ITEM_NOT_FOUND);
        return quitScriptProgram(1);
    }
    /*Count number of comma and we have to decide if the numbers are int or float?? search for '.' if you can find it ..
      all values will be saved as float.*/
      // instr_linkedList* pSaveProg = p_prog->prev;

    int returnResult = 1;
    //Count number of comma (i.e. items)
    if (!NoLeftCurly) {
        int DotFound = 0;
        int  countComma = countCommaInAssignment(DotFound);
        //Item of array found .. save it.
        if (countComma == 0) {
            //We have only one item  It in the array
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_WARNING_ONLY_ONE_ITEM_FOUND);
            //We know that the variable is not registered. Create a new and calculate the array size also which is one
            returnResult = newOneNumberSquareBracket(varPosition, newVariable, DotFound);
        }
        else {
            //We have multiple numbers.. Save them in the array.
            returnResult = newMultipleNumberSquareBracket(varPosition, newVariable, DotFound, countComma);
            if (current_token_word == TOKEN_RIGHT_CURLY_BRACKET)
                process_token(TOKEN_RIGHT_CURLY_BRACKET);
        }
    }
    else {
        float result2 = CalculateRightSideofVariableEquation();
        if (((int)result2) == result2) {
            //We have an integer value
            script_variables[varPosition].tag = T_INT;
            script_variables[varPosition].Value.pi = new int[1];
            if (script_variables[varPosition].Value.pi == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                return quitScriptProgram(1);
            }
            *script_variables[varPosition].Value.pi = (int)result2;
        }
        else
        {
            //We have a float value
            script_variables[varPosition].tag = T_FLOAT;
            script_variables[varPosition].Value.pf = new float[1];
            if (script_variables[varPosition].Value.pf == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                return quitScriptProgram(1);
            }
            *script_variables[varPosition].Value.pf = result2;
        }
    }
    /*
    if (current_token_word != TOKEN_RIGHT_CURLY_BRACKET) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_TOKEN_RIGHT_CURLY_BRACKET_NOTFOUND);
        return quitScriptProgram(1);
    }
    */
    if (current_token_word == TOKEN_RIGHT_CURLY_BRACKET)
        process_token(TOKEN_RIGHT_CURLY_BRACKET);
    return returnResult;
}

/**We have an var[nr]
 * either it is var[nr] = nr
 * or var[nr]=nr1
 * but both the left side is without number.
 * */
int toke_leftSquareBracket_token_Number(int& varPosition, const int& newVariable) {
    float result2;
    float results = convert_text_to_number(); //Number of elements
    int arraySize = (int)results;
    if (arraySize <= 0 || results != arraySize) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_NUMBER);
        return quitScriptProgram(1);
    }
    process_token(TOKEN_NUMBER);
    if (current_token_word != TOKEN_RIGHT_SQUARE_BRACKET) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_SQUARE_BRACKET);
        return quitScriptProgram(1);
    }
    process_token(TOKEN_RIGHT_SQUARE_BRACKET);
    if (current_token_word != TOKEN_EQUAL)
    {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_EQUAL_NOT_FOUND);
        return quitScriptProgram(1);
    }
    process_token(TOKEN_EQUAL);
    if (newVariable == 1) {
        /**
        *   We might have a[3]={1,2,3}  values will be assigned for each item.
        *   Or            a[3]= 4 ---> all values are 4
        */

        if (current_token_word == TOKEN_LEFT_CURLY_BRACKET) {
            // a[3]={1,2,3}  values will be assigned for each item.
            int DotFound = 0;
            process_token(TOKEN_LEFT_CURLY_BRACKET);
            int countComma = countCommaInAssignment(DotFound);
            //Save the values. a[3]={2}   -- > this should be accepted.
            newMultipleNumberSquareBracket(varPosition, newVariable, DotFound, countComma);
            if (current_token_word == TOKEN_RIGHT_CURLY_BRACKET)
                process_token(TOKEN_RIGHT_CURLY_BRACKET);
        }
        else if (current_token_word == TOKEN_NUMBER) {
            //  a[3]= 4 ---> all values are 4
                    //Get the value to be assigned to all elements.
            result2 = convert_text_to_number();
            process_token(TOKEN_NUMBER);

            //Size of the array
            script_variables[varPosition].sizeOfArray = arraySize;
            if (((int)result2) == result2) {
                //We have an integer value
                script_variables[varPosition].tag = T_INT;
                script_variables[varPosition].Value.pi = new int[arraySize];
                if (script_variables[varPosition].Value.pi == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                    return quitScriptProgram(1);
                }
                for (int coun = 0; coun < arraySize; coun++)
                    script_variables[varPosition].Value.pi[coun] = result2;//Put the same value inside all locations.
            }
            else
            {
                //We have a float value
                script_variables[varPosition].tag = T_FLOAT;
                script_variables[varPosition].Value.pf = new float[arraySize];
                if (script_variables[varPosition].Value.pf == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                    return quitScriptProgram(1);
                }
                for (int coun = 0; coun < (int)results; coun++)
                    script_variables[varPosition].Value.pf[coun] = result2;//Put the same value inside all locations.
            }
        }
        else {
            //Error
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_NOT_FOUND);
            return quitScriptProgram(1);
        }
    }//End of newVariable ==1
    else if (newVariable == 0) {
        //Not a new variable, but an element inside the array will be updated.
        //If the nature of the variable changed (from single to array), we have
        //to recreate the variable.
        if (current_token_word != TOKEN_NUMBER) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_NUMBER);
            return quitScriptProgram(1);
        }
        result2 = convert_text_to_number();
        process_token(TOKEN_NUMBER);

        if (results < 0 || results != (int)results) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_NUMBER);
            return quitScriptProgram(1);
        }

        if (script_variables[varPosition].sizeOfArray <= (int)results) {
        	//Here we have to remove the old array and make a new a our new array is bigger.
            if (script_variables[varPosition].tag == T_FLOAT)       delete[] script_variables[varPosition].Value.pf;
            else if (script_variables[varPosition].tag == T_INT)    delete[] script_variables[varPosition].Value.pi;
            else if (script_variables[varPosition].tag == T_STRING) delete[] script_variables[varPosition].Value.txtvalue;

            script_variables[varPosition].Value.pf=NULL;
            script_variables[varPosition].Value.pi=NULL;
            script_variables[varPosition].Value.txtvalue=NULL;
            if (int(result2) == result2) {
                //New value is int
                script_variables[varPosition].tag = T_INT;
                script_variables[varPosition].Value.pi = new int[(int)results];
                for (int i = 0; i < results; i++)
                    script_variables[varPosition].Value.pi[i] = result2; //Assign the same value to tall
            }
            else {
                //new value is float
                script_variables[varPosition].tag = T_FLOAT;
                script_variables[varPosition].Value.pf = new float[(int)results];
                for (int i = 0; i < results; i++)
                    script_variables[varPosition].Value.pf[i] = result2; //Assign the same value to tall
            }
            script_variables[varPosition].sizeOfArray = results;
        }
        else {
            //There is only one value .. so just assigned to the global variable.
            result2 = CalculateRightSideofVariableEquation();
            if (script_variables[varPosition].tag == T_FLOAT)
                script_variables[varPosition].Value.pf[(int)results] = result2;
            else if (script_variables[varPosition].tag == T_INT)   script_variables[varPosition].Value.pi[(int)results] = (int)result2;
            else {
                //We shouldn't be here . A string variable cannot be updated with INT/FLOAT
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_STRING_ARRAY_NOT_NUMBER);
                return quitScriptProgram(1);
            }
        }
    }
    return 1;
}

/**
 * User entered the variable as follow
 * var[]=nr1
 * */
int newOneNumberSquareBracket(int& varPosition, const int& newVariable, const int& DotFound) {
    float results = convert_text_to_number();
    process_token(TOKEN_NUMBER);
    script_variables[varPosition].sizeOfArray = 1;
    if (newVariable == 1) {
        if (!DotFound) {
            //Value is int
            script_variables[varPosition].tag = T_INT;
            script_variables[varPosition].Value.pi = new int[1];
            if (script_variables[varPosition].Value.pi == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                return quitScriptProgram(1);//Exit the program
            }
            *script_variables[varPosition].Value.pi = (int)results;
        }
        else {
            //All value will be float
            script_variables[varPosition].tag = T_FLOAT;
            script_variables[varPosition].Value.pf = new float[1];
            if (script_variables[varPosition].Value.pf == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                return quitScriptProgram(1);//Exit the program
            }
            *script_variables[varPosition].Value.pf = results;
        }
    }//End Var[]={OnlyOneNr} and the variable is new
    else {
        //we have an old variable for ex( a[]={5} or a[]= 5    ---
            //We need to remove the variable and re assign it
                //delete the old variable
        if (script_variables[varPosition].tag == T_INT) {
            delete (script_variables[varPosition].Value.pi);
            script_variables[varPosition].Value.pi = NULL;
        }
        else if (script_variables[varPosition].tag == T_FLOAT) {
            delete[](script_variables[varPosition].Value.pf);
            script_variables[varPosition].Value.pf = NULL;
        }
        else if (script_variables[varPosition].tag == T_STRING) {
            delete[](script_variables[varPosition].Value.txtvalue);
            script_variables[varPosition].Value.txtvalue = NULL;
        }

        if (!DotFound) {
            //Value is int
            script_variables[varPosition].tag = T_INT;
            script_variables[varPosition].Value.pi = new int[1];
            if (script_variables[varPosition].Value.pi == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                return quitScriptProgram(1);//Exit the program
            }
            *script_variables[varPosition].Value.pi = (int)results;
        }
        else {
            //All value will be float
            script_variables[varPosition].tag = T_FLOAT;
            script_variables[varPosition].Value.pf = new float[1];
            if (script_variables[varPosition].Value.pf == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                return quitScriptProgram(1);//Exit the program
            }
            *script_variables[varPosition].Value.pf = results;
        }
    }//END - We have only one item  It is not an array.
    return 1;
}
/**
 * User entered the variable as follow
 * var[]={nr1,n2, ..etc}
 * */
int newMultipleNumberSquareBracket(int& varPosition, const int& newVariable, const int& DotFound, const int& countComma) {
    float results;
    script_variables[varPosition].sizeOfArray = countComma + 1;
    if (newVariable == 1) {
        //We need to create the variable  --Note : This will be equal to a[sizeOfArray]= number or {number } or {n1,n2,n3 ..etc }
        int i = 0;
        if (DotFound == 1) {
            script_variables[varPosition].Value.pf = new float[countComma + 1];
            if (script_variables[varPosition].Value.pf == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                return quitScriptProgram(1);
            }
            script_variables[varPosition].tag = T_FLOAT;
            for (i = 0; i < countComma + 1; i++) {
                int sign = 0;
                //while(current_token_word==TOKEN_NUMBER || current_token_word==TOKEN_MINUS){
                if (current_token_word == TOKEN_NUMBER)    sign = 1;
                else if (current_token_word == TOKEN_MINUS) { sign = -1; process_token(TOKEN_MINUS); }
                else { DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ARRAY_ITEM_NOT_FOUND); return quitScriptProgram(1); }
                if (current_token_word != TOKEN_NUMBER) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND4);
                    return quitScriptProgram(1);
                }
                results = convert_text_to_number();
                script_variables[varPosition].Value.pf[i] = sign * results;
                process_token(TOKEN_NUMBER);
                if (current_token_word == TOKEN_COMMA)  process_token(TOKEN_COMMA);
            }
        }
        else { // Int array .. get them and save them.
            script_variables[varPosition].Value.pi = new int[countComma + 1];
            if (script_variables[varPosition].Value.pi == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                return quitScriptProgram(1);
            }

            script_variables[varPosition].tag = T_INT;
            for (i = 0; i < countComma + 1; i++) {
                int sign = 0;
                if (current_token_word == TOKEN_NUMBER)    sign = 1;
                else if (current_token_word == TOKEN_MINUS) { sign = -1; process_token(TOKEN_MINUS); }
                else { DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ARRAY_ITEM_NOT_FOUND); return quitScriptProgram(1); }
                if (current_token_word != TOKEN_NUMBER) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND5);
                    return quitScriptProgram(1);
                }
                results = convert_text_to_number();
                script_variables[varPosition].Value.pi[i] = (int)sign * results;
                process_token(TOKEN_NUMBER);
                if (current_token_word == TOKEN_COMMA)  process_token(TOKEN_COMMA);
            }
        }
    }// END OF SAVE new array of int/float
    else {
        //Only update the value .. The variable is found
        if (DotFound == 1) {
            //Float array
            if ((script_variables[varPosition].Value.pf != NULL)) {
                delete script_variables[varPosition].Value.pf;
                script_variables[varPosition].Value.pf = NULL;
            }
            script_variables[varPosition].Value.pf = new float[countComma + 1];
            if (script_variables[varPosition].Value.pf == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                return quitScriptProgram(1);
            }
            script_variables[varPosition].tag = T_FLOAT;
            int i = 0;
            for (i = 0; i < countComma + 1; i++) {
                int sign = 0;
                //while(current_token_word==TOKEN_NUMBER || current_token_word==TOKEN_MINUS){
                if (current_token_word == TOKEN_NUMBER)    sign = 1;
                else if (current_token_word == TOKEN_MINUS) { sign = -1; process_token(TOKEN_MINUS); }
                else { DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ARRAY_ITEM_NOT_FOUND); return quitScriptProgram(1); }
                if (current_token_word != TOKEN_NUMBER) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND6);
                    return quitScriptProgram(1);
                }
                results = convert_text_to_number();
                script_variables[varPosition].Value.pf[i] = sign * results;
                process_token(TOKEN_NUMBER);
                if (current_token_word == TOKEN_COMMA)  process_token(TOKEN_COMMA);
            }
        }
        else {
            //Update variable
            //INT array
            if (script_variables[varPosition].Value.pi != NULL) {
                delete script_variables[varPosition].Value.pi;
                script_variables[varPosition].Value.pi = NULL;
            }
            script_variables[varPosition].Value.pi = new int[countComma + 1];
            if (script_variables[varPosition].Value.pi == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_VAR);
                return quitScriptProgram(1);
            }
            script_variables[varPosition].tag = T_INT;
            int i = 0;
            for (i = 0; i < countComma + 1; i++) {
                int sign = 0;
                if (current_token_word == TOKEN_NUMBER)    sign = 1;
                else if (current_token_word == TOKEN_MINUS) { sign = -1; process_token(TOKEN_MINUS); }
                else { DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ARRAY_ITEM_NOT_FOUND); return quitScriptProgram(1); }
                if (current_token_word != TOKEN_NUMBER) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND7);
                    return quitScriptProgram(1);
                }
                results = convert_text_to_number();
                script_variables[varPosition].Value.pi[i] = (int)sign * results;
                process_token(TOKEN_NUMBER);
                if (current_token_word == TOKEN_COMMA)  process_token(TOKEN_COMMA);
            }
        } //END UPDATE variable INT
    }
    return 1;
}

int countCommaInAssignment(int& DotFound) {
    /*Count number of comma and we have to decide if the numbers are int or float?? search for '.' if you can find it ..
      all values will be saved as float.*/
    instr_linkedList* pSaveProg = p_prog->prev;
    int  countComma = 0;
    float value;
    //Count number of comma (i.e. items)

    while (current_token_word != TOKEN_RIGHT_CURLY_BRACKET && current_token_word != TOKEN_EOF) {
        if (current_token_word == TOKEN_COMMA)   countComma++;
        else if (current_token_word == TOKEN_NUMBER && DotFound != 1) {
            value = convert_text_to_number();
            if (ceil(value) != value) {
                DotFound = 1;       //we have a float value.
            }
        }
        check_and_get_next_token();
    }
    // Go back to the beginning
    token_jump_to(pSaveProg);
    return countComma;
}
