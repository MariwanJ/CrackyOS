/**
 ******************************************************************************
 * @file    sourcecodeline.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief This file contains line-code mechanism functions.
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
#include "sourcecodeline.h"

SourceCodeLine::SourceCodeLine() :
    m_TableOfPointer(NULL), m_psourceCode(NULL), mfirst(NULL)
{
}

SourceCodeLine::SourceCodeLine(instr_linkedList* pSourceCodeText) :
    m_TableOfPointer(NULL), m_psourceCode(pSourceCodeText), mfirst(NULL)
{
}

SourceCodeLine::~SourceCodeLine()
{
    if (mfirst == NULL)
        return; //Nothing to do .
    while (mfirst->next != NULL) {
        mfirst = mfirst->next;
        delete (mfirst->prev);
        mfirst->prev = NULL;
    }
    delete (mfirst);
    mfirst = NULL;
}
void SourceCodeLine::addNewLine(instr_linkedList* pPosition)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION;
    if (m_TableOfPointer == NULL) {
        m_TableOfPointer = new listObj;
        if (m_TableOfPointer == NULL) {
            DEBUG_MESSAGE("Error %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_TABLEOFPOINTER);
            quitScriptProgram(1);//Exit the program
        }
        m_TableOfPointer->prev = NULL;    //Point the new item to the old item.
        m_TableOfPointer->next = NULL;                //Point the next of the new item to Null
        m_TableOfPointer->lineNo = 1;
        m_TableOfPointer->pLineStart = pPosition;
        mfirst = m_TableOfPointer;
    }
    else {
        m_TableOfPointer->next = new listObj;
        if (m_TableOfPointer->next == NULL) {
            DEBUG_MESSAGE("Error %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_TABLEOFPOINTER);
            quitScriptProgram(1);//Exit the program
        }
        m_TableOfPointer->next->prev = m_TableOfPointer;    //Point the new item to the old item.
        m_TableOfPointer->next->next = NULL;                //Point the next of the new item to Null
        m_TableOfPointer = m_TableOfPointer->next;          //Change the current pointer to the created one
        m_TableOfPointer->lineNo = m_TableOfPointer->prev->lineNo + 1;  //Add one to the previous line number
        m_TableOfPointer->pLineStart = pPosition;
    }
}

//This function will return a pointer to the list element. You can use it to get line number of printf the line
listObj* SourceCodeLine::findPointer(instr_linkedList* pCurrent)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_NULL

        if (pCurrent == NULL || current_token_word == TOKEN_EOF)
            return NULL;
    listObj* pTempSearch = NULL;;
    pTempSearch = mfirst;
    if (pTempSearch == NULL) {
        return NULL;    //We shoulnd't be here
    }
    do {
        if (pTempSearch->next != NULL && pCurrent >= pTempSearch->pLineStart && pCurrent <= pTempSearch->next->pLineStart) {
            //The pointer is inside this line .. return the first position of the len
            return pTempSearch;
        }
        pTempSearch = pTempSearch->next;
    } while (pTempSearch->next != NULL);
    return NULL;    //We shoulnd't be here
}

void SourceCodeLine::scanSourceCode()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION
        // if no program given to the scanSourceCode go out.
        if (m_psourceCode == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_NO_PROGRAM);
            return;
        }
    addNewLine(p_prog->prev);  //Save the first line
    do {
        if (current_token_word == TOKEN_NEW_LINE) {
            check_and_get_next_token();
            //We have found the new line .. add them to the list
            addNewLine(p_prog->prev);  //Save the line and continue scanning the script program
        }
        check_and_get_next_token();
    } while (current_token_word != TOKEN_EOF);
    p_prog = m_psourceCode;
}
/**
 * Remove the linked list that save the line number.
 * */
void freepScriptIndex() {
    if (!(DebugSerialFIFO.checkEmpty())) {
        rtos::ThisThread::sleep_for(chrono::milliseconds(10));
    }
    delete pScriptIndex;
    pScriptIndex = NULL;
}