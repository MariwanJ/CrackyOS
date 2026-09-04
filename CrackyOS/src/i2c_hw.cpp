/**
 ******************************************************************************
 * @file    i2c_hw.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains I2C instructions.
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

#include "i2c_hw.h"

 /** Definition of I2C1*/
I2C_Communication i2c(I2C_SDA, I2C_SCL, I2CUDEFINED);

/**
 * I2C Communication constructor
    *@param SDA      :    Pinname of SDA
    *@param SCL      :    Pinname of SCL
    *@param i2ctype  :    ENUM communication type (0=I2CSLAVE , 1= I2CMASTER, 2= I2CUDEFINED
    */
I2C_Communication::I2C_Communication(PinName SDA, PinName SCL, i2ctype vtype = I2CUDEFINED)
{
    m_type = vtype;
    m_sda = SDA;
    m_scl = SCL;
    m_i2cSlave = NULL;
    m_i2cMaster = NULL;
    m_sendData = NULL;
    m_recieveData = NULL;
    m_address = 0;
}
/**Class Destructor */
I2C_Communication::~I2C_Communication()
{
}
/**
*   This funciton is used to put the address of the slave device
*   @param  add : Adress of the target device
*/
void I2C_Communication::address(int add)
{
    m_address = add;
    if (m_i2cSlave != NULL)
        m_i2cSlave->address(m_address);
}

/** I2C communications type
*  0=I2CSLAVE ,
*  1= I2CMASTER
*  2= I2CUDEFINED
*/
void I2C_Communication::type(i2ctype typ)
{
    m_type = typ;
}
/**
*   I2C Class initializer.
*/
int I2C_Communication::init()
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS;
    // Create the I2C depending on the m_type
    if (m_i2cSlave != NULL) {
        delete m_i2cSlave;
        m_i2cSlave = NULL;
    }
    if (m_i2cMaster != NULL) {
        delete m_i2cMaster;
        m_i2cMaster = NULL;
    }
    switch (m_type) {
    case I2CSLAVE: {
        m_i2cSlave = new I2CSlave(m_sda, m_scl);

        if (m_i2cSlave == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_I2CSLAVE);
            return -1;
        }
        m_i2cSlave->address(m_address);
    }
                 break;
    case I2CMASTER: {
        m_i2cMaster = new I2C(m_sda, m_scl);
        if (m_i2cMaster == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_I2CSLAVE);
            return -1;
        }
    }
                  break;
    }
    return 0;
}

