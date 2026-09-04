/**
 ******************************************************************************
 * @file    sdcard.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains sd-card driver
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

#include "sdcard.h"
 // Since we are using a decoder, for each instruction there should be a select_spi_channel(CHANNEL0); instruction.
 /**
  * Global variable keeps the currentDirectory name.
  * */
std::string CurrentDirectory;

/**
 * Check if the file name contains /sd/
 * @param a string as a filename
 * */
bool defaultDirIncluded(std::string& fileName) {
    bool testSDPath;
    std::string path = "/sd/";
    std::size_t found;
    found = fileName.find(path);
    if (found != std::string::npos)
        testSDPath = true;
    else
        testSDPath = false;
    return testSDPath;
}

/**
 This function can be used to test the SD-Card. The function is not used by the CrackyOS
 */
 /*
 void testSDCard() {
     select_spi_channel(CHANNEL0);
     printf("\nWait for new connection...\n");
     mkdir("/sd/mydir", 0777);
     FILE *fp = NULL;
     fp = fopen("/sd/mydir/sdtest.txt", "w");
     if (fp == NULL) {
         printf("Could not open file for write\n");
         return;
     }
     fprintf(fp, "Script System say \"Hello, have fun!\" ");
     fclose(fp);
 }
 */

 /**
  *Initialize the file used by the system.
  *@param  a byte represent the type of the file 0 is read
  *                                              1 is write
  */
FILE* initialize_progFile(unsigned char a) {
    //Stop running if we have error
    STOP_CODE_EXECUTION_NULL

        select_spi_channel(CHANNEL0);
    DEBUG_MESSAGE("Wait connection..\n");
    FILE* fp = NULL;
    if (a == 0) {
        fp = fopen(SCRIPT_PROG_NAME.c_str(), "r");
    }
    else {
        fp = fopen(SCRIPT_PROG_NAME.c_str(), "w");
    }

    if (fp == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_CANNOT_OPEN_FILE);
    }
    return fp;
}

/**
 * File copy  - implementation.
 * syntax copy OldFileNameWithPath  newFileNameWithPath
 * @param src   as source file name
 * @param dst   as destination file name
 * @return -1 if error , 0 if ok.
 */
int file_copy(const char* src, const char* dst) {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        select_spi_channel(CHANNEL0);
    int retval = 0;
    if (NULL == src || NULL == dst) {
        return -1; // Return error.
    }
    FILE* fpsrc = fopen(src, "r");   // src file
    FILE* fpdst = fopen(dst, "w");   // dest file

    while (1) {                  // Copy src to dest
        int ch = fgetc(fpsrc);       // until src EOF read.
        if (ch == EOF)
            break;
        fputc(ch, fpdst);
    }
    fclose(fpsrc);
    fclose(fpdst);

    fpdst = fopen(dst, "r");     // Reopen dest to insure
    if (fpdst == NULL) {          // that it was created.
        retval = -1;           // Return error.
    }
    else {
        fclose(fpdst);
        retval = 0;              // Return success.
    }
    return retval;
}
/**
 * List the files in the directory ..
 * syntax: dir
 * @param pPos : the directory to list
 * @ dirBuf a reference to an array represent a 2 dimensions array will contain the results
 * @return number of objects found
 */
unsigned int do_dir(const char* pPos, char(&dirBuf)[MAX_NO_OF_DIR][MAX_LENGTH_OF_DIR_NAME]) {
    //Stop running if we have error
    STOP_CODE_EXECUTION_NULL;

    select_spi_channel(CHANNEL0);
    DIR* d = NULL;
    d = opendir(pPos);
    if (!d) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_FAILED_TO_OPEN_DIR);
        return 0;
    }
    struct dirent* p = NULL;
    unsigned int counter = 0;
    while ((p = readdir(d)) != NULL) {
        if (counter >= MAX_NO_OF_DIR)
            break;
        memset(dirBuf[counter], '\0', MAX_LENGTH_OF_DIR_NAME);
        strncpy(dirBuf[counter], p->d_name, MAX_LENGTH_OF_DIR_NAME - 1);
        counter++;

//TODO  : THIS IS NOT COMPLETE.2020-02-28
    }
    SERIAL_MESSAGE("\n");
    closedir(d);
    return counter;
}
/**
 * DEL-file instruction implementation.
 * syntax:
 * del FileNameWithPath
 * @param fsrc filename to be deleted.
 */
