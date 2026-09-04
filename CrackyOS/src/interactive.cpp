/**
 ******************************************************************************
 * @file    interactive.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains function for creating/reading config file
            It defines default filename, baudrate, interactive and other
            parameters for the system.
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

#include"interactive.h"
#include "hardware_used.h"
/**
  * Keeps the name of the default script file name
  * non processed source code
  * */
std::string SCRIPT_PROG_NAME;
/**
 * Keeps the name of the default script file name
 * processed source code
 * */

std::string SCRIPT_PROG_NAME_DAD;

/** Default baudrate*/

int baud_rate = 115200;     //Default baud rate

/*We assume that there is a file name. This will be changed soon when we
  discover that there is no file name.
*/
unsigned char InterActiveMode = 0;
unsigned char StopExecution = 0;
/*
*  The idea is when there is no file name, the system will go to interactive mode.
    If there is a file name, the system will run that file name. This is to allow
    the system to run the script in a  silent mode.
*/
const char DEFAULT_CONFIG[] = { "#CrackyOS Script OS.\nDefaultFile=/sd/script.dat\n\n#Default BaudRate\nBaudRate=112500\n\n#Default Interactive mode\nInteractive=0" };

/**
* Create config.ini with default values.
*/
int writeConfigToFile()
{
    int x = 0;
    FILE* fp = NULL;
    fp = fopen(DEFAULT_CONFIG_FILE_NAME, "w+");
    if (fp != 0) {
        fprintf(fp, "%s", DEFAULT_CONFIG);
    }
    else {
        DEBUG_MESSAGE("Couldn't create config.ini file\n");
        x = -1;
    }
    if (fp)
        fclose(fp);
    return x;
}
/**
* This file will determine the size of the file.
* "@param filename
*/
int getFileSize(std::string filename)
{
    FILE* p_file = NULL;
    p_file = fopen(filename.c_str(), "rb");
    if (p_file == NULL)
        return -1;
    fseek(p_file, 0, SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);
    return size;
}

/**
 * Is it a comment line  ?
 *
 * @param string represent the input data to the function that will be checked to figure out if the string is text
 * @return boolean value  1 if the input value was a string. otherwise 0
 * */

inline bool IsComment(std::string& str) {
    for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
        if (*it != ' ') {
            if (*it == '#')
                return 1;
            else
                return 0;
        }
    }
    return 0;
}

/**
*  This function will read the configuration saved on the SD-Disk
*  If the configuration file is not there it will return the following
*  integer
*   -1      : DefaultFile     is not found or bad
*   -2      : BaudRate        is not found or bad
*   -3      : InterActiveMode is not found or bad
*
*   -10     : File not found
*   -20     : File size is wrong. The file is corrupted.
*
*   In all these cases, default configuration file should be re-created.
*/

