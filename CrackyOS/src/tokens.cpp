/**
 ******************************************************************************
 * @file    tokens.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @brief   Script program decoding and analysis.
 ******************************************************************************
 *
 * Copyright (C) 2026 Mariwan Jalal
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This file is part of CrackyOS.
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

#include "tokens.h"
#include "types.h"

 // Pointers to read the code. Max no of instructions is what "int" give us
instr_linkedList* p_prog = NULL;
instr_linkedList* p_prog_next = NULL;

int current_token_word;
unsigned int script_current_line_no;

//GlobalfunList *functionList=NULL;
canType* gCanVariable = NULL;

/* Variable to keep the PWM channel info*/
pwmpinname PWM_Var;

token_for_object* forExtract = NULL;
token_while_object* whileExtract = NULL;

/*Variable to keep the script line number*/
SourceCodeLine* pScriptIndex = NULL;

/* Variable to keep the label position and name*/
glLBL* globallabelVar = NULL;

//This variable keep the variable_token name and it's value
GlobalVariables* script_variables = NULL;

/**Global variable keeps the name of the last activated interrupt.
 *
 * */

GlobalfunList* varGlobActiveInterruptFunc = NULL;

/**Global variable keeps the linked list of interrupt functions.
 *
 * */
gINTstruct* GlobLinkedListInterruptIn = NULL;

/** Class GlobalVariables Constructor */
GlobalVariables::GlobalVariables()
{
    tag = T_NULL;                //Variable type
    varname = NULL;             //Variable Name
    Value.pi = NULL;              //Value
    Value.pf = NULL;
    Value.txtvalue = NULL;
    sizeOfArray = 0; //Size of the array (if any)
    hashVarnme = 0;
}
/** destructor */
GlobalVariables::~GlobalVariables()
{
    removeVarname();
    removePI();
    removePF();
    removeTXTString();
}
/** Free up varname memory*/
void GlobalVariables::removeVarname()
{
    delete[] varname;
    varname = NULL;
}
/** Free up pf memory*/
void GlobalVariables::removePF()
{
    if (tag != T_FLOAT)
        return; //Nothing to do here
    if (sizeOfArray == 1)      delete (Value.pf);
    else  delete[] Value.pf;
    Value.pf = NULL;
}
/** Free up pi memory*/
void GlobalVariables::removePI()
{
    if (tag != T_INT)
        return; //Nothing to do here
    if (sizeOfArray == 1) delete (Value.pi);
    else delete[] Value.pi;
    Value.pi = NULL;
}
/** Free up txtstring memory*/
void GlobalVariables::removeTXTString()
{
    if (tag != T_STRING)
        return; //Nothing to do here
    if (sizeOfArray == 1) delete (Value.txtvalue);
    else delete[] Value.txtvalue;
    Value.txtvalue = NULL;
}

// This function will remove extra '\' in the string for \n command
void FixStringIssue(char* inStr)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION

        char* pScanner1 = NULL;
    char* pScanner2 = NULL;
    pScanner2 = pScanner1 = inStr;
    while (*pScanner2 != '\0') {
        if (*pScanner2 == '\\' && *(pScanner2 + 1) == 'n') {
            *pScanner1 = '\n';
            pScanner2 = pScanner2 + 1;
        }
        else {
            *pScanner1 = *pScanner2;
        }
        pScanner1++;
        pScanner2++;
    }
    while (*pScanner1 != '\0') {
        *pScanner1 = '\0';
    }
}

//Open the file and put it in an array of char. skip \r and multiple \n
int change_program_to_lower_case(char* prog_pointer)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_NULL

        // This should skip the strings ..
        int counterForQuotationMarks = 0;
    char* p_pointer = prog_pointer;
    char* p_Result = p_pointer;
    if (p_pointer == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND2);
        return quitScriptProgram(1);
    }
    while (*p_pointer != '\0') {
        if (*p_pointer == '"') {
            // String will not be changed. skip them.
            counterForQuotationMarks++;
            do {
                p_pointer++;
                p_Result++;
            } while (*p_pointer != '"' && *p_pointer != '\0');

            if (*p_pointer != '\0') {
                p_pointer++;
                p_Result++;
                counterForQuotationMarks--;
            }
        }
        else {
            switch (*p_pointer) {
            case '\r': {  //Just skip the char
                p_pointer++;
            }
                     break;
            case '\t': { // change it to one space
                *p_Result = ' ';
                p_pointer++;
                p_Result++;
            }
                     break;
            case '\n': {
                *p_Result = *p_pointer;
                p_Result++;
                //Skip multiple \n to reduce memory usage.
                while (*p_pointer == '\n')
                    p_pointer++;
            }
                     break;
            default: {
                int xd = tolower(*p_pointer);
                *p_Result = xd;
                p_pointer++;
                p_Result++;
            }
                   break;
            }
        }
        if (counterForQuotationMarks != 0) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_QUOTATION_MISSING);
            return quitScriptProgram(1);
        }
    }
    return 0;
}
/** This function will save the position of the label used in the script.
 *
 */
