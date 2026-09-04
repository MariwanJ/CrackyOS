/**
 ******************************************************************************
 * @file    spi_hw.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains SPI instructions.
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

#include "spi_hw.h"

 /** sd variable definition . SPI 1 is used for SD-Card*/
SDBlockDevice sd(SPI_1_MOSI, SPI_1_MISO, SPI_1_SCK, SPI_1_CS);  //This varies depending on the HW

/*
 *  SPI communication syntax. This device works only as Master

  DecoderGPIO select the SPI device connected to the system.
  Device 0000 is reserved for SD_Disc.
*/

/** DecoderGPIOclass class
 *
 *  DecoderGPIOclass class Implementation
 *
 */
DecoderGPIOclass::DecoderGPIOclass(PinName p3, PinName p2, PinName p1, PinName p0) :
Line3(NULL),
Line2(NULL),
Line1(NULL),
Line0(NULL)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION

        Line0 = new DigitalOut(p0);
    Line1 = new DigitalOut(p1);
    Line2 = new DigitalOut(p2);
    Line3 = new DigitalOut(p3);
    if (Line0 == NULL || Line1 == NULL || Line2 == NULL || Line3 == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_CONCAT_FUNCTION);
        quitScriptProgram(1);
    }
}
DecoderGPIOclass::~DecoderGPIOclass() {
    if (Line0)  delete Line0;
    if (Line1)  delete Line1;
    if (Line2)  delete Line2;
    if (Line3)  delete Line3;
    Line0 = Line1 = Line2 = Line3 = NULL;
}

void DecoderGPIOclass::select(unsigned char v3, unsigned char v2, unsigned char v1, unsigned char v0) {
    //Stop running if we have error
    STOP_CODE_EXECUTION
        * Line0 = v0;
    *Line1 = v1;
    *Line2 = v2;
    *Line3 = v3;
}
SPI MasterSPI_Device(SPI_1_MOSI, SPI_1_MISO, SPI_1_SCK);
DigitalOut Master_SPI_cs(SPI_1_CS);

//This variable will be used when data sends or receives from the SIP device.
//spispecialVar spiSendGlobalVar, spiReceiveGlobalVar;

DecoderGPIOclass DecoderGPIO(LINE3, LINE2, LINE1, LINE0);

