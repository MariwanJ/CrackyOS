/**
 ******************************************************************************
 * @file    tokenizetofile.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief This file contains per-compiling mechanism functions.
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
#include "tokenizetofile.h"

instr_linkedList* scriptProg = NULL;		//Used in the CrackyOS
instr_linkedList* scriptProgHead = NULL;   //Keep the head
/**
 * add new item to the scriptProg linked list
 * */
void addNewItemToLinkedList()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION
        if (scriptProg == NULL) {
            //We have first Item
            scriptProg = new instr_linkedList;
            if (scriptProg == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_READTOLINKEDLIST);
                quitScriptProgram(1);
            }
            scriptProg->next = NULL;
            scriptProg->prev = NULL;
            scriptProg->txtString = NULL;
            scriptProg->index = 0;
            //scriptProgHead keeps the head
            scriptProgHead = scriptProg;
        }
        else {
            scriptProg->next = new instr_linkedList;
            if (scriptProg->next == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_READTOLINKEDLIST);
                quitScriptProgram(1);
            }
            scriptProg->next->next = NULL;
            scriptProg->next->prev = scriptProg;
            scriptProg->next->txtString = NULL;
            scriptProg = scriptProg->next;
            scriptProg->index = scriptProg->prev->index + 1;
        }
}
TokenizeToFile::TokenizeToFile(const char* inFile, const char* outFile) :
    m_p_prog(NULL),
    m_p_prog_next(NULL),
    testProg(NULL),
    testProgW(NULL),
    inpuFileName(inFile),
    outputFileName(outFile) {
    //Constructor
}
TokenizeToFile::~TokenizeToFile()
{
    //Destructor
}
int TokenizeToFile::checkSingelChar(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        unsigned char testSingle = 1;
    //Check if the char is a single char that is a math or logical operation symbols
    switch (*m_p_prog) {
    case'\r': {
        current_token_word = TOKEN_NEW_LINE;
        if (*m_p_prog == '\n') {
            m_p_prog++; // ignore the \r\n and changed to /n
            m_p_prog_next = m_p_prog;
        }
        break;
    }
    case'\n': {
        current_token_word = TOKEN_NEW_LINE;
        break;
    }
    case '^': {
        current_token_word = TOKEN_CARET;
        break;
    }
    case ':': {
        current_token_word = TOKEN_LABEL;
        m_p_prog_next++;
        while (*m_p_prog_next != ':') {
            if (*m_p_prog_next == '\n' || *m_p_prog_next == '\r' || *m_p_prog_next == '\t' || *m_p_prog_next == ';' || *m_p_prog_next == '\0') {
                current_token_word = TOKEN_BAD;
                return current_token_word;
            }
            m_p_prog_next++;
        }
        m_p_prog_next++;
        return current_token_word;
    }
    case '*': {
        current_token_word = TOKEN_ASTR;
        break;
    }
    case '/': {
        current_token_word = TOKEN_SLASH;
        break;
    }
    case '\\': {
        current_token_word = TOKEN_BACKSLASH;
        break;
    }
    case '%': {
        current_token_word = TOKEN_MOD;
        break;
    }
    case ',': {
        current_token_word = TOKEN_COMMA;
        break;
    }
    case ';': {
        current_token_word = TOKEN_SEMICOLON;
        break;
    }
    case '(': {
        current_token_word = TOKEN_LEFT_PARENTHESIS;
        break;
    }
    case ')': {
        current_token_word = TOKEN_RIGHT_PARENTHESIS;
        break;
    }
    case '{': {
        current_token_word = TOKEN_LEFT_CURLY_BRACKET;
        break;
    }
    case '}': {
        current_token_word = TOKEN_RIGHT_CURLY_BRACKET;
        break;
    }
    case '[': {
        current_token_word = TOKEN_LEFT_SQUARE_BRACKET;
        break;
    }
    case ']': {
        current_token_word = TOKEN_RIGHT_SQUARE_BRACKET;
        break;
    }
    case '#': {
        current_token_word = TOKEN_HASH;
        break;
    }
    case '~': {
        current_token_word = TOKEN_INVERT;
        break;
    }
    case '&': {
        current_token_word = TOKEN_AND;
        break;
    }
    case '=': {
        if (*(m_p_prog + 1) == '=') {
            current_token_word = TOKEN_EQUAL_EQUAL;
            m_p_prog_next = m_p_prog + 1;
        }
        else
            current_token_word = TOKEN_EQUAL;
        break;
    }
    case '<': {
        if (*(m_p_prog + 1) == '<') {
            m_p_prog_next = m_p_prog + 1;         // Two char in fact (<=) .. Add 1 to p_prog_next and p_prog
            current_token_word = TOKEN_SHIFT_LEFT;
        }
        else if (*(m_p_prog + 1) == '=') {
            m_p_prog_next = m_p_prog + 1;         // Two char in fact (<=) .. Add 1 to p_prog_next and p_prog
            current_token_word = TOKEN_LESSTHANEQUAL;
        }
        else
            current_token_word = TOKEN_LESSTHAN;
        break;
    }
    case '>': {
        if (*(m_p_prog + 1) == '>') {
            m_p_prog_next = m_p_prog + 1;         // Two char in fact (>=) .. Add 1 to p_prog_next and p_prog
            current_token_word = TOKEN_SHIFT_RIGHT;
        }
        else if (*(m_p_prog + 1) == '=') {
            m_p_prog_next = m_p_prog + 1;         // Two char in fact (>=) .. Add 1 to p_prog_next and p_prog
            current_token_word = TOKEN_GREATERTHANEQUAL;
        }
        else
            current_token_word = TOKEN_GREATERTHAN;
        break;
    }
    case '!': {
        m_p_prog_next = m_p_prog + 1;
        // Two char in fact (!=) .. Add 1 to p_prog_next and p_prog
        if ((*m_p_prog_next) != '=') {
            //error

            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_NEEDS_EQUAL_SIGN);
            current_token_word = TOKEN_BAD;
            return -1; //error
        }
        else {
            current_token_word = TOKEN_NOTEQUAL;
            m_p_prog_next = m_p_prog + 1;
        }
        break;
    }

    case '+': {
        if (*(m_p_prog + 1) == '+') {
            current_token_word = TOKEN_PLUSPLUS;
            m_p_prog_next = m_p_prog + 1;
        }
        else
            current_token_word = TOKEN_PLUS;
        break;
    }
    case '-': {
        if (*(m_p_prog + 1) == '-') {
            current_token_word = TOKEN_MINUSMINUS;
            m_p_prog_next = m_p_prog + 1;
        }
        else
            current_token_word = TOKEN_MINUS;
        break;
    }

    case '|': {
        m_p_prog_next = m_p_prog + 1;
        // Two char in fact (||) .. Add 1 to p_prog_next and p_prog
        if ((*m_p_prog_next) != '|') {
            //error
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_DOUBLE_OR_SYMBOL_REQUIRED);
            current_token_word = TOKEN_BAD;
            return -1; //error
        }
        else {
            current_token_word = TOKEN_OR;
            m_p_prog_next = m_p_prog + 1;
        }
        break;
    }

    default: {
        testSingle = 0; // Not found / Not single char  break; // do nothing.
    }
    } //End of switch - end of single special char.

    if (testSingle == 1) {
        // We found single char and we need to move the next pointer by one char
        m_p_prog_next++;
        return current_token_word;
    }
    return -1;
}
int TokenizeToFile::checkNumber(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        //Now we have to check if the token is digits (numbers) : Dec, Hex, bin are supported.
        if (isdigit(*m_p_prog)) {
            while (!endOfNumber(m_p_prog_next)) {
                m_p_prog_next++;
            }
            current_token_word = TOKEN_NUMBER;
            return current_token_word;
        }
    return -1;
}
int TokenizeToFile::checkString(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        // Look for string
        if (*m_p_prog == '\"') {
            //   FOUND STRING
            m_p_prog_next = m_p_prog + 1;
            while (*m_p_prog_next != '\"' && *m_p_prog_next != '\0') {
                //Seek to the end of the string.
                m_p_prog_next++;
            }
            if (*m_p_prog_next == '\0') {
                //BAD String
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_DBOULEQUATIOAN_REQUIRED);
                current_token_word = TOKEN_BAD;
                return -1;
            }
            m_p_prog_next++; // Go to the next char after the string.
            current_token_word = TOKEN_STRING;
            return current_token_word;
        } //End of looking for string
    return -1;
}
int TokenizeToFile::checkInstruction(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        //Looking for instruction set, GPIO commands in the program.
        //We should find here the commands that could be found in the  (instruction_set)
        // Add or delete instruction set in that variable.
        static const token_exchanger* p_instruction_set;
    unsigned int token_length = 0;        // Will be used to calculate the token_word length for avoiding mix between words like TOKEN_PA_11 and TOKEN_PA_1
    //Find the length of the token.
    token_length = 1;
    while (isSpecialChar(m_p_prog + token_length) != 1 && (*(m_p_prog + token_length) != ' ')) {
        token_length++;
    }

    uint32_t  saveHasheValue = CalculateHashOfString(m_p_prog, token_length);
    for (p_instruction_set = token_exchange_list; p_instruction_set->token_word != NULL; p_instruction_set++) {
        /*
       We get a problem when we take a GPIO like TOKEN_PA_11 and TOKEN_PA_1 as the first one could be counted as the second one.
       To avoid that we have to calculate a correct length
       */
        if (saveHasheValue == p_instruction_set->hash) {
            m_p_prog_next = m_p_prog + strlen(p_instruction_set->token_word);
            current_token_word = p_instruction_set->token_enum;
            return current_token_word;      // We found the instruction .. Return the token.
        //}
        }
    }//End looking for instruction set and GPIO
    return -1;
}
int TokenizeToFile::checkVariable(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        //Here, we checked everything.. If we get letters .. they are variables ..
        //everything else .. we should generate error
        if (*m_p_prog >= 'a' && *m_p_prog <= 'z') {
            m_p_prog_next = m_p_prog;
            while (!(*(m_p_prog_next) == ' ' || *(m_p_prog_next) == '\0' || isSpecialChar(m_p_prog_next))) {
                // DEBUG_MESSAGE("Skip all letters/bad string/instruction");
                m_p_prog_next++; // Skip all char
            }
            current_token_word = TOKEN_VARIABLE;
            //  VARIABLE OR BAD CODE
            return current_token_word;
        }
    return -1;
}
/**
 * This function will analyze the script file and generate a new tokinized file.
 * It allows the system to remove all '\r' also to minimize mem usage.
 * */