int SavelabelPositions(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_NULL

        instr_linkedList* pscan = p_prog;
    token_jump_to(p_prog);
    int counter = 0;
    while (current_token_word != TOKEN_EOF) {
        if (current_token_word == TOKEN_LABEL) {
            /* label found*/
            if (globallabelVar == NULL) {
                globallabelVar = new glLBL;
                if (globallabelVar == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GLOBAL_VAR);
                    quitScriptProgram(1);
                }
                globallabelVar->next = NULL;
                globallabelVar->prev = NULL;
            }
            else {
                globallabelVar->next = new glLBL;
                if (globallabelVar->next == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GLOBAL_VAR);
                    quitScriptProgram(1);
                }
                globallabelVar->next->next = NULL;
                globallabelVar->next->prev = globallabelVar;
                globallabelVar = globallabelVar->next;
            }
            globallabelVar->label.start = p_prog;
            globallabelVar->label.end = p_prog->next;
            globallabelVar->label.name = p_prog->txtString;
        }
        check_and_get_next_token();
    }
    p_prog = pscan;
    return counter;
}
/**
 * This function will do nothing.
 * It is already saved during the initialization.
 * */
int token_label()
{
    process_token(TOKEN_LABEL);   //Nothing to do.
    return 0;
}

/**Free up the memory/Initialize a variable.
 *  But you can specify a variable to delete or all. Send NULL to delete all*/
void IntializeScriptVariable(char* nameOfVariable)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION
        if (nameOfVariable != NULL) {
            /* Here we delete only the content of the variable not the variable position itself */
            if (script_variables == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_VARIABLE_NOT_FOUND);
                quitScriptProgram(1);
                return;
            }
            else {
                for (int i = 0; i < SCRIPT_MAX_VARIABLE_NUMBERS; i++) {
                    //Delete only one variable.
                    int varPosition = TestSystem_get_variable(nameOfVariable);
                    if (varPosition != -1) {

                        script_variables[varPosition].removePI();
                        script_variables[varPosition].removePF();
                        script_variables[varPosition].removeTXTString();
                        script_variables[varPosition].removeVarname();
                        script_variables[varPosition].tag = T_NULL;
                        script_variables[varPosition].hashVarnme = 0;
                        return; //We are done.
                    }
                    else {
                        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_VARIABLE_NOT_FOUND);
                        quitScriptProgram(1);
                        return;
                    }
                }
            }
        }
        else {
            //No name for variable was provided .. initialize all.
            if (script_variables == NULL) {
                script_variables = new GlobalVariables[SCRIPT_MAX_VARIABLE_NUMBERS];

                if (script_variables == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
                    quitScriptProgram(1);
                }
                return;
            }
            else {
                //Remove everything and initialize it again
                for (int i = 0; i < SCRIPT_MAX_VARIABLE_NUMBERS; i++) {
                    script_variables[i].removePI();
                    script_variables[i].removePF();
                    script_variables[i].removeTXTString();
                    script_variables[i].removeVarname();
                    script_variables[i].tag = T_NULL;
                    script_variables[i].hashVarnme = 0;
                }
            }
        }
    return; //we are done
}
/** Initialize internal variable for interactive mode */
int token_initializeInterActive(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        current_token_word = TOKEN_BAD; //Default value for the accessed tokens. The variable will be used while the system read the user-code.

    if (forExtract != NULL) {
        free_forExtract();
    }
    /**FOR -LOOP INSTRUCTION WILL BE ACCEPTED ONLY IF THEY ARE '=SCRIPT_MAX_NESTED_FOR_WHILE_LOOP' NESTED FOR-LOOP. NO MORE. FOR SIMPLICITY*/
    forExtract = (token_for_object*) new token_for_object[SCRIPT_MAX_NESTED_FOR_WHILE_LOOP];
    if (forExtract == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FOR_LOOP);
        quitScriptProgram(1);
    }

    if (whileExtract != NULL) {
        free_whileExtract();
    }
    /**WHILE -LOOP INSTRUCTION WILL BE ACCEPTED ONLY IF THEY ARE '=SCRIPT_MAX_NESTED_FOR_WHILE_LOOP' NESTED FOR-LOOP. NO MORE. FOR SIMPLICITY*/
    whileExtract = (token_while_object*) new token_while_object[SCRIPT_MAX_NESTED_FOR_WHILE_LOOP];
    if (whileExtract == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_WHILE_LOOP);
        quitScriptProgram(1);
    }
    nestedForWhileloop = 0;
    nestedForloop = 0;
    nestedWhileloop = 0;

    //Add to GPIO list the reserved pins.
    add_Reserved_GPIO();

    return 1; //Success
}
/**
 * Initialize the default file names for the system.
 * */