void SendReceiveSPI_Device(unsigned char DEVICE_ID,
    int numberOfBytesWillBeReturned,        // Do the slave sends us any value? //Notice that if the slave return 8 bits, we should AND the result with 0x00FF for each int value.
    unsigned int sendDataToSlave,           //  Data to be sent to the slave.
    int globalVariableData,                 //  Global variable has got the data if the pointer is not -1. This will be applicable when we have an array to send.
    int globalVariableResults,              //  This should be a position if we need results or -1 if we don't need results.
    spiDataSize DataSize,                   //  Data size or type -- 8,12,16
    int spi_frequency,                      //  SPI channel frequency
    int spi_mode,                           //  SPI Mode - Default is 0
    int DelayBetweenPackets) {               // Delay between packets .. Default=0;
//Stop running if we have error
    STOP_CODE_EXECUTION

        int counter = 0;

    int dataSend = 0;
    ///SPI communication function.
    select_spi_channel(DEVICE_ID);
    MasterSPI_Device.format((int)DataSize, spi_mode);
    MasterSPI_Device.frequency(spi_frequency);

    if (numberOfBytesWillBeReturned < 0)
        return;

    //Note:: numberOfBytesWillBeReturned cannot be less than returned bytes by the slave

    // Prepare the global variable for receiving data. Remove old arrays.
    if (globalVariableResults != -1) {
        if (script_variables[globalVariableResults].tag != T_INT) {
            if (script_variables[globalVariableResults].tag == T_FLOAT) {
                if (script_variables[globalVariableResults].Value.pf != 0) {
                    delete[] script_variables[globalVariableResults].Value.pf;
                    script_variables[globalVariableResults].Value.pf = NULL;
                }
            }
            else {
                if (script_variables[globalVariableResults].Value.txtvalue != NULL) {
                    delete[] script_variables[globalVariableResults].Value.txtvalue;
                    script_variables[globalVariableResults].Value.txtvalue = NULL;
                }
            }

            script_variables[globalVariableResults].tag = T_INT;
        } {
            if (script_variables[globalVariableResults].Value.pi != NULL) {
                delete[] script_variables[globalVariableResults].Value.pi;
                script_variables[globalVariableResults].Value.pi = NULL;
            }
        }
        script_variables[globalVariableResults].Value.pi = new int[numberOfBytesWillBeReturned];
        if (script_variables[globalVariableResults].Value.pi == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
            quitScriptProgram(1);
        }
        script_variables[globalVariableResults].sizeOfArray = numberOfBytesWillBeReturned;
    }
    //Check if we have an array to send or just only two bytes/one bytes.
    if (globalVariableData == -1) {
        //We have only one word/byte to sent to slave.
        if (DataSize == SPI_8BIT) {
            dataSend = (0xFF) & sendDataToSlave;    // We need only 8 bits..
        }
        else if (DataSize == SPI_12BIT) {
            dataSend = (0x0FFF) & sendDataToSlave;   // We need only 8 bits..
        }
        Master_SPI_cs = 1;  //Should be HIGH before going LOW
        Master_SPI_cs = 0;  //Should be HIGH before going LOW
        MasterSPI_Device.write(dataSend);    //First reply is just dummy byte. but we send the command/data to the salve
        Master_SPI_cs = 1;  //Should be HIGH before going LOW
        if (globalVariableResults != -1) {
            //If we have results .. get them.
            for (counter = 0; counter < numberOfBytesWillBeReturned; counter++) {
                Master_SPI_cs = 0;  //Should be HIGH before going LOW
                script_variables[globalVariableResults].Value.pi[counter] = MasterSPI_Device.write(SPI_DUMMY); //Send dummy bytes to get results from slave.
                Master_SPI_cs = 1;  //Should be HIGH before going LOW
                if (DelayBetweenPackets != 0) {
                    wait_us(DelayBetweenPackets);
                }
            }
        }
    }
    else {
        //TODO : This part needs to be tested. I don't know if it is correct. 2021-20-16

        //We have an array to send to the slave. The array sends in continuous mode without changing CS
        int ResultCounter = 0;        //This will be counting the no. of received data.
        if (script_variables[globalVariableData].tag == T_FLOAT) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SPI_FLOAT_NOT_SUPPOIRTED);
            quitScriptProgram(1);
        }
        Master_SPI_cs = 1;  //Should be HIGH before going LOW
        Master_SPI_cs = 0;  //Should be HIGH before going LOW
       //Make a loop depending on the amount of data we received and should be send to the slave.
        for (counter = 0; counter < script_variables[globalVariableData].sizeOfArray; counter++) {
            //Two kinds of data are allowed .. Either string, or INT. When it is string, depending on packet size we have to do some math.
            if (script_variables[globalVariableData].tag == T_INT) {
                if (DataSize == SPI_8BIT) {
                    dataSend = (0xFF) & script_variables[globalVariableData].Value.pi[counter];    // We need only 8 bits..
                }
                else if (DataSize == SPI_12BIT) {
                    dataSend = (0x0FFF) & script_variables[globalVariableData].Value.pi[counter];   // We need only 12 bits..
                }
            }
            else {
                //We have a string .. so treat it correctly.
                Master_SPI_cs = 0;  //Cycling the CS is essential for text array.
                if (DataSize == SPI_8BIT) {
                    dataSend = (0xFF) & script_variables[globalVariableData].Value.txtvalue[counter];    // We need only 8 bits..
                }
                else if (DataSize == SPI_12BIT) {
                    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SPI_12BIT_STRING_NOT_SUPPORTED);
                    quitScriptProgram(1); //Exit the program.
                }
                else if (DataSize == SPI_16BIT) {
                    //Since strings are always 1 byte, we have to send to the slave 2 char per each transmission. SLAVE MUST KNOW THAT!!! 2018-07-26
                    dataSend = (0xFF) & script_variables[globalVariableData].Value.txtvalue[counter];    // We need only 8 bits..
                    if (counter + 1 < script_variables[globalVariableData].sizeOfArray) {
                        counter++;
                        dataSend = dataSend + (((0xFF) & script_variables[globalVariableData].Value.txtvalue[counter]) << 8);    // We need only 8 bits..
                    }
                }
            }
            if (counter == 0 || numberOfBytesWillBeReturned == 0 || ResultCounter >= numberOfBytesWillBeReturned) {
                //First reply is always a dummy byte ..skip it. If we don't return any value, also skip saving reply from slave.
                MasterSPI_Device.write(dataSend); //Send command, get a  reply.
            }
            else {
                script_variables[globalVariableResults].Value.pi[ResultCounter] = MasterSPI_Device.write(dataSend); //Send command, get a  reply.
                //ResultCounter++; Only one byte should be saved .. it is the last one.
            }
            //String array should be treated differently to int array. We send either one byte or two byte per each CS cyckling.
            if (script_variables[globalVariableData].tag == T_STRING) {
                Master_SPI_cs = 1;
            }
            if (DelayBetweenPackets != 0) {
                wait_us(DelayBetweenPackets);
            }

            Master_SPI_cs = 1;  //Should be HIGH before going LOW. We have sent all bytes in one CS cycle.
            ResultCounter++; // We are done.
            //TODO : Do we need this??
            if (ResultCounter < numberOfBytesWillBeReturned) {
                //For getting the last byte from the slave, we have to send a dummy byte.
                Master_SPI_cs = 0;  //Should be HIGH before going LOW
                script_variables[globalVariableResults].Value.pi[ResultCounter] = MasterSPI_Device.write(SPI_DUMMY); //Send command, get a  reply.
                ResultCounter++;
                Master_SPI_cs = 1;  //Should be HIGH before going LOW
            }
        }
    }
    return;
}