void do_del(const char* fsrc) {
    //Stop running if we have error
    STOP_CODE_EXECUTION

        select_spi_channel(CHANNEL0);
    DIR* d = NULL;
    d = opendir(fsrc);
    if (d == NULL)
        return;
    struct dirent* p;
    char path[MAX_LENGTH_OF_DIR_NAME * 2] = { 0 };
    while ((p = readdir(d)) != NULL) {
        int pathLength = snprintf(path, sizeof(path), "%s/%s", fsrc, p->d_name);
        if (pathLength >= 0 && pathLength < (int)sizeof(path))
            remove(path);
    }
    closedir(d);
    remove(fsrc);
}
/**
 * DEL all files in the directory - implementation
 * TODO*: Write the syntax.
 */
void delAllFiles_SDCard(void) {
    //Stop running if we have error
    STOP_CODE_EXECUTION
        //TODO : THIS CODE IS WRONG!!!   2020-02-28
        select_spi_channel(CHANNEL0);
    char SystemDirName[MAX_NO_OF_DIR][MAX_LENGTH_OF_DIR_NAME];
    unsigned int counter = do_dir("/", SystemDirName);
    clearAndHome();
    SERIAL_MESSAGE("Total objects %d", counter);
    while (counter != 0) {
        counter--;
        do_del(SystemDirName[counter]);
        clearAndHome();
        SERIAL_MESSAGE("Remained %d/%d", counter);
    }
    print_help();
}

/**
 *   SD-Card file/folder operations
 *  list all directories and print it on the terminal
 */
void do_Listdir() {
    //Stop running if we have error
    STOP_CODE_EXECUTION

        if (CurrentDirectory.size() == 0) {
            CurrentDirectory = "/sd";
        }
    vector < string > filenames; //filenames are stored in a vector string
    DIR* directoryPointer = NULL;

    directoryPointer = opendir(CurrentDirectory.c_str());
    if (directoryPointer != NULL) {
        struct dirent* DirectoryEntP = NULL;
        while ((DirectoryEntP = readdir(directoryPointer)) != NULL)
            filenames.push_back(string(DirectoryEntP->d_name));
        // directoryPointer->close();
        for (unsigned int i = 0; i < filenames.size(); ++i) {
            //Check  if it is a directory
            std::string scanDir = CurrentDirectory + "/" + filenames[i];
            DIR* tempd = opendir(scanDir.c_str());
            if (tempd != 0) {
                ChangeTerminalColor(BKG_YELLOW);
                SERIAL_MESSAGE("%s\n", ((std::string)filenames[i]).c_str());
                ChangeTerminalColor(BKG_DEFAULT);
                // tempd->close();
            }
            else {
                SERIAL_MESSAGE("%s\n", ((std::string)filenames[i]).c_str());
            }
        }
    }
    SERIAL_MESSAGE("\n");
}
/**
 * Change the current active directory.
 * syntax: cd NewDirectoryName.
 * @param st a string represent the new directory to change to
 * */
int do_cd(std::string& st) {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS;
    if (!defaultDirIncluded(st)) {
        CurrentDirectory = "/sd";
    }
    if (st.find("..") != string::npos) {
        CurrentDirectory = "/sd";  //Root directory
    }
    else
        st = CurrentDirectory + "/" + st;

    while ((CurrentDirectory.length() >= 1)) {
        //Remove all \n
        if (CurrentDirectory[CurrentDirectory.length() - 1] == '\n')
            CurrentDirectory.erase(CurrentDirectory.length() - 1);
        else
            break;
    }

    if (opendir(st.c_str()) == NULL)        return -1; //error
    else CurrentDirectory = st;
    return 0;
}

/**
 * Reset the system
 * */
void do_reset() {
    do_umount();
    NVIC_SystemReset();
}
/**
 * Re mount the SD card
 * */
void do_mount() {
    /* We don't know if it is not initialized.
     * We prevent error
     * */

    fs.unmount();
    sd.deinit();

    sd.init();
    fs.mount(&sd);
    return;
}
/**
 *  unmount the SD card
 * */
void do_umount() {
    /*while(sd.IsInitialized())*/ {
        fs.unmount();
        sd.deinit();
    }
    return;
}

/**
 * Copy a file
 * Syntax: copy oldFileNameWithPath newFileNameWithPath
 * @param st a string of oldfilename and newfilename to be copied
 * */
void do_copy(std::string& st) {
    //Stop running if we have error
    STOP_CODE_EXECUTION

        if (CurrentDirectory.size() == 0) {
            CurrentDirectory = "/sd";
        }

    char delim = ' ';
    std::string oldName, newName;
    int pos = st.find(delim);

    oldName = st.substr(0, pos);
    newName = st.substr(pos + 1, st.size());
    removeSpace(oldName);
    removeSpace(newName);
    if (!defaultDirIncluded(oldName)) {
        oldName = CurrentDirectory + '/' + oldName;
    }
    if (!defaultDirIncluded(newName)) {
        newName = CurrentDirectory + '/' + newName;
    }

    file_copy(oldName.c_str(), newName.c_str());
}
/**
 * Delete a file
 * Syntax: del fileName without path.
 * @param  delFile filename to be deleted.
 * */
void do_delete(std::string& delFile) {
    //Stop running if we have error
    STOP_CODE_EXECUTION

        if (CurrentDirectory.size() == 0) {
            CurrentDirectory = "/sd";
        }

    unsigned int pos = 0;
    std::string fileDel;
    std::string subString;
    int delAll = 0;
    int ManyFiles = 0;

    DIR* directoryPointer = NULL;
    struct dirent* DirectoryEntP = NULL;
    directoryPointer = opendir(CurrentDirectory.c_str());
    if (directoryPointer == NULL) {
        SERIAL_MESSAGE("Cannot open directory %s\n", CurrentDirectory.c_str());
        return;
    }
    if (delFile == string("*")) {
        delAll = 1;
    }
    else {
        pos = delFile.find("*");
        if ((pos != std::string::npos) && (delFile.size() > 1))
            ManyFiles = 1;
    }
    while ((DirectoryEntP = readdir(directoryPointer)) != NULL) {
        if (delAll) {
            // Delete all files.
            fileDel = CurrentDirectory + "/" + DirectoryEntP->d_name;
            remove(fileDel.c_str());
            fileDel.clear();
        }
        else if (ManyFiles) {
            //Delete files that starts with one or more than one letters and has a '*' in the end
            subString = delFile.substr(0, pos - 1); //Take the letters before '*'
            fileDel = DirectoryEntP->d_name;
            if (fileDel.rfind(subString.c_str(), 0) == 0) {
                //Found delete the file.
                fileDel = CurrentDirectory + "/" + DirectoryEntP->d_name;
                remove(fileDel.c_str());
            }
        }
        else {
            if (defaultDirIncluded(delFile)) {
                removeSD(delFile);
            }
            if (delFile.compare(std::string(DirectoryEntP->d_name)) == 0) {
                //Delete only one file
                fileDel = CurrentDirectory + "/" + DirectoryEntP->d_name;
                remove(fileDel.c_str());
                break; //We are done.
            }
        }
    }
    closedir(directoryPointer);
    SERIAL_MESSAGE("%s deleted\n", delFile.c_str());
}

/**
 * Make a directory
 * Syntax: mkdir directoryName
 * @param path of the directory that dir should take place
 * */
void do_mkdir(std::string& st) {
    //Stop running if we have error
    STOP_CODE_EXECUTION

        if (CurrentDirectory.size() == 0) {
            CurrentDirectory = "/sd";
        }
    removeSpace(st);
    st = CurrentDirectory + '/' + st;
    mkdir(st.c_str(), 0777);
}
/**
 * Move a file to another directory
 * move fileNameWithPath  fileNameWithPath
 * @param st a string contation both  fileNameWithPath  fileNameWithPath
 */
void do_move(std::string& st) {
    //Stop running if we have error
    STOP_CODE_EXECUTION

        if (CurrentDirectory.size() == 0) {
            CurrentDirectory = "/sd";
        }

    char delim = ' ';
    std::string oldName, newName;
    int pos = st.find(delim);
    oldName = st.substr(0, pos);
    newName = st.substr(pos + 1, st.size());
    removeSpace(oldName);
    removeSpace(newName);

    file_copy(oldName.c_str(), newName.c_str());
    do_delete(oldName); //Remove the old file.
}
/**
 * Format the SD Card
 * Syntax: format
 *
 * */