void InitializeDefaultScriptFileName() {
    SCRIPT_PROG_NAME.clear();
    SCRIPT_PROG_NAME_DAD.clear();
    //We use the default names
    SCRIPT_PROG_NAME = DEFAULT_DOT_DAT_FILENAME
        SCRIPT_PROG_NAME_DAD = DEFAULT_DOT_DAD_FILENAME
}
//Setup the pointers and initialize them to the start of the program.
int token_initialize(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        //Clean up mem if there is any.
        CleanUpMemToOriginal();

    /** prepare the .dad file and load it to the memory*/
    TokenizeToFile* scriptPro = NULL;

    scriptPro = new TokenizeToFile(SCRIPT_PROG_NAME.c_str(), SCRIPT_PROG_NAME_DAD.c_str());
    if (scriptPro == NULL) {
        //If file not found this will fail.
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
        return -1;
    }
    if (scriptPro->run() < 0)//Analyze the code and save it in the .dad file.
        return -1; //error
    int noOfLines = countlines(SCRIPT_PROG_NAME_DAD.c_str());
    //Read the file to memory
    readTolinkedList(SCRIPT_PROG_NAME_DAD.c_str(), noOfLines);
    if (scriptPro != NULL) {
        delete scriptPro;
        scriptPro = NULL;
    }
    pScriptIndex = NULL;
    if (p_prog == NULL) return -1;
    p_prog_next = p_prog; ///Not known what value it has. we should initialize it.

    pScriptIndex = new SourceCodeLine(p_prog);
    if (pScriptIndex != NULL)
        pScriptIndex->scanSourceCode();
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
        quitScriptProgram(1);
    }

    if (forExtract != NULL) {
        free_forExtract();
    }

    if (forExtract != NULL) {
        free_forExtract();
    }
    /**FOR -LOOP INSTRUCTION WILL BE ACCEPTED ONLY IF THEY ARE '=SCRIPT_MAX_NESTED_FOR_WHILE_LOOP' NESTED FOR-LOOP. NO MORE. FOR SIMPLICITY*/
    forExtract = (token_for_object*) new token_for_object[SCRIPT_MAX_NESTED_FOR_WHILE_LOOP];
    if (forExtract == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FOR_LOOP);
        quitScriptProgram(1);
    }

    if (whileExtract != NULL) {
        free_whileExtract();
    }
    /**WHILE -LOOP INSTRUCTION WILL BE ACCEPTED ONLY IF THEY ARE '=SCRIPT_MAX_NESTED_FOR_WHILE_LOOP' NESTED FOR-LOOP. NO MORE. FOR SIMPLICITY*/
    whileExtract = (token_while_object*) new token_while_object[SCRIPT_MAX_NESTED_FOR_WHILE_LOOP];
    if (whileExtract == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_WHILE_LOOP);
        quitScriptProgram(1);
    }
    nestedForWhileloop = 0;
    nestedForloop = 0;
    nestedWhileloop = 0;

    current_token_word = TOKEN_BAD; //Default value for the accessed tokens. The variable will be used while the system read the user-code.
    // The same here .. SaveLablePosition should be done before running the code 2018-08-22
    SavelabelPositions();   //Save label positions for GOTO instruction.

    current_token_word = TOKEN_BAD; //Default value for the accessed tokens. The variable will be used while the system read the user-code.
    token_jump_to(p_prog);          // Bring the first instruction

    //Add to GPIO list the reserved pins.
    add_Reserved_GPIO();
    return 1; //Success
}