void select_spi_channel(unsigned char channelNo) {
    //Stop running if we have error
    STOP_CODE_EXECUTION

        switch (channelNo) {
        case  0: {DecoderGPIO.select(0, 0, 0, 0); break; }  //SPI Device 0: Reserved for SD-Card.
        case  1: {DecoderGPIO.select(0, 0, 0, 1); break; }
        case  2: {DecoderGPIO.select(0, 0, 1, 0); break; }
        case  3: {DecoderGPIO.select(0, 0, 1, 1); break; }
        case  4: {DecoderGPIO.select(0, 1, 0, 0); break; }
        case  5: {DecoderGPIO.select(0, 1, 0, 1); break; }
        case  6: {DecoderGPIO.select(0, 1, 1, 0); break; }
        case  7: {DecoderGPIO.select(0, 1, 1, 1); break; }
        case  8: {DecoderGPIO.select(1, 0, 0, 0); break; }
        case  9: {DecoderGPIO.select(1, 0, 0, 1); break; }
        case 10: {DecoderGPIO.select(1, 0, 1, 0); break; }
        case 11: {DecoderGPIO.select(1, 0, 1, 1); break; }
        case 12: {DecoderGPIO.select(1, 1, 0, 0); break; }
        case 13: {DecoderGPIO.select(1, 1, 0, 1); break; }
        case 14: {DecoderGPIO.select(1, 1, 1, 0); break; }
        case 15: {DecoderGPIO.select(1, 1, 1, 1); break; }
        };
}