int TokenizeToFile::run()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        int result = TOKEN_BAD;
    testProg = testProgW = NULL;
    testProg = fopen(inpuFileName, "r");     //Open for reading.
    testProgW = fopen(outputFileName, "w+");     //Open for writing.
    int strlength = 0;
    char pTokenBuffer[MAX_TOKEN_SIZE];    //This will be used to hold the token text and later save it to the disk.
    char pTokenNumber[22];     //TODO: Do we need so big buffer??? \n & \0 also must be counted
    if (testProg == NULL || testProgW == NULL) {
        SERIAL_MESSAGE("default prog=%s not found\n", inpuFileName);
        if (testProg != NULL) {
            fclose(testProg);
            testProg = NULL;
        }
        if (testProgW != NULL) {
            fclose(testProgW);
            testProgW = NULL;
        }

        return quitScriptProgram(1);
    }
    char program_test[MAX_LINE_SIZE];
    memset(program_test, '\0', MAX_LINE_SIZE);
    if (testProg == 0) {
        return quitScriptProgram(1);
    }
    while (fgets(program_test, MAX_LINE_SIZE, testProg) != NULL) {
        //We have a new line from the script_program file ..process it.
        change_program_to_lower_case(program_test);
        m_p_prog = m_p_prog_next = program_test;
        while (*m_p_prog != '\0') {
            //old part
            m_p_prog = m_p_prog_next;
            result = TOKEN_BAD;

            //Skip space char, and tab
            while ((*m_p_prog == ' ') || (*m_p_prog == '\t')) {
                m_p_prog++;
            }

            m_p_prog_next = m_p_prog;
            if (*m_p_prog != '\0') {
                result = checkNumber();      //Check if it is a number
                if (result == -1) {
                    result = checkSingelChar();  //Check if it is a single char
                    if (result == -1) {
                        result = checkString();      //Check if it is a string
                        if (result == -1) {
                            result = checkInstruction(); //Check if it is an instruction
                            if (result == -1) {
                                result = checkVariable();     //check variable.
                                if (result == -1) {
                                    result = TOKEN_BAD;
                                }
                            }
                        }
                    }
                }
                if (result != TOKEN_BAD) {
                    // Here we should save the result to the SD-Disk .. We checked the text, we know the equivalent enum, and we have the text, save enum, and later the text
                    /* Here we should save the result to the SD-Disk .. We checked the text, we know the equivalent enum, and we have the text, save enum, and later the text
                    *  Note: TOKEN_REM is not saved to .dad .. nothing .. That line is just skipped.
                    */
                    if (result != TOKEN_REM) {
                        //Skip REM .. dont save it.
                        memset(pTokenNumber, '\0', MAX_INT_SIZE);//Reset the buffer;
                        snprintf(pTokenNumber, MAX_INT_SIZE - 1, "0x%x\n", result);
                        fputs(pTokenNumber, testProgW); //save the token enum as text number -- you need to convert it to number again.
                        // Here we should save the result to the SD-Disk ..
                        memset(pTokenBuffer, '\0', MAX_TOKEN_SIZE);//Reset the buffer;
                        strlength = (m_p_prog_next - m_p_prog) + 1;
                        if ((strlength - 1) > MAX_TOKEN_SIZE) {
                            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_TOKEN_SIZE_IS_BIG);
                            quitScriptProgram(1);
                        }
                    }
                    if (result == TOKEN_REM) {
                        m_p_prog = m_p_prog_next;
                        //Continue to take the pointer to the end of the line. Only new line is counted as the end.. semicolon cannot be used here.
                        while (*m_p_prog_next != '\n' && *m_p_prog_next != '\0') //This means that the REM should be a separate line in the program. Any text after REM will be counted as REM body if you don't put \n
                            m_p_prog_next++;
                        while (*m_p_prog == ' ')
                            m_p_prog++;        //eat the space before the start of the remark-word(s)
                    }
                    else if (result == TOKEN_LABEL) {
                        strncpy(pTokenBuffer, (m_p_prog + 1), strlength - 2); //We don't need to add '\0' as we uses calloc

                        pTokenBuffer[strlength - 3] = '\n';
                        fputs(pTokenBuffer, testProgW); //save the text of the token
                    }
                    else {
                        strncpy(pTokenBuffer, m_p_prog, strlength); //We don't need to add '\0' as we uses calloc
                        pTokenBuffer[strlength - 1] = '\n';
                        fputs(pTokenBuffer, testProgW); //save the text of the token
                    }
                }
                else {
                    // TODO : WHY ARE WE HERE ???2020-02-27   TOKEN_BAD IS HERE
                    //We shouldn't be here .. ERROR -I don't know when this happen?
                    memset(pTokenNumber, '\0', MAX_INT_SIZE);//Reset the buffer;

                    fputs(pTokenNumber, testProgW); //save the token enum as text number -- you need to convert it to number again.

                    memset(pTokenNumber, '\0', MAX_INT_SIZE);//Reset the buffer;
                    snprintf(pTokenNumber, MAX_INT_SIZE - 1, "%s", "BAD"); //TODO  this should be TOKEN_BAD. WRONG 2020-05-11
                    fputs(pTokenNumber, testProgW); //save the token enum as text number -- you need to convert it to number again.

                    if (*m_p_prog_next != '\0')
                        m_p_prog_next++; //We need to skip one char..
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_FATAL_WE_SHOULDNT_BE_HERE2);
                }
            }
        }
        memset(program_test, '\0', MAX_LINE_SIZE); //Re-initialize the variable
    }

    //At the end of the file, we MUST have something.. we use EOF
    result = TOKEN_EOF;
    memset(pTokenNumber, '\0', MAX_INT_SIZE);//Reset the buffer;
    snprintf(pTokenNumber, MAX_INT_SIZE - 1, "0x%x\n", result);
    fputs(pTokenNumber, testProgW); //save the token enum as text number -- you need to convert it to number again.
    // Here we should save the result to the SD-Disk ..
    memset(pTokenBuffer, '\0', MAX_TOKEN_SIZE);//Reset the buffer;
    strlength = 5;
    snprintf(pTokenBuffer, 5, "%s\n", "eof");
    fputs(pTokenBuffer, testProgW); //save the token text

    fclose(testProgW);
    fclose(testProg);
    return 1;
}