//Find a variable in the global variable
int TestSystem_get_variable(char* VarName)
{
    //we will make it simple. Support only max 25 variables
    //Return the variable position in the global array or -1 if not found.
    /*
     If you are trying to find a variable that was created before, don't send the name.
     But if you are trying to save a value to an old/new variable, send the var name .. if you cannot find it .. you can create it by yourself.
     This function will not create any variable.
     */

    int results;
    /**
     * Before doing anything, we shouldn't allow any operation if the
     * script_variables=NULL
     * */
    if (script_variables == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
        return quitScriptProgram(1);
    }
    uint32_t pTempVarNameHash = 0;
    //Function called without variable name .. We pick the name here
    if (VarName == NULL) {
        //Function called without variable name .. We pick the name here
        pTempVarNameHash = CalculateHashOfString(p_prog->txtString, strlen(p_prog->txtString));
    }
    else {
        //We have the name ..
        pTempVarNameHash = CalculateHashOfString(VarName, strlen(VarName));
    }

    int i = 0;
    results = -1;  //Return -1 if we couln't find it

    while (i < SCRIPT_MAX_VARIABLE_NUMBERS) {
        if (script_variables[i].varname != NULL) {
            if (script_variables[i].hashVarnme == pTempVarNameHash) {
                // We found the variable.
                results = i;
                break;
            }
        }
        i++;
    }
    return results;
}

/**
 * Quit the execution of the script program and try to go out from the function by changing the StopExecution.
 *
 * */
int quitScriptProgram(unsigned char exitType)
{
    if (pScriptIndex != NULL) {
        listObj* currentLineInfo = pScriptIndex->findPointer(p_prog);
        if (currentLineInfo != NULL)
            SERIAL_MESSAGE("Execution Terminated: at Line %i\n", currentLineInfo->lineNo);
    }
    else {
        if (p_prog == NULL)
            SERIAL_MESSAGE("Execution Terminated\n");
        else  if (p_prog->txtString == NULL)
            SERIAL_MESSAGE("Execution Terminated\n");
        else
            SERIAL_MESSAGE("Execution Terminated TOKEN=%s\n", p_prog->txtString);
    }
    InterActiveMode = 1;
    StopExecution = 1;
    return -1; //Execution stopped
}
/*
void capitalize(char* prog_pointer)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION

        // This should skip the strings ..

    char* p_pointer = prog_pointer;
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_TO_UPPER_START);
    if (p_pointer == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND3);
        quitScriptProgram(1);
    }
    else {
        int counterForQuotationMarks = 0;
        while (*p_pointer != '\0') {
            if (*p_pointer == '"') {
                counterForQuotationMarks++;
                do {
                    p_pointer++;
                } while (*p_pointer != '"' && *p_pointer != '\0');
                if (*p_pointer != '\0') {
                    p_pointer++;
                    counterForQuotationMarks--;
                }
            }
            else {
                *p_pointer = toupper(*p_pointer);
                p_pointer++;
            }
            if (counterForQuotationMarks != 0) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_QUOTATION_MISSING);
                quitScriptProgram(1);
            }
        }
    }
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_TO_LOWER_END);
}
*/

/**
 *  \brief Function that retrieve a value from the global script variable
 *
 *  \return Value from global script variables
 *
 *  \details Use this function whenever you need to retrieve a value from the script program.
 */