/**
I2C sending data instruction

*    Note: Please notice that we use standard definition of the SDA SCL. This should help
*    in porting the system to other devices :
*
*    I2C_SCL= PB_8
*    I2C_SDA= PB_9
*
*    The syntax of this script is as follow
*                        0     1
*    i2c_send MASTER/SLAVE, address, DATA_tobesend, lengthOfData
*
*    DATA_tobesend : Should be at least 1 byte
*    VARIABLE_recieve_data : Results of running the code.
*/
int token_i2c_send(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS;

    int MasterSlave = 0;
    int varPositionDataSend = -1;
    char data_cmd;
    int msgLength = 0;
    process_token(TOKEN_I2C_SEND);
    char* i2cvar_buf = NULL;;
    data_cmd = 0;
    int address = 0;
    i2cvar_buf = NULL;

    //1) Bring the definition of the I2C communication device Master=0/ Slave=1
    if (current_token_word != TOKEN_NUMBER) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SPI_MASTER_OR_SLAVE_NOT_FOUND);
        return quitScriptProgram(1); //Exit the program.
    }
    MasterSlave = convert_text_to_number();
    process_token(TOKEN_NUMBER);

    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }

    //2) Now we need the address
    if (current_token_word == TOKEN_VARIABLE) {
        address = (int)RetrieveValueInGlobalVariable();
    }
    else if (current_token_word == TOKEN_NUMBER) {
        address = convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_ADDRESS_NOT_FOUND);
        return quitScriptProgram(1);
    }

    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    //3) Now we have either a single command or a variable of array.
    switch (current_token_word) {
    case TOKEN_VARIABLE: {
        varPositionDataSend = TestSystem_get_variable(NULL);
        process_token(TOKEN_VARIABLE);           // Get next token word.
        if (varPositionDataSend == -1) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_DATA_VARIABLE_ARGUMENT_NOT_FOUND);
            return quitScriptProgram(1);
        }
    }
                       break;
    case TOKEN_NUMBER: {
        data_cmd = (char)convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }
                     break;
    }
    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    //4) Now we need the last part before we call i2c functions.
    if (current_token_word == TOKEN_VARIABLE) {
        msgLength = (int)RetrieveValueInGlobalVariable();
    }
    else if (current_token_word == TOKEN_NUMBER) {
        msgLength = convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_DATALENGTH_NOT_FOUND);
        return quitScriptProgram(1);
    }

    // We have everything .. Now we should write i2C
    switch (MasterSlave) {
    case 0: { //We have a master
        i2c.type(I2CMASTER);
        i2c.address(address);
        i2c.init();
        if (varPositionDataSend == -1) {
            i2c.m_i2cMaster->write(address, &data_cmd, 1);        //Only one byte
        }
        else {
            if (script_variables[varPositionDataSend].tag != T_STRING) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_DATALENGTH_NOT_FOUND);
                return quitScriptProgram(1);
            }
            if (msgLength > script_variables[varPositionDataSend].sizeOfArray) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_DATALENGTH_GRATERTHAN_ARRAY);
                return quitScriptProgram(1);
            }
            i2c.m_i2cMaster->write(address, script_variables[varPositionDataSend].Value.txtvalue, msgLength);
        }
    }
          break;

    case 1: { //We have a slave
        i2c.type(I2CSLAVE);
        i2c.address(address);
        i2c.init();
        if (varPositionDataSend == -1) {
            i2c.m_i2cSlave->write(&data_cmd, 1);        //Only one byte
        }
        else {
            if (script_variables[varPositionDataSend].tag != T_STRING) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_DATALENGTH_NOT_FOUND);
                return quitScriptProgram(1);
            }
            if (msgLength > script_variables[varPositionDataSend].sizeOfArray) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_DATALENGTH_GRATERTHAN_ARRAY);
                return quitScriptProgram(1);
            }
            i2c.m_i2cSlave->write(script_variables[varPositionDataSend].Value.txtvalue, msgLength);
        }
    }
          break;
    }

    if (i2cvar_buf != NULL)
        delete[]i2cvar_buf;
    return 0;
}