int readConfigFile(void)
{
    size_t pos = 0;
    size_t posEnter = 0;

    int filesize = getFileSize(DEFAULT_CONFIG_FILE_NAME);
    if (filesize == -1)
        return -10; //file not found.
    if (filesize > MAX_LENGTH_OF_CONF_FILE) {
        DEBUG_MESSAGE("File is bigger than normal size. It will be replaced with new\n");
        return -20;
    }
    int result = 0;

    // baud_rate = 115200; //Default
    InterActiveMode = 0; //Default

    std::string myline;
    std::ifstream myfile(DEFAULT_CONFIG_FILE_NAME);
    while (getline(myfile, myline)) {
        if (IsComment(myline)) continue; //Ignore the line .. it is a comment.
        posEnter = myline.find("\n");
        //Start- DefaultFile name
        pos = myline.find("DefaultFile");
        if (pos != std::string::npos) {
            pos = myline.find("=");
            if (posEnter != std::string::npos) {
                DEBUG_MESSAGE("Config.ini is corrupted\n");
                SCRIPT_PROG_NAME.clear();
                SCRIPT_PROG_NAME_DAD.clear();
                result = -1;
                break;
            }
            if (pos == std::string::npos) {
                //No default file name.
                SCRIPT_PROG_NAME.clear();
                SCRIPT_PROG_NAME_DAD.clear();
                continue;
            }
            pos++;
            std::string str2 = myline.substr(pos, myline.size() - pos);
            str2.erase(std::remove_if(str2.begin(), str2.end(), ::isspace), str2.end());
            //File name must be at least 5 digits.
            if (str2.length() < 5) {
                SCRIPT_PROG_NAME.clear();
                SCRIPT_PROG_NAME_DAD.clear();
                continue;
            }

            SCRIPT_PROG_NAME = str2;
            pos = SCRIPT_PROG_NAME.find(".");
            if (pos != std::string::npos) {
                SCRIPT_PROG_NAME_DAD.clear();
                SCRIPT_PROG_NAME_DAD.append(SCRIPT_PROG_NAME.begin(), SCRIPT_PROG_NAME.begin() + pos);
            }
            else {
                SCRIPT_PROG_NAME_DAD = SCRIPT_PROG_NAME;
            }
            SCRIPT_PROG_NAME_DAD = SCRIPT_PROG_NAME_DAD + ".dad";
            continue;
        } //end DefaultFile
        //Start - Find BaudRate
        pos = myline.find("BaudRate");

        if (pos != std::string::npos) {
            if (posEnter != std::string::npos) {
                DEBUG_MESSAGE("Config.ini is corrupted\n");
                result = -2;
                break;
            }
            pos = myline.find("=");
            if (pos == std::string::npos) {
                DEBUG_MESSAGE("Config.ini is corrupted\n");
                result = -2;
                break;
            }
            pos++;
            std::string str2 = myline.substr(pos, myline.length() - pos);
            if (str2.length() < 3) {
                result = -2;
                break;
            }
            baud_rate = atoi(str2.c_str());
            if (baud_rate == 0) {
                baud_rate = 115200;
                result = -2;
                break;
            }
            continue;
        } //END - Find BaudRate

        //Start - Find InterActiveMode
        pos = myline.find("InterActiveMode");

        if (pos != std::string::npos) {
            if (posEnter != std::string::npos) {
                DEBUG_MESSAGE("Config.ini is corrupted\n");
                result = -3;
                break;
            }
            pos = myline.find("=");
            if (pos == std::string::npos) {
                DEBUG_MESSAGE("Config.ini is corrupted\n");
                result = -3;
                break;
            }
            pos++;
            std::string str2 = myline.substr(pos, myline.length() - pos);
            if (str2.size() < 1) {
                //not found
                InterActiveMode = 0; //Default
                result = -4;
                continue;
            }
            InterActiveMode = atoi(str2.c_str());
            continue;
        } //END - Find InterActiveMode
        else {
            // Do nothing .. Just continue.
            continue;
        } //END - Find DefaultFile.
    } //End while
    myfile.close();
    if (result < 0)
        InterActiveMode = 1; //Switch to interactive mode.
    return result;
}

/**
 * Run a specific script file
 *
 * */
int RunScriptFile(void) {
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_START_SCRIPT);
    StopExecution = 0;
    token_initialize();
    //Run the program.
    return (scriptSystem_script_run());
}

/**
 * This function will activate the interactive mode
 * User can use this mode when there is no script program running.
 * Different activities can be performed plus running instructions.
 * As long as the instructions runs, values will be remained in memory.
 * But if you reset the system .. values will be delete.
 * User can start a program from here. Please read the manual for further info.
 * @return -1 if failed, >=0 if OK
 * */