/** Load from the file the linked list which contains the code
 *
 * @param filename A file name to load the tokinized instructions.
 * @param lines Number of lines to load.
 * */
int readTolinkedList(const char* filename, unsigned int lines)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        FILE* fp = NULL;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_FORTEMPBUF);
        quitScriptProgram(1);
    }
    char tempBuf[MAX_TOKEN_SIZE];
    /*    if(scriptProg!=NULL){
            freeListedLink();
        }

       */
    for (unsigned int i = 0; i < lines; i++) {
        addNewItemToLinkedList(); //Create a new item and fix the link
        if (fgets(tempBuf, MAX_TOKEN_SIZE - 1, fp) != NULL) {
            int tokenSize = strlen(tempBuf);
            scriptProg->txtString = (char*)new char[tokenSize + 1];
            if (scriptProg->txtString == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
                quitScriptProgram(1);
            }
            memset(scriptProg->txtString, '\0', tokenSize + 1);
            /**
             * We should avoid having \n in all token words and strings we save in memory.
             * We should have it in the .dad file but, not in the memory. 2020-03-05
             * */
            if (tokenSize > 1)
                strncpy(scriptProg->txtString, tempBuf, tokenSize - 1); //We don't need to add '\0' as we uses memset, we skip all \n
            else
                scriptProg->txtString[0] = tempBuf[0]; //    We heave only 1 char .. it must be '\n'
        }
        memset(tempBuf, '\0', MAX_TOKEN_SIZE);
    }
    fclose(fp);
    if (*scriptProg->txtString == '\0') {
        //Last item shouldn't be there .
        scriptProg = scriptProg->prev;
        delete scriptProg->next;
        scriptProg->next = NULL;
    }
    p_prog = scriptProgHead;
    return 0;
}

