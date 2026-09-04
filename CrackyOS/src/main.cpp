/**
 ******************************************************************************
 * @file    main.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains Main function used by the interpreter
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
#include "mbed_mem_trace.h"

/** Serial class definition used by the system.*/
SerialCommunication globalSerialCOMvar;
/** Fat filesystem definition used to open/read/write files on the SD card*/
FATFileSystem fs("sd");

osThreadId_t  mainThreadID;

void print_memory_info() {
    // allocate enough room for every thread's stack statistics
    int cnt = osThreadGetCount();
    mbed_stats_stack_t *stats = (mbed_stats_stack_t*) malloc(cnt * sizeof(mbed_stats_stack_t));

    cnt = mbed_stats_stack_get_each(stats, cnt);
    for (int i = 0; i < cnt; i++) {
        printf("Thread: 0x%lX, Stack size: %lu / %lu\r\n", stats[i].thread_id, stats[i].max_size, stats[i].reserved_size);
    }
    free(stats);

    // Grab the heap statistics
    mbed_stats_heap_t heap_stats;
    mbed_stats_heap_get(&heap_stats);
    printf("Heap size: %lu / %lu bytes\r\n", heap_stats.current_size, heap_stats.reserved_size);
}

unsigned char BreakPressed = 0;
/**
*MAIN function
*/
/**
 * Use this function to calculate the hash values.
 * Warning: Different architecture could give different results.
 * update the hash values whenever you change the architecture.
 *
 * */

 /*
 void printfAllhash() {
     static const token_exchanger* p_instruction_set = 0;
     for (p_instruction_set = token_exchange_list; p_instruction_set->token_word != NULL; p_instruction_set++) {
         unsigned long valuem = CalculateHashOfString(p_instruction_set->token_word, strlen(p_instruction_set->token_word));
         std::string st = p_instruction_set->token_word;
         char buf[255]; memset(buf, 0, 255);
         snprintf(buf, 255, "%s  = %lu\n", st.c_str(), valuem);
         globalSerialCOMvar.Serial_pc->write(buf, strlen(buf));
     }
 }
 */


int main()
{
    mainThreadID = rtos::ThisThread::get_id();
    globalSerialCOMvar.Serial_pc = NULL;
    globalSerialCOMvar.Debug_pc = NULL;
    globalSerialCOMvar.Serial_pc = new specialSerial(SERIAL_TX, SERIAL_RX/*, NULL*/, baud_rate);  //SERIAL 2
    if (globalSerialCOMvar.Serial_pc == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_SERIAL_BUFF);
        quitScriptProgram(1);
    }
    //Used only when you need to update the hash numbers.
  //  printfAllhash();

    createDebugPC();

    // Configure the serial USB port for communications
    // globalSerialCOMvar.Serial_pc->baud(baud_rate);
    clearAndHome(); //Clear terminal screens

    /** Start the thread, if debug enabled by default, that take care of debug communications. */
    enable_disableDebugThread();
    select_spi_channel(CHANNEL0);
    Master_SPI_cs = 1;
    CurrentDirectory = "/sd"; //Will be used in file/directory activities

   // Read configuration file. Check if we have other definition than the defaults.
    int sdFailed=1;
    InterActiveMode = 1;
    if(sd.init()==BD_ERROR_OK) {
        int checkScriptfile = 0;
        fs.mount(&sd);
    	checkScriptfile = readConfigFile();
    	if (checkScriptfile == -10 || checkScriptfile == -20) {
    		InterActiveMode = 1;
    		globalSerialCOMvar.Serial_pc->attach(NULL);
    		sdFailed = writeConfigToFile();
    		}else
    		{
    			sdFailed=0; //SD works
    			InterActiveMode = 0;
    		}
    	}else
    	{
    		sdFailed = 1;
    	}

    if(sdFailed==0){
        InterActiveMode = 0;
        InitializeDefaultScriptFileName();
        globalSerialCOMvar.Serial_pc->attach(&ChangeInterActiveModeStatus);
    }

    while (1) {
        mbed_stats_heap_t heap_stats;
        mbed_stats_heap_get(&heap_stats);
        //Run the script file.
        if (InterActiveMode == 0) {
            if (RunScriptFile() == -1) {
                InterActiveMode = 1;
                //Not disabling the interrupt will cause the system to hangs/freeze
                globalSerialCOMvar.Serial_pc->attach(NULL);
            }
        }
        // We have interactive mode and will continue in this mode
        /* We have to make sure that there is a file name in bot .dat och .dad file.*/

        InitializeDefaultScriptFileName();

        /* We can now reset this variable. When error happens in the normal run,
         this will reset that and we can run interactive code again.*/
        StopExecution = 0;
        if (BreakPressed == 1) {
            SERIAL_MESSAGE("^C\n");
            BreakPressed = 0;
        }
        interactive_mode();
    }
}