int interactive_mode(void)
{
    SERIAL_MESSAGE("\033[7m\033[1m\t\tInteractive Mode\033[0m \t\n");
    select_spi_channel(CHANNEL0);
    int checkIfItWasnotZeroLength = 1;
    std::string command;
    std::string arguments;
    char *interactiveModeBuf=new char[INTER_ACTIVE_MODE_MAX_LENGTH_OF_THE_STRING];
    memset(interactiveModeBuf, '\0', INTER_ACTIVE_MODE_MAX_LENGTH_OF_THE_STRING);
    CleanUpMemToOriginal();
    token_initializeInterActive();

    //This will run until you execute "run" or you reset your system.

    while (1) {
        if (checkIfItWasnotZeroLength == 1) {
            SERIAL_MESSAGE("\n  OK\n");
            while ((CurrentDirectory.length() >= 1)) {
                //Remove all \n
                if (CurrentDirectory[CurrentDirectory.length() - 1] == '\n')
                    CurrentDirectory.erase(CurrentDirectory.length() - 1);
                else
                    break;
            }

            SERIAL_MESSAGE("%s", (CurrentDirectory.c_str()));
            ChangeTerminalColor(FONT_GREEN);
            SERIAL_MESSAGE("$");
            ChangeTerminalColor(FONT_DEFAULT);
        }
        else {
            checkIfItWasnotZeroLength = 1;
        }

        GetStringFromSerial(interactiveModeBuf);
        change_program_to_lower_case(interactiveModeBuf);

        //These must be cleaned as we don't use them anymore.
        nestedForWhileloop = 0;
        nestedForloop = 0;
        nestedWhileloop = 0;
        command = interactiveModeBuf;
        if (command.size() > 1) {
            while ((command.length() >= 1)) {
                //Remove all \n
                if (command[command.length() - 1] == '\n' || command[command.length() - 1] == '\r')
                    command.erase(command.length() - 1);
                else
                    break;
            }

            // We have something to do/ separate command and arguments.
            int pos = command.find(" ");
            arguments.clear();
            if (pos != -1) {
                arguments.append(command.begin() + pos + 1, command.end());
                command.erase(command.begin() + pos, command.end());
            }
            if (command.rfind("dir", 0) == 0) { do_Listdir(); }
            else if (command.rfind("cd", 0) == 0) { do_cd(arguments); }
            else if (command.rfind("copy", 0) == 0) { do_copy(arguments); }
            else if (command == "del" || command.rfind("del ", 0) == 0) { do_delete(arguments); }
            else if (command.rfind("mkdir", 0) == 0) { do_mkdir(arguments); }
            else if (command.rfind("move", 0) == 0) { do_move(arguments); }
            else if (command.rfind("format", 0) == 0) { do_format(); }
            else if (command.rfind("run", 0) == 0) { return do_run(arguments); } // Execute a file and go out from this mode.
            else if (command.rfind("rename", 0) == 0) { do_rename(arguments); }
            else if (command.rfind("clear", 0) == 0) { clearAndHomeSerial(); }
            else if (command.rfind("cls", 0) == 0) { clearAndHomeSerial(); }
            else if (command.rfind("help", 0) == 0) { print_help(); }
            else if (command.rfind("mount", 0) == 0) { do_mount(); }
            else if (command.rfind("umount", 0) == 0) { do_umount(); }
            else if (command.rfind("unmount", 0) == 0) { do_umount(); }
            else if (command.rfind("reset", 0) == 0) { do_reset(); }
            else if (command.rfind("send_script", 0) == 0) { getScriptProgramFile(); }
            else if (command.rfind("cat", 0) == 0) { doCat(arguments); }
            else if (command.rfind("instructionset", 0) == 0) { do_InstructionSet(); }
            else {
                //Here we have instruction set command .. not folder operations.
                // Now we need to convert the code to a linkedlist

                command.clear();// not needed anymore
                TokenizeToFile* scriptPro = NULL;
                scriptPro = new TokenizeToFile(SCRIPT_PROG_NAME.c_str(), SCRIPT_PROG_NAME_DAD.c_str()); //These files will not be used .. so don't care we need them for the class only
                current_token_word = TOKEN_BAD;            //Default value for the accessed tokens. The variable will be used while the system read the user-code.
                if (scriptPro != NULL) {
                    scriptPro->runInteractive(interactiveModeBuf); //Analyze the code and save it in the link list
                    //Enable Interrupt
                    globalSerialCOMvar.Serial_pc->attach(&ChangeInterActiveModeStatus);
                    StopExecution = 0;
                    if (scriptSystem_script_run() == -1) {
                        /* Disable interrupt again*/
                        globalSerialCOMvar.Serial_pc->attach(NULL);
                    } //Run the code
                    if (scriptPro != NULL) {
                        delete (scriptPro);
                        scriptPro = NULL;
                    }
                    free_scriptProg(); //We should remove the old instruction from the memory.
                }
            }
        }
        else
        {
            checkIfItWasnotZeroLength = 0; //We have a zero length message.
        }
        memset(interactiveModeBuf, '\0', INTER_ACTIVE_MODE_MAX_LENGTH_OF_THE_STRING);
        StopExecution = 0;
    }
    if(interactiveModeBuf!=NULL)
    	delete [] interactiveModeBuf;
    NVIC_SystemReset();  //Reset the system : TODO : Should we do that??
    return 0;
}