/**
*     SPI data send instruction
*     The syntax of this script is as follow:
*     spi DEVICE_ID, WaitForReply(returned bytes), SPI_DATA (could be an array using a variable), DATASIZE/TYPE =8/12/16, FREQ, SPI_MOD =0/1/2/3 Default=0 , VARIABLE_NAME
*     unsigned char DEVICE_ID,          1) SPI address - Decoder
*     int waitForReply,                 2) Do the slave send us any value?
*     unsigned int SpiData,             3) Data to be sent to the slave.
*     spiDataSize  DataSize,            4) Data size or type    8, 12, 16
*     int spi_frequency,                5) SPI channel frequency
*     int spi_mode                      6) SPI Mode or SPI Format - default: put 0
*     Variable_Name                     7) Variable will be used to save the results.
*     DelayBetweenPackets               8) DelayBetweenPackets in uSec needs between each packet.
*Note:
*                                       Mode   Polarity    Phase
*                                         0        0        0
*                                         1        0        1
*                                         2        1        0
*                                         3        1        1
*/
int token_spi_send(void)
{
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        unsigned int SpiData = 0;
    int waitForReply = 0;
    int DelayBetweenPackets = 0;
    unsigned char device_id = 0;
    spiDataSize DataSize;
    int spi_mode = 0;
    int spi_frequency = 0;
    process_token(TOKEN_SPI_SEND);
    int stringLength;
    int varPosition = -1;
    int varPositionData = -1;

    //1-Get SPI Address . This needed as we use a decoder up to 16 devices
    if (current_token_word != TOKEN_NUMBER) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SPI_ADDRESS_NOT_FOUND);
        quitScriptProgram(1); //Exit the program.
    }
    device_id = (unsigned char)convert_text_to_number();
    process_token(TOKEN_NUMBER);
    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);                                         //1

    //2-Get Wait for reply configuration
    if (current_token_word != TOKEN_NUMBER) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SPI_ADDRESS_NOT_FOUND);
        quitScriptProgram(1); //Exit the program.
    }
    waitForReply = (int)convert_text_to_number();
    process_token(TOKEN_NUMBER);
    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);                                         //2

    //3-Get DATA that will be send to the SPI device. It could be command.          ///TODO : WE SHOULD ALLOW USING VARIABLES. 2018-07-25
    if (current_token_word == TOKEN_VARIABLE) {
        varPositionData = TestSystem_get_variable(NULL); // we dont' need to send the name .. the function will do the job.
        process_token(TOKEN_VARIABLE);
        if (varPositionData < 0) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SPI_DATA_VAR_NOT_FOUND);
            quitScriptProgram(1); //Exit the program.
        }
        /*  Notice that, if we dont' have variable, varPositionData will be equal to -1
        *   and should not be any error .. So, reaching the rest of the instructions means that
        *   we had a number .. not a variable .. so send only the 2 bytes you got as an unsigned int.
        */
    }
    else if (current_token_word == TOKEN_NUMBER) {
        SpiData = (unsigned int)convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }
    else {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SPI_DATA_NOT_FOUND);
        quitScriptProgram(1); //Exit the program.
    }
    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);                                         //3

    //4- how many bits are the data? 8/12/16
    if (current_token_word != TOKEN_NUMBER) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_DATASIZE_NOT_FOUND);
        quitScriptProgram(1); //Exit the program.
    }
    DataSize = (spiDataSize)convert_text_to_number();
    process_token(TOKEN_NUMBER);
    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);                                         //4
    }

    //5 SPI frequency
    if (current_token_word != TOKEN_NUMBER) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SPI_FREQ_NOT_FOUND);
        quitScriptProgram(1); //Exit the program.
    }
    spi_frequency = (int)convert_text_to_number();
    process_token(TOKEN_NUMBER);
    if (current_token_word == TOKEN_COMMA) {
        process_token(TOKEN_COMMA);                                         //5
    }

    //6 spi MODE
    if (current_token_word != TOKEN_NUMBER) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SPI_MODE_NOT_FOUND);
        quitScriptProgram(1); //Exit the program.
    }
    spi_mode = (unsigned int)convert_text_to_number();
    process_token(TOKEN_NUMBER);
    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);                                         //6

    //7 We have all bytes, we need now the name of the variable.
    if (current_token_word != TOKEN_VARIABLE) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
        quitScriptProgram(1); //Exit the program.
    }

    //Variable name that will save the returned byte/bytes by the slave.
    char* spivar_buf = NULL;
    stringLength = strlen(p_prog->txtString);
    spivar_buf = (char*)calloc(stringLength + 1, sizeof(char));
    if (spivar_buf == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SPIVAR);
        quitScriptProgram(1);
    }
    snprintf(spivar_buf, stringLength + 1, "%s", p_prog->txtString);//We don't need to add '\0' as we uses calloc

    process_token(TOKEN_VARIABLE);           // Get next token word.

    if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);                                         //7

    /* TimeDelay in uSec: This is necessary if we send multiple bytes in
    sequence and the slave might not be able to capture the stream so quickly needs between each packet.
    You can skip this if you don't needed .. The default is zero. If you don't write
    this the default value will be used.
    */
    if (current_token_word != TOKEN_NUMBER) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_WARNING_SPI_DELAY_NOT_DEFINED);
        DelayBetweenPackets = 0;
    }
    else
    {
        DelayBetweenPackets = (unsigned int)convert_text_to_number();
        process_token(TOKEN_NUMBER);
    }

    //Execute the real SPI code now

    /*
     * TODO: Check if we should always accept the variable and keep it. If you ask for data without having the variable saved you will cause error and crash*/
    /*if (waitForReply!=0)*/ {
        varPosition = TestSystem_get_variable(spivar_buf);
        if (varPosition == -1) {
            varPosition = TestSystem_get_empty_variable_pos();
            if (varPosition < 0) {
                DEBUG_MESSAGE("No empty global var found\n");
                quitScriptProgram(1);//Exit the program
            }
            addNewScriptVariable(spivar_buf, T_INT, DataSize, varPosition);
            spivar_buf = NULL;
        }
    }
    if (waitForReply == 0) {
        SendReceiveSPI_Device(device_id, waitForReply, SpiData, varPositionData, -1, DataSize, spi_frequency, spi_mode, DelayBetweenPackets);  //No data will be returned
    }
    else {
        SendReceiveSPI_Device(device_id, waitForReply, SpiData, varPositionData, varPosition, DataSize, spi_frequency, spi_mode, DelayBetweenPackets);  //Data will be returned
    }
    return 1;
}