/**
 * This function will count the number of lines available for the given filename
 * @param filename A filename
 * @return unsigned int value represent the total line numbers.
 * */
unsigned int countlines(const char* filename)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_NULL

        // count the number of lines in the file called filename
        FILE* fp = NULL;
    int ch = 0;
    unsigned int lines = 0;
    fp = fopen(filename, "r");

    if (fp == 0) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_CANNOT_OPEN_DAD_FILE);
        return 0;
    }
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n')
            lines++;
    }
    fclose(fp);
    return lines;
}

/** Remove the linked list items from memory*/
void free_scriptProg(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION

    scriptProg = scriptProgHead;
    if (scriptProg != 0) {
        while (scriptProg->next != NULL) {
            instr_linkedList* old = scriptProg;
            scriptProg = scriptProg->next;
            scriptProg->prev = NULL;
            delete[] old->txtString;
            delete old;
            old = NULL;
        }
        delete scriptProg;
        scriptProg = NULL;
        scriptProgHead = NULL;
    }
}

/** This will tokinize a string (not a file) in the interactive mode
 * tokinize a string given to the function. This is used only in the interactive mode.
 * @param inputCommand A pointer points to the buffer containing the instructions given to the system
 * via the terminal.
 * @return -1 if error, return >=0 if OK
 * */