float RetrieveValueInGlobalVariable(void)
{
    float result = 0;
    // We have a variable, we need to retrieve the value.

    if (script_variables == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR);
        return quitScriptProgram(1);
    }

    int varPosition = TestSystem_get_variable(NULL);
    process_token(TOKEN_VARIABLE);
    if (varPosition == -1) {
        //Syntax error. variable not found
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
        quitScriptProgram(1);
    }
    //Here , we need to check if the variable is an array or just a variable
    if (current_token_word == TOKEN_LEFT_SQUARE_BRACKET) {
        //We have an array here.
        process_token(TOKEN_LEFT_SQUARE_BRACKET);
        int index = 0;
        switch (current_token_word) {
        case TOKEN_NUMBER: {
            index = (int)convert_text_to_number();
            process_token(TOKEN_NUMBER);
            break;
        }
        case TOKEN_VARIABLE:
            //We have a variable inside the brackets  i.e. array[variable]
            int varPosition2 = TestSystem_get_variable(NULL);
            process_token(TOKEN_VARIABLE);
            if (varPosition2 == -1) {
                //Syntax error. variable not found
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND_DELAY);
                quitScriptProgram(1);
            }
            if (script_variables[varPosition2].tag == T_INT) {
                index = (int)*script_variables[varPosition2].Value.pi;
            }
            else if (script_variables[varPosition2].tag == T_FLOAT) {
                index = (int)*script_variables[varPosition2].Value.pf;
            }
            else {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ARRAY_VAR_NOT_FOUND);
                quitScriptProgram(1);
            }
            break;
        }
        if (current_token_word != TOKEN_RIGHT_SQUARE_BRACKET) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_NOT_FOUND);
            quitScriptProgram(1);
        }
        process_token(TOKEN_RIGHT_SQUARE_BRACKET);

        //We have everything, return the value
        if (index < 0 || index >= script_variables[varPosition].sizeOfArray) {
            //Script program try to retrieve a value that doesn't exist
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_INVALID_ARRAY_INDEX);
            quitScriptProgram(1);
        }
        if (script_variables[varPosition].tag == T_INT)
            result = (float)script_variables[varPosition].Value.pi[index];
        else if (script_variables[varPosition].tag == T_FLOAT)
            result = script_variables[varPosition].Value.pf[index];
        else if (script_variables[varPosition].tag == T_STRING)
            result =
            (float)script_variables[varPosition].Value.txtvalue[index];
    }
    else {
        //We should have only a normal variable- No array
        if (script_variables[varPosition].tag == T_INT) {
            result = (int)*script_variables[varPosition].Value.pi;
        }
        else if (script_variables[varPosition].tag == T_FLOAT) {
            result = *script_variables[varPosition].Value.pf;
        }
        else {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
            quitScriptProgram(1);
        }
    }
    return result;
}

/** Free PWM variable*/
void free_PWM_Var(void)
{
    if (PWM_Var.pwm1.pwm != NULL) {
        delete (PWM_Var.pwm1.pwm);
        PWM_Var.pwm1.pwm = NULL;
    }
    if (PWM_Var.pwm2.pwm != NULL) {
        delete (PWM_Var.pwm2.pwm);
        PWM_Var.pwm2.pwm = NULL;
    }
    if (PWM_Var.pwm3.pwm != NULL) {
        delete(PWM_Var.pwm3.pwm);
        PWM_Var.pwm3.pwm = NULL;
    }
    if (PWM_Var.pwm4.pwm != NULL) {
        delete (PWM_Var.pwm4.pwm);
        PWM_Var.pwm4.pwm = NULL;
    }
    PWM_Var.pwm1.enum_pinname = PWM_Var.pwm2.enum_pinname = PWM_Var.pwm3.enum_pinname = PWM_Var.pwm4.enum_pinname = TOKEN_NC;
}
/** Free whileExtract  variable*/
void free_whileExtract(void)
{
    if (whileExtract == NULL)
        return;
    delete[] whileExtract;
    whileExtract = NULL;
}
void free_forExtract(void)
{
    if (forExtract == NULL)
        return;
    delete[] forExtract;
    forExtract = NULL;
}
/** Free for_while variable*/
void free_functionList(void)
{
    if (functionList == NULL)
        return;
    while (functionList->next != NULL)
        functionList = functionList->next;
    while (functionList->prev != NULL) {
        functionList = functionList->prev;
        delete functionList->next;
        functionList->next = NULL;
    }
    delete functionList;
    functionList = NULL;
}
/** Free Label variable*/
void free_globallabelVar(void)
{
    if (globallabelVar != NULL) {
        while (globallabelVar->next != NULL)
            globallabelVar = globallabelVar->next;
        while (globallabelVar->prev != NULL) {
            globallabelVar = globallabelVar->prev;
            delete globallabelVar->next;
            globallabelVar->next = NULL;
        }
        delete globallabelVar;
        globallabelVar = NULL;
    }
}
/**
 * Will find the end of the instruction line
*  but it doesn't jump to the end of the line
*  \return end of the line
*/
instr_linkedList* RetrieveEndOfInstruction(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_NULL

        instr_linkedList* SaveCurrentLine = p_prog->prev; //point to the enum value
    instr_linkedList* endofLine = NULL;
    while (current_token_word != TOKEN_NEW_LINE
        && current_token_word != TOKEN_SEMICOLON
        && current_token_word != TOKEN_EOF) {
        check_and_get_next_token();
    }
    endofLine = p_prog->prev; /*Why prev? because we always jump twice. Pointer is always on the text part ..
                                not the enum part. but you have to take back for this to work*/
                                //Now we have either NewLine or Semicolon or EOF. We shoul go back one step
    p_prog = SaveCurrentLine;
    token_jump_to(p_prog);
    return endofLine;
}
//
///** A function to add string variables and return a new mem allocated variable
// *You should free up the mem.
// */
//char* concat(int count, ...)
//{
//    //Stop running if we have error
//    STOP_CODE_EXECUTION_NULL
//
//    va_list ap;
//    int i;
//    // Find required length to store merged string
//    int len = 1; // room for NULL
//    va_start(ap, count);
//    for (i = 0; i < count; i++)
//        len += strlen(va_arg(ap, char*));
//    va_end(ap);
//
//    // Allocate memory to concat strings
//    char* merged = NULL;
//    merged = (char*)calloc(sizeof(char), len + 1);
//    if (merged == NULL) {
//        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_CAN_SEND);
//        quitScriptProgram(1);
//        return NULL;
//    }
//    int null_pos = 0;
//
//    // Actually concatenate strings
//    va_start(ap, count);
//    for (i = 0; i < count; i++) {
//        char* s = va_arg(ap, char*);
//       int sizeofthestring = strlen(s);
//        if (len > 0 && (len - null_pos) > sizeofthestring) {
//            strncpy(merged + null_pos, s, sizeofthestring);
//            null_pos += sizeofthestring;
//        }
//    }
//    va_end(ap);
//    return merged;
//}
/**
 * Clean mem from free_GlobalGPIO_REG
 * */