/**
*   I2C Get data instruction
*   Note: Please notice that we use standard definition of the SDA SCL. This should help
*   in porting the system to other devices :
*
*   I2C_SCL= PB_8
*   I2C_SDA = PB_9
*
*   The syntax of this script is as follow
*              0     1
*   i2c_get MASTER/SLAVE, address, VARIABLE_recieve_data, lengthOfData
*   VARIABLE_recieve_data : Results of running the code.
*/
int token_i2c_get(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS;

    int MasterSlave = 0;
    int varPositionDataReceived = -1;

    int msgLength = 0;

    int address = 0;
    int varPosition = -1;

    process_token(TOKEN_I2C_GET);

    //Bring the definition of the I2C communication device Master=0/ Slave=1
    if (current_token_word != TOKEN_NUMBER) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SPI_MASTER_OR_SLAVE_NOT_FOUND);
        return quitScriptProgram(1); //Exit the program.
    }
    MasterSlave = convert_text_to_number();
    process_token(TOKEN_NUMBER);

    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }

    //Now we need the address --This is needed only when you are master ..slave don't need address.
    if (MasterSlave == 0) {
        if (current_token_word == TOKEN_VARIABLE) {
            address = RetrieveValueInGlobalVariable();
        }
        else if (current_token_word == TOKEN_NUMBER) {
            address = convert_text_to_number();
            process_token(TOKEN_NUMBER);
        }
        else {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_ADDRESS_NOT_FOUND);
            return quitScriptProgram(1);
        }
    }

    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }

    //Now we have either a single byte to read or a variable of array but we must get a variable name.
    if (current_token_word == TOKEN_VARIABLE) {
        char* i2cvar_buf = NULL;;
        int stringLength = strlen(p_prog->txtString);
        i2cvar_buf = (char*)calloc(stringLength + 1, sizeof(char));
        if (i2cvar_buf == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_I2CVAR);
            return quitScriptProgram(1);
        }
        //snprintf(i2cvar_buf, stringLength ,"%s", p_prog->txtString);
        strncpy(i2cvar_buf, p_prog->txtString, stringLength); //We don't need to add '\0' as we uses calloc

        varPositionDataReceived = TestSystem_get_variable(NULL);
        process_token(TOKEN_VARIABLE);           // Get next token word.
        if (varPositionDataReceived != -1) {
            //Found- delete the string/INT/Float inside the variable.
            if (script_variables[varPositionDataReceived].tag != T_STRING) {
                if (script_variables[varPositionDataReceived].tag == T_INT && script_variables[varPositionDataReceived].Value.pi != NULL)
                    delete[] script_variables[varPositionDataReceived].Value.pi;
                else if (script_variables[varPositionDataReceived].tag == T_FLOAT && script_variables[varPositionDataReceived].Value.pf != NULL)
                    delete[] script_variables[varPositionDataReceived].Value.pf;
                script_variables[varPositionDataReceived].Value.pi = NULL;
                script_variables[varPositionDataReceived].Value.pf = NULL;
                script_variables[varPositionDataReceived].tag = T_STRING;
            }
            if (script_variables[varPositionDataReceived].Value.txtvalue!=NULL)
                delete[] script_variables[varPositionDataReceived].Value.txtvalue;
            script_variables[varPositionDataReceived].Value.txtvalue = NULL;
            script_variables[varPositionDataReceived].Value.txtvalue = new char[MAX_READ_BUFF_I2C]();
            if (script_variables[varPositionDataReceived].Value.txtvalue == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_I2C_GET);
                if (i2cvar_buf != NULL)
                    free(i2cvar_buf);
                return quitScriptProgram(1);
            }
            script_variables[varPositionDataReceived].sizeOfArray = MAX_READ_BUFF_I2C;
        }
        else {  //Not found, create a variable.
            varPosition = TestSystem_get_empty_variable_pos();
            if (varPosition < 0) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_OVERFLOW);
                if (i2cvar_buf != NULL) {
                    free(i2cvar_buf);
                    i2cvar_buf = NULL;
                }
                return quitScriptProgram(1);
            }
            /*
             * This is not a bug, the i2cvar_buf will be used in the global
             * variable and the buffer will be deleted when the variable
             * dies.
             * */
            addNewScriptVariable(i2cvar_buf, T_STRING, MAX_READ_BUFF_I2C, varPosition);
        }
        if (i2cvar_buf != NULL)
            free(i2cvar_buf);
        i2cvar_buf = NULL;
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_VAR_MISSING_I2C);
        return quitScriptProgram(1);
    }

    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);
    }
    //Now we need the last part before we call i2c functions.

    if (current_token_word != TOKEN_NUMBER) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_DATALENGTH_NOT_FOUND);
        return quitScriptProgram(1);
    }
    msgLength = convert_text_to_number();
    process_token(TOKEN_NUMBER);
    if (msgLength < 0) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_DATALENGTH_NOT_FOUND);
        return quitScriptProgram(1);
    }

    // We have everything .. Now we should write i2C
    switch (MasterSlave) {
    case 0: { //We have a master
        i2c.type(I2CMASTER);
        i2c.address(address);
        i2c.init();
        if (varPositionDataReceived == -1) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_VAR_NOT_FOUND);
        }
        else {
            if (msgLength > script_variables[varPositionDataReceived].sizeOfArray) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_DATALENGTH_GRATERTHAN_ARRAY);
                return quitScriptProgram(1);
            }
            i2c.m_i2cMaster->read(address, script_variables[varPositionDataReceived].Value.txtvalue, msgLength);
        }
    }
          break;

    case 1: { //We have a slave
        i2c.type(I2CSLAVE);
        i2c.address(address);
        i2c.init();
        if (varPositionDataReceived == -1) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_VAR_NOT_FOUND);
            return quitScriptProgram(1);
        }
        else {
            if (msgLength > script_variables[varPositionDataReceived].sizeOfArray) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_I2C_DATALENGTH_GRATERTHAN_ARRAY);
                return quitScriptProgram(1);
            }
            i2c.m_i2cSlave->read(script_variables[varPositionDataReceived].Value.txtvalue, msgLength);
        }
    }
          break;
    }
    return varPosition;
}
