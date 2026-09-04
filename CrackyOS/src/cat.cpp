/**
 *******************************************************************************
 * @file    cat.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief  This file contains definitions of file operations (create, cat,..etc
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

#include "cat.h"
/**
  * Print Help message
  * */
void printHelp() {
    SERIAL_MESSAGE(
        "Syntax cat <filename> <-arg1> <-arg2>\n\
           -c : create file \n\
           -a : append/Create new file by joining two other files\n\
            commands -c argument: Optional <arg1> : text will be written to the created file\n\
                     -a:argument: <New FN> <FN1> <FN2>\n\
                   help: print help info\n\
            no argument: print the content of this message.\n");
}

/**
 * Convert a string to lower case.
 * @param me  a pointer to a string that should be converted to lower case.
 * */
void ToLower(char* me) {
    //Stop running if we have error
    STOP_CODE_EXECUTION
        int x = 0;
    while (me[x] != '\0') {
        me[x] = tolower(me[x]);
        x++;
    }
}
/**
 * Write the content of a file to terminal.
 * @param fname  file name which should be printed on the terminal.
 * */
void printfile(const char* fname) {
    //Stop running if we have error
    STOP_CODE_EXECUTION


    FILE* fptr;
    int LineNo = 1;
    fptr = 0;
    // Open file
    std::string stFname = fname;
    if (!defaultDirIncluded(stFname)) stFname = CurrentDirectory + "/" + stFname;
    fptr = fopen(stFname.c_str(), "r");

    if (fptr == 0) {
        SERIAL_MESSAGE("Cannot open file \n");
        return;
    }
    char *a=new char[256];
    // Read contents from file
    while (fgets(a, 255, fptr) != NULL) {
        ChangeTerminalColor(BKG_WHITE);
        ChangeTerminalColor(FONT_BLACK);
        SERIAL_MESSAGE("%04d: %s", LineNo, a);
        ChangeTerminalColor(BKG_DEFAULT);
        ChangeTerminalColor(FONT_DEFAULT);
        LineNo++;
    }
    fclose(fptr);
    SERIAL_MESSAGE("\n");
    if (a!=NULL)
    	delete []a;
    return;
}
/**
 * CAT command implementation'
 * @param argc  int value .. describes how many arguments are there
 * @param argv  two dimensional array contains the command, arguments.
 * */
void Do_Cat(int argc, char** argv) {
    //Stop running if we have error
    STOP_CODE_EXECUTION
        int c;
    std::string stfName1, stfName2, stfName3;
    if (argc < 1) {
        printHelp();
    }
    else if (argc == 1) {
        ToLower(argv[0]);
        if (strcmp(argv[0], "help") == 0) {
            printHelp();
        }
        else {
            printfile(argv[0]);
        }
    }
    else if (argc >= 2) {
        stfName1 = argv[1]; //New file name.
        if (!defaultDirIncluded(stfName1)) stfName1 = CurrentDirectory + "/" + stfName1;

        if (strcmp(argv[0], "-a") == 0) {
            if (argc == 4) {
                stfName2 = argv[2]; //First file name
                if (!defaultDirIncluded(stfName2)) stfName2 = CurrentDirectory + "/" + stfName2;
                stfName3 = argv[3];  //Second file name
                if (!defaultDirIncluded(stfName3)) stfName3 = CurrentDirectory + "/" + stfName3;

                FILE* appendfname = fopen(stfName1.c_str(), "w");  //New file
                if (appendfname == 0) {
                    SERIAL_MESSAGE("ERROR: Cannot create the file a%s", argv[1]);
                    return;
                }
                FILE* fname = fopen(stfName2.c_str(), "r");
                if (fname == 0) {
                    SERIAL_MESSAGE("ERROR: Cannot open the file %s", argv[2]);
                    fclose(appendfname);
                    return;
                }

                while (1) {
                    c = fgetc(fname);
                    if (c == EOF) break;
                    fputc(c, appendfname);
                }
                fclose(fname);
                fname = 0;
                fname = fopen(stfName3.c_str(), "r");
                if (fname == 0) {
                    SERIAL_MESSAGE("ERROR: Cannot open the file %s", argv[3]);
                    return;
                }
                while (1) {
                    c = fgetc(fname);
                    if (c == EOF) break;
                    fputc(c, appendfname);
                }
                fclose(fname);
                fclose(appendfname);
            }
            else {
                SERIAL_MESSAGE("ERROR. Syntax error\n");
            }
        }
        else if (strcmp(argv[0], "-c") == 0) {
            FILE* fname = fopen(stfName1.c_str(), "w");
            if (fname == 0) {
                SERIAL_MESSAGE("ERROR: Cannot open the file %s", argv[1]);
                return;
            }
            char* temstr = correctString(argv[2]);
            fputs(temstr, fname);
            if( temstr!=NULL)
            	delete[] temstr;
            temstr = NULL;
            SERIAL_MESSAGE("File created successfully\n");
            //close the created file pointer.
            fclose(fname);
        }
        else {
            printHelp();
        }
        return;
    }
}