int free_GlobalGPIO_REG() {
    if (GlobalGPIO_REG == NULL) return 0; //Nothing to do
    delete GlobalGPIO_REG;
    GlobalGPIO_REG = NULL;
    return 0;
}
/**
 * Clean mem from CAN variable
 * */
int freeUp_gCanVariable() {
    canType* pScan = gCanVariable;
    if (pScan == NULL)
        return 0; //Nothing to do
    while (pScan->next != NULL) {
        pScan = pScan->next;
    }

    while (pScan->prev != NULL) {
        pScan = pScan->prev;
        delete pScan->next->_CanDevice;
        pScan->next->_CanDevice = NULL;
        delete pScan->next;
        pScan->next = NULL;
    }
    delete pScan->_CanDevice;
    delete pScan;
    return 0;
}

/**
 * Clean mem from global script variable
 * */
void freeUp_ScriptVariable() {
    if (script_variables == NULL) return; //Nothing to clean
    for (int i = 0; i < SCRIPT_MAX_VARIABLE_NUMBERS; i++) {
        switch (script_variables[i].tag) {
        case T_INT:    script_variables[i].removePI(); break;
        case T_FLOAT:  script_variables[i].removePF(); break;
        case T_STRING: script_variables[i].removeTXTString(); break;
        case T_NULL: return;
        }
        script_variables[i].removeVarname();
    }
    delete[] script_variables;
    script_variables = NULL;
}

/**
 * Clean up all variables in the system.
 * This shoulnd't be run when you run instruction by instruction in interactive mode.
 * */
void CleanUpMemToOriginal(void)
{
    free_scriptProg();
    freeUp_gCanVariable();
    free_GlobalGPIO_REG();
    FreeUp_Tickers(-1);     // Ticker  Must be first as it generates interrupt and could cause problem.
    free_scriptGPIOVariable(); //All GPIO In, Out Digital/ Analog
    free_PWM_Var();         //PWM
    free_forExtract();      //For
    free_whileExtract();    //While
    free_globallabelVar();  //Label
    free_functionList();    //Function
    freepScriptIndex();     //Index (script line number)
    freeUp_ScriptVariable(); //ScriptVariable.
    IntializeScriptVariable(NULL); //This must be done .. We cannot let the variable not defined.
}

void process_token(int token_word)
{
    if (token_word != current_token_word) {
        quitScriptProgram(1);
        return;
    }
    /*Introducing Interrupt handling.
     *
     * Whenever GlobActiveInterruptFunc has got a value (not NULL)
     * it means that we have an interrupt that must be executed.
     * After executing the code .. we go to the normal mode again.
     * Warning: Make it short .. not big block of code should be in the
     * interrupt function
     * */

    if (varGlobActiveInterruptFunc != 0) {
        instr_linkedList* OrigionValue = p_prog->prev;  //Return back to the ENUM value
        token_jump_to(varGlobActiveInterruptFunc->start);
        while (current_token_word != TOKEN_LEFT_CURLY_BRACKET) {
            check_and_get_next_token();
        }
        // Will be difficult to tell when it fails here.
        if (current_token_word != TOKEN_LEFT_CURLY_BRACKET) {
            quitScriptProgram(1);
            return;
        }
        check_and_get_next_token();
        instr_linkedList* endOfInstructions = varGlobActiveInterruptFunc->end;
        varGlobActiveInterruptFunc = NULL;  //Remove the link to the function .. so we run it only once.
        Execute_Instructions(endOfInstructions);
        token_jump_to(OrigionValue);
    }

    if (!end_of_program_token()) {
        check_and_get_next_token();
    }
}

/**
 * Allocate a new variable
 * @param name Variable name or NULL
 * @param Position Index position of the variable
 * return -1 if error, >=0 if OK
 * */
int addNewScriptVariable(char* name, int Position) {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS;
    if (name == NULL) {
        //We have the name in p_prog->txtString
        int stringLength = strlen(p_prog->txtString);
        char* var_buf = (char*)calloc(sizeof(char) * (stringLength + 1), sizeof(char));
        if (var_buf == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_VAR);
            return quitScriptProgram(1);  //Exit the program
        }

        strncpy(var_buf, p_prog->txtString, stringLength); //We don't need to add '\0' as we uses calloc
        script_variables[Position].varname = var_buf;
        var_buf = NULL;
    }
    else {
        script_variables[Position].varname = name;
    }
    script_variables[Position].hashVarnme = CalculateHashOfString(script_variables[Position].varname, strlen(script_variables[Position].varname));
    return 1;
}

/**
 * Allocate a new variable
 * @param name Variable name or NULL
 * @param tag  Type of variable
 * @param arraySize Array Size
 * @param Position Index position of the variable
 * return -1 if error, >=0 if OK
 * */
int addNewScriptVariable(char* name, int tag, int arraySize, int Position) {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS;

    script_variables[Position].sizeOfArray = arraySize;
    if (name == NULL) {
        //We have the name in p_prog->txtString
        int stringLength = strlen(p_prog->txtString);
        char* var_buf = (char*)calloc(sizeof(char) * (stringLength + 1), sizeof(char));
        if (var_buf == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_VAR);
            return quitScriptProgram(1);  //Exit the program
        }

        strncpy(var_buf, p_prog->txtString, stringLength); //We don't need to add '\0' as we uses calloc
        script_variables[Position].varname = var_buf;
        var_buf = NULL;
    }
    else {
        script_variables[Position].varname = name;
    }
    script_variables[Position].hashVarnme = CalculateHashOfString(script_variables[Position].varname, strlen(script_variables[Position].varname));
    switch (tag) {
    case T_INT: {
        script_variables[Position].Value.pi = new int[arraySize];
        if (script_variables[Position].Value.pi == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
            return quitScriptProgram(1);
        }
        script_variables[Position].tag = T_INT;
    }
              break;
    case T_FLOAT: {
        script_variables[Position].Value.pf = new float[arraySize];

        if (script_variables[Position].Value.pf == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
            return quitScriptProgram(1);
        }
        script_variables[Position].tag = T_FLOAT;
    }
                break;
    case T_STRING: {
        script_variables[Position].Value.txtvalue = new char[arraySize];
        if (script_variables[Position].Value.txtvalue == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
            return quitScriptProgram(1);
        }
        script_variables[Position].tag = T_STRING;
    }
                 break;
    }

    if (script_variables[Position].Value.pi == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
        return quitScriptProgram(1);
    }
    return 1;
}