int TokenizeToFile::runInteractive(char* inputCommand)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

    int result = TOKEN_BAD;
    int strlength = 0;
    char pTokenBuffer[MAX_TOKEN_SIZE];    //This will be used to hold the token text and later save it to the disk.
    char pTokenNumber[22];     //TODO: Do we need so big buffer??? \n & \0 also must be counted

    if (inputCommand == NULL) {
        SERIAL_MESSAGE("ERROR:interactive mode didn't get any command\n", -1);
        return -1;   // we must return -1 .. so interactive mode remove the class.
    }
    if (scriptProg != NULL) {
        //When we have the scriptProgram from another round, we need to add more items only. No need to remove the old one .. and we must be at the end
        while (scriptProg->next != NULL)
            scriptProg = scriptProg->next;
        // Remove EOF as we will continue adding instructions to the old one.
        if (strcmp(scriptProg->txtString, "eof\n") == 0) {
            scriptProg = scriptProg->prev;
            delete[] scriptProg->next->txtString;//Free the text first
            scriptProg->next->txtString = NULL;
            delete scriptProg->next;             //No need anymore for the list
            scriptProg->next = NULL;
            delete[] scriptProg->txtString;
            scriptProg->txtString = NULL;         // Remove the enum of the text
            scriptProg->next = NULL;              //fix the link.
        }
    }
    else {
        // There wasn't any item in the p_prog .. create the first one.
        addNewItemToLinkedList(); //Create a new item and fix the link
    }
    scriptProgHead = scriptProg;
    char* program_test = inputCommand;
    //We have a new line from the script_program file ..process it.
    change_program_to_lower_case(program_test);
    m_p_prog = m_p_prog_next = program_test;
    //Main loop to process the input string.

    while (*m_p_prog != '\0') {
        //old part
        m_p_prog = m_p_prog_next;
        result = TOKEN_BAD;

        //Skip space char, and tabs
        while ((*m_p_prog == ' ') || (*m_p_prog == '\t')) {
            m_p_prog++;
        }
        m_p_prog_next = m_p_prog;
        if (*m_p_prog != '\0') {
            result = checkNumber();      //Check if it is a number
            if (result == -1) {
                result = checkSingelChar();  //Check if it is a single char
                if (result == -1) {
                    result = checkString();      //Check if it is a string
                    if (result == -1) {
                        result = checkInstruction(); //Check if it is an instruction
                        if (result == -1) {
                            result = checkVariable();     //check variable.
                            if (result == -1) {
                                result = TOKEN_BAD;
                            }
                        }
                    }
                }
            }

            //One word is scanned and result is found
            if (result != TOKEN_BAD) {
                // Here we should save the results to the memory link-list .. We checked the text, we know the equivalent enum, and we have the text, save enum, and later the text
                scriptProg->txtString = (char*)calloc(MAX_INT_SIZE, sizeof(char));

                if (scriptProg->txtString == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_TOKENIZER);
                    quitScriptProgram(1);
                }
                // Save the ENUM first
                snprintf(scriptProg->txtString, MAX_INT_SIZE - 1, "0x%x", result);

                //We saved the ENUM .. Take the text now
                addNewItemToLinkedList(); //Create a new item and fix the link

               // Here we should save the text of the result -enum ...
                memset(pTokenBuffer, '\0', MAX_TOKEN_SIZE);//Reset the buffer;
                strlength = (m_p_prog_next - m_p_prog) + 1;
                if ((strlength - 1) > MAX_TOKEN_SIZE) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_TOKEN_SIZE_IS_BIG);
                    return -1;   // we must return -1 .. so interactive mode remove the class.
                }
                switch (result) {
                case TOKEN_LABEL: {
                    strncpy(pTokenBuffer, (m_p_prog + 1), strlength - 1); //We don't need to add '\0' as we uses calloc
                    pTokenBuffer[strlength - 3] = '\0';
                    break;
                }
                case TOKEN_REM: {
                    m_p_prog = m_p_prog_next;
                    //Continue to take the pointer to the end of the line or to the end of the command line (might be a semicolon)
                    while (*m_p_prog_next != '\n' && *m_p_prog_next != '\0')  //This means that the REM should be a separate line in the program. Any text after REM will be counted as REM body if you don't put \n
                        m_p_prog_next++;
                    while (*m_p_prog == ' ')
                        m_p_prog++;        //eat the space before the start of the remark-word(s)
                    strlength = (m_p_prog_next - m_p_prog) + 1;
                    if ((strlength - 1) > MAX_TOKEN_SIZE) {
                        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_TOKEN_SIZE_IS_BIG);
                        quitScriptProgram(1);
                    }
                    //Save the text of the token as second parameter for each token in the list
                    strncpy(pTokenBuffer, m_p_prog, strlength); //We don't need to add '\0' as we uses calloc

                    pTokenBuffer[strlength - 1] = '\0';
                    break;
                }
                default: {
                    strncpy(pTokenBuffer, m_p_prog, strlength); //We don't need to add '\0' as we uses calloc
                    pTokenBuffer[strlength - 1] = '\0';
                }
                }

                strlength = strlen(pTokenBuffer) * sizeof(char) + 1;
                scriptProg->txtString = (char*)calloc(strlength, sizeof(char));
                if (scriptProg->txtString == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_TOKENIZER);
                    quitScriptProgram(1);
                }
                strncpy(scriptProg->txtString, pTokenBuffer, strlength); //We don't need to add '\0' as we uses calloc

                //Create new listedlink for the next item (ENUM)
                addNewItemToLinkedList(); //Create a new item and fix the link
            }
            else {
                // TODO : WHY ARE WE HERE ???
                //We shouldn't be here .. ERROR -I don't know when this happen?
                scriptProg->txtString = (char*)calloc(MAX_INT_SIZE, sizeof(char));
                if (scriptProg->txtString == NULL) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_TOKENIZER);
                    return -1;   // we must return -1 .. so interactive mode remove the class.
                }
                snprintf(scriptProg->txtString, MAX_INT_SIZE - 1, "0x%x", TOKEN_BAD); //changed to avoid having '\n 2020-03-05
                addNewItemToLinkedList(); //Create a new item and fix the link //Create a new item and fix the link

                scriptProg->txtString = (char*)calloc(12, sizeof(char));
                snprintf(scriptProg->txtString, 11, "%s", "token_bad");
                addNewItemToLinkedList(); //Create a new item and fix the link //Create a new item and fix the link
                if (*m_p_prog_next != '\0')
                    m_p_prog_next++; //We need to skip one char..
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_FATAL_WE_SHOULDNT_BE_HERE3);
            }
        }
    }
    //At the end of the file, we MUST have something.. we use EOF

    result = TOKEN_EOF;
    memset(pTokenNumber, '\0', MAX_INT_SIZE);//Reset the buffer;
    snprintf(pTokenNumber, MAX_INT_SIZE - 1, "0x%x", result);    //

    scriptProg->txtString = (char*)calloc(MAX_INT_SIZE, sizeof(char));
    if (scriptProg->txtString == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_TOKENIZER);
        quitScriptProgram(1);
    }
    snprintf(scriptProg->txtString, MAX_INT_SIZE - 1, "0x%x", result);

    addNewItemToLinkedList(); //Create a new item and fix the link
    strlength = 5;
    scriptProg->txtString = (char*)calloc(strlength, sizeof(char));
    if (scriptProg->txtString == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_TOKENIZER);
        quitScriptProgram(1);
    }
    strncpy(scriptProg->txtString, "eof\0", 4);
    token_jump_to(scriptProgHead);   //Point the pointer to the beginning of the program.
    return 0;
}
