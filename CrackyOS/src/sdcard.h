/**
 ******************************************************************************
 * @file    sdcard.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains for_loop instruction.
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

#ifndef SDCARD_H
#define SDCARD_H
#include "mbed.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>

#include "SD/SDBlockDevice.h"
#include <fat/FATFileSystem.h>

#include "cat.h"
#include "serialmessage.h"
#include "spi_hw.h"
#include "tokens.h"

/*
 * L476RG
SPI_MOSI = PA_7,
SPI_MISO = PA_6,
SPI_SCK = PA_5,
SPI_CS = PB_6,

* F207zg
SPI_MOSI =PB_5 ,
SPI_MISO = PA_6,
SPI_SCK = PA_5,
SPI_CS = PD_14 ,

*/
/*
//Use this name for your file containing the program for the system.
#define SCRIPT_PROG_NAME            "/sd/prog_script.dat"
#define SCRIPT_PROG_NAME_DAD       "/sd/prog_script.dad"
*/

/*
#define INTERACTIVE_SCRIPT_PROG_NAME      "/sd/interac_prog_script.dat"
#define INTERACTIVE_SCRIPT_PROG_NAME_DAD  "/sd/interac_prog_script.dad"
*/

#define MAX_NO_OF_DIR 256
#define MAX_LENGTH_OF_DIR_NAME 256
// We have only two ways of programming this device with script, a file on SD
// card, or using serial communications
enum { SD_CARD_SCRIPT_PROGRAM, USART_SCRIPT_PROGRAM };

// extern class SDFileSystem sd;//(PA_7, PA_6, PA_5, PB_6, "sd");

FILE *initialize_progFile(unsigned char a);
extern std::string CurrentDirectory;

// void testSDCard();

extern FATFileSystem fs;

/**
 * File copy  - implementation.
 * syntax copy OldFileNameWithPath  newFileNameWithPath
 * @param src   as source file name
 * @param dst   as destination file name
 * @return -1 if error , 0 if ok.
 */
int file_copy(const char *src, const char *dst);

/**
 * List the files in the directory ..
 * syntax: dir
 * @param pPos : the directory to list
 * @ dirBuf a reference to an array represent a 2 dimensions array will contain
 * the results
 * @return number of objects found
 */
unsigned int do_dir(const char *pPos,
                    char (&dirBuf)[MAX_NO_OF_DIR][MAX_LENGTH_OF_DIR_NAME]);

/**
 * DEL-file instruction implementation.
 * syntax:
 * del FileNameWithPath
 * @param fsrc filename to be deleted.
 */
void do_del(const char *pFilename);

/**
 * DEL all files in the directory - implementation
 * TODO*: Write the syntax.
 */
void delAllFiles_SDCard();

/**
 *   SD-Card file/folder operations
 *  list all directories and print it on the terminal
 */
void do_Listdir();
/**
 * Change the current active directory.
 * syntax: cd NewDirectoryName.
 * @param st a string represtend the new directory to change to
 * */
int do_cd(std::string &st);
/**
 * Copy a file
 * Syntax: copy oldFileNameWithPath newFileNameWithPath
 * @param st a string of oldfilename and newfilename to be copied
 * */
void do_copy(std::string &st);
/**
 * Delete a file
 * Syntax: del fileName without path.
 * @param  delFile filename to be deleted.
 * */
void do_delete(std::string &delFile);
/**
 * Make a directory
 * Syntax: mkdir directoryName
 * @param path of the directory that dir should take place
 * */
void do_mkdir(std::string &st);
/**
 * Move a file to another directory
 * move fileNameWithPath  fileNameWithPath
 * @param st a string concatenation both  fileNameWithPath  fileNameWithPath
 */
void do_move(std::string &st);
/**
 * Format the SD Card
 * Syntax: format
 *
 * */
void do_format();
/**
 * Run a script File
 * Syntax run scriptFileName without path.
 *@param st a string contains the  scriptFileName without path.
 * */
int do_run(std::string &st);
/**
 * Rename a file
 * Syntax:rename fileNameWithPath fileNameWithPath
 * @param st : a string that contains fileNameWithPath fileNameWithPath
 * */
void do_rename(std::string &st);

/**
 * CAT instruction implementation.
 * This instruction reads data from the file and gives their content as output.
 * You can also use it to create new file
 * @param stIn : Astring with multiple words
 * syntax:
 * cat : It displays the help on the screen
 * cat help : It displays the help on the screen
 * cat <filename> : It displays the contents of the text file on the screen
 * cat -c <filename> <Script_commands> : Creates a new file and put the <text>
 * inside the file. <Script_commands> is optional. cat -a <filenme1> <filename2>
 * <filename3> : This command creates a new file with the name <filename1> and
 * combines the text of file <filename2> and <filename3> and put it in the new
 * file.
 *
 *
 */
void doCat(std::string &st);

void _DoCat(int argc, const char **argv);
/**
 * Remove space before and after filename.
 * Warning:FileName shouldn't contain space. It will cause error
 * */
void removeSpace(std::string &st);

/**
 * Check if the file name contains /sd/
 * */
bool defaultDirIncluded(std::string &fileName);

/**
 * Remove /sd/ from a string
 * */
void removeSD(std::string &_st);

/**
 * Re mount the SD card
 * */
void do_mount();

/**
 *  unmount the SD card
 * */
void do_umount();
/**
 * Reset the system
 * */
void do_reset();

/**
 * Show instruction sets.
 */
void do_InstructionSet();
#endif