void do_format() {
    //Stop running if we have error
    STOP_CODE_EXECUTION
        const char* mes = "SD-Disk will be formatted\n";
    globalSerialCOMvar.Serial_pc->write(mes, strlen(mes));

    fs.unmount();
    fs.reformat(&sd);
    fs.mount(&sd);
}
/**
 * Run a script File
 * Syntax run scriptFileName without path.
 *@param st a string contains the  scriptFileName without path.
 * */
int do_run(std::string& st) {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        //Reactivate interrupt.
        InterActiveMode = 0;
    StopExecution = 0;

    if (CurrentDirectory.size() == 0) {
        CurrentDirectory = "/sd";
    }
    CleanUpMemToOriginal(); //We must clean memory after running interactive mode.
    if (st.size() <= 2) {
        //No filename entered.
        st.clear();
        InitializeDefaultScriptFileName();
    }
    else {
        removeSpace(st);
        if (!defaultDirIncluded(st)) SCRIPT_PROG_NAME = CurrentDirectory + '/' + st;
        SCRIPT_PROG_NAME_DAD.clear();
        unsigned int pos = SCRIPT_PROG_NAME.find(".");
        if (pos != std::string::npos) {
            SCRIPT_PROG_NAME_DAD.append(SCRIPT_PROG_NAME.begin(), SCRIPT_PROG_NAME.begin() + pos);
        }
        else {
            SCRIPT_PROG_NAME_DAD = SCRIPT_PROG_NAME;
        }
        SCRIPT_PROG_NAME_DAD = SCRIPT_PROG_NAME_DAD + ".dad";
    }

    /*We changed the file name, we return -1,
    the main function's while should take care of executing the instruction*/
    /* We deinitialize and demount since it will be re-mounted later*/
    globalSerialCOMvar.Serial_pc->attach(&ChangeInterActiveModeStatus);
    return -1; //RunScriptFile(); //Exit from interactive mode by returning the results
}
/**
 * Remove space before and after filename.
 * Warning:FileName shouldn't contain space. It will cause error
 * @param st a string that has space needs to be removed.
 * */
void removeSpace(std::string& _st) {
    _st.erase(std::remove_if(_st.begin(), _st.end(), ::isspace), _st.end());
}

/**
 * Remove the path /sd/. This assume that the path is only /sd/
 * @param _st remove the word /sd/ from a string.
 */
void removeSD(std::string& _st) {
    std::string input = "/sd";
    std::size_t i = _st.find(input);
    if (i != std::string::npos)
        _st.erase(i, 4);
}

/**
 * Rename a file
 * Syntax:rename fileNameWithPath fileNameWithPath
 * @param st : a string that contains fileNameWithPath fileNameWithPath
 * */
void do_rename(std::string& st) {
    //Stop running if we have error
    STOP_CODE_EXECUTION
        if (CurrentDirectory.size() == 0) {
            CurrentDirectory = "/sd";
        }
    char delim = ' ';
    std::string oldName, newName;
    int pos = st.find(delim);
    oldName = st.substr(0, pos);
    newName = st.substr(pos + 1, st.size());
    removeSpace(oldName);
    removeSpace(newName);
    oldName = CurrentDirectory + '/' + oldName;
    newName = CurrentDirectory + '/' + newName;
    fs.rename(oldName.c_str(), newName.c_str());
}

/**
* CAT instruction implementation.
* This instruction reads data from the file and gives their content as output. You can also use it to create new file
* @param stIn : Astring with multiple words
* syntax:
* cat : It displays the help on the screen
* cat help : It displays the help on the screen
* cat <filename> : It displays the contents of the text file on the screen
* cat -c <filename> <Script_commands> : Creates a new file and put the <text> inside the file. <Script_commands> is optional.
* cat -a <filenme1> <filename2> <filename3> :
* This command creates a new file with the name <filename1> and combines the text of file <filename2> and <filename3> and put it in the new file.
*
*
*/

void doCat(std::string& stIn) {
    //Stop running if we have error
    STOP_CODE_EXECUTION
        if (stIn.size() == 0) {
            //No arguments
            Do_Cat(0, NULL);
            return;
        }

    std::string st;
    std::string delim = " ";
    auto end = stIn.find(delim);
    int x = 0;
    char** _argv = new char* [6];
    for (int i = 0; i < 6; i++) _argv[i] = NULL;
    if (end == std::string::npos) {
        st = stIn;
        _argv[0] = (char*)calloc(st.size() + 1, sizeof(char));
        memcpy(_argv[0], st.c_str(), st.size());
        x=1;
    }
    else {
        auto start = 0U;
        while (end != std::string::npos)
        {
            st = stIn.substr(start, end - start);

            _argv[x] = (char*)calloc(st.size()+ 1, sizeof(char));
            memcpy(_argv[x], st.c_str(), st.size());
            x++;
            start = end + delim.length();
            if ( (x == 2 && strcmp(_argv[0], "-c") == 0) || (x == 3 && strcmp(_argv[0], "-a") == 0)){
                //We have a -c filename "big text here .. script code could be with multiple spaces"
                st = stIn.substr(start);
                _argv[x] = (char*)calloc(st.size()+ 1, sizeof(char));
                memcpy(_argv[x], st.c_str(), st.size());
                x++;
                break; //go out from while loop. We are done .. last string should be counted as 1 string.
            }
            else
                end = stIn.find(delim, start);
        }
    }
    Do_Cat(x, _argv);
    for (int i = 0; i < 6; i++) {
    	if(_argv[i]!=NULL){
    		delete[]_argv[i];
            _argv[i] = NULL;}
    }
    if(_argv!=NULL)
    	delete[]_argv;
}

typedef struct {
    const char* name;
    unsigned char type;  //1 instruction , 0 others.
} instSet;

const instSet code_keywords[] = {   // List of known script keywords...
  {"abs"                          , 0},
  {"acos"                         , 0},
  {"aget"                         , 1},
  {"and"                          , 0},
  {"aput"                         , 1},
  {"asin"                         , 0},
  {"atan"                         , 0},
  {"baud_rate"                    , 1},
  {"call"                         , 1},
  {"can_get"                      , 1},
  {"can_send"                     , 1},
  {"clear"                        , 1},
  {"cls"                          , 1},
  {"cos"                          , 0},
  {"create_reg"                   , 1},
  {"debug_port"                   , 1},
  {"delay"                        , 1},
  {"delete"                       , 1},
  {"disable_debug"                , 1},
  {"else"                         , 1},
  {"enable_debug"                 , 1},
  {"exit"                         , 1},
  {"for"                          , 1},
  {"function"                     , 1},
  {"get"                          , 1},
  {"goto"                         , 1},
  {"i2c_get"                      , 1},
  {"i2c_scl"                      , 0},
  {"i2c_sda"                      , 0},
  {"i2c_send"                     , 1},
  {"if"                           , 1},
  {"interruptin"                  , 1},
  {"invert"                       , 0},
  {"ln"                           , 0},
  {"log"                          , 0},
  {"mod"                          , 0},
  {"msec"                         , 0},
  {"or"                           , 0},
  {"put"                          , 1},
  {"pwm"                          , 1},
  {"quit"                         , 1},
  {"rem"                          , 1},
  {"round"                        , 0},
  {"sec"                          , 0},
  {"serial_get"                   , 1},
  {"print"                        , 1},
  {"sin"                          , 0},
  {"sizeof"                       , 0},
  {"spi_send"                     , 1},
  {"sqrt"                         , 0},
  {"tan"                          , 0},
  {"ticker"                       , 1},
  {"usec"                         , 0},
  {"user_button"                  , 0},
  {"variable"                     , 1},
  {"while"                        , 1},
  {NULL                           , 0}
};

void do_InstructionSet() {
    int i = 0;
    int g = 0;
    int maxLength = 0;

    while (code_keywords[i].name != NULL) {
        int le = strlen(code_keywords[i].name);
        if (le > maxLength)
            maxLength = le + 2;
        i++;
    }
    char buf[80];
    i = 0;
    while (code_keywords[i].name != NULL) {
        if (code_keywords[i].type == 1) {
            SERIAL_MESSAGE(code_keywords[i].name);
            if (g < 3) {
                memset(buf, 0, maxLength);
                memset(buf, ' ', maxLength - strlen(code_keywords[i].name));
                SERIAL_MESSAGE(buf);
                g++;
            }
            else
            {
                SERIAL_MESSAGE("\n");
                g = 0;
            }
        }
        i++;
    }
}
