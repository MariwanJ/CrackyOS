/**
 ******************************************************************************
 * @file    nucleo_l4r5zi.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains some functions and definitions.
 *          and important variables that are used by the interpreter.
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
#if TARGET_NUCLEO_L4R5ZI
#include "l4r5zi/nucleo_l4r5zi_hw.h"
#include "common.h"

/**
 * This function will register systems GPIO that are in the architecture of the system.
 * example of these are SPI, Serial, I2C ..etc
 * TODO: THIS CODE IS NOT CORRECT FOR THIS BOARD!!! 2020-05-19
 * */
void add_Reserved_GPIO(void) {
    //Stop running if we have error
    STOP_CODE_EXECUTION
        if (GlobalGPIO_REG == NULL) {
            GlobalGPIO_REG = new RegisterGPIO();
            if (GlobalGPIO_REG == NULL) {
                DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GLOBAL_GPIO);
                quitScriptProgram(1); //Exit the program.
                return;
            }
        }
    //* Be aware when you add any GPIO, you should use the TOKEN_XXXX where xxxx is the pinname, not just the pinname */
    //Serial PC
    GlobalGPIO_REG->add(TOKEN_PA_2, FTOKEN_SERIAL_TX);
    GlobalGPIO_REG->add(TOKEN_PA_3, FTOKEN_SERIAL_RX);
    //  PC_1, PC_0
    GlobalGPIO_REG->add(TOKEN_PC_1, FTOKEN_SERIAL_TX);
    GlobalGPIO_REG->add(TOKEN_PC_0, FTOKEN_SERIAL_RX);
    //Master SPI used for SD and all other SPI devices.

    GlobalGPIO_REG->add(TOKEN_PA_7, FTOKEN_SPI_MOSI);
    GlobalGPIO_REG->add(TOKEN_PA_6, FTOKEN_SPI_MISO);
    GlobalGPIO_REG->add(TOKEN_PA_5, FTOKEN_SPI_CLK);
    GlobalGPIO_REG->add(TOKEN_PB_6, FTOKEN_SPI_CS);
    //I2C1
    GlobalGPIO_REG->add(TOKEN_PB_8, FTOKEN_I2C_CLK);
    GlobalGPIO_REG->add(TOKEN_PB_9, FTOKEN_I2C_SDA);
    //CAN  CAN_1_RD, CAN_1_TD
    GlobalGPIO_REG->add(PA_11, FTOKEN_CAN_RD);
    GlobalGPIO_REG->add(PA_12, FTOKEN_CAN_TD);
    // ENCODER
    GlobalGPIO_REG->add(LINE0, FTOKEN_DIGITAL_IN_OUT);
    GlobalGPIO_REG->add(LINE1, FTOKEN_DIGITAL_IN_OUT);
    GlobalGPIO_REG->add(LINE2, FTOKEN_DIGITAL_IN_OUT);
    GlobalGPIO_REG->add(LINE3, FTOKEN_DIGITAL_IN_OUT);
}

/**This function is necessary to return the pin name from TOKEN_NAME
 *
 * @param inputENUM : Enum representation of the GPIO
 * @return PinName  or NC
 * */
PinName retrievePinNameFromTOKEN_NAME(int inputENUM) {
    //Stop running if we have error
    if (StopExecution == 1) return NC;

    switch (inputENUM) {
    case TOKEN_PA_0:           return  PA_0;
    case TOKEN_PA_1:           return  PA_1;
    case TOKEN_PA_2:           return  PA_2;
    case TOKEN_PA_3:           return  PA_3;
    case TOKEN_PA_4:           return  PA_4;
    case TOKEN_PA_5:           return  PA_5;
    case TOKEN_PA_6:           return  PA_6;
    case TOKEN_PA_7:           return  PA_7;
    case TOKEN_PA_8:           return  PA_8;
    case TOKEN_PA_9:           return  PA_9;
    case TOKEN_PA_10:           return  PA_10;
    case TOKEN_PA_11:           return  PA_11;
    case TOKEN_PA_12:           return  PA_12;
    case TOKEN_PA_13:           return  PA_13;
    case TOKEN_PA_14:           return  PA_14;
    case TOKEN_PA_15:           return  PA_15;
    case TOKEN_PB_0:           return  PB_0;
    case TOKEN_PB_1:           return  PB_1;
    case TOKEN_PB_2:           return  PB_2;
    case TOKEN_PB_3:           return  PB_3;
    case TOKEN_PB_4:           return  PB_4;
    case TOKEN_PB_5:           return  PB_5;
    case TOKEN_PB_6:           return  PB_6;
    case TOKEN_PB_7:           return  PB_7;
    case TOKEN_PB_8:           return  PB_8;
    case TOKEN_PB_9:           return  PB_9;
    case TOKEN_PB_10:           return  PB_10;
    case TOKEN_PB_11:           return  PB_11;
    case TOKEN_PB_12:           return  PB_12;
    case TOKEN_PB_13:           return  PB_13;
    case TOKEN_PB_14:           return  PB_14;
    case TOKEN_PB_15:           return  PB_15;
    case TOKEN_PC_0:           return  PC_0;
    case TOKEN_PC_1:           return  PC_1;
    case TOKEN_PC_2:           return  PC_2;
    case TOKEN_PC_3:           return  PC_3;
    case TOKEN_PC_4:           return  PC_4;
    case TOKEN_PC_5:           return  PC_5;
    case TOKEN_PC_6:           return  PC_6;
    case TOKEN_PC_7:           return  PC_7;
    case TOKEN_PC_8:           return  PC_8;
    case TOKEN_PC_9:           return  PC_9;
    case TOKEN_PC_10:           return  PC_10;
    case TOKEN_PC_11:           return  PC_11;
    case TOKEN_PC_12:           return  PC_12;
    case TOKEN_PC_13:           return  PC_13;
    case TOKEN_PC_14:           return  PC_14;
    case TOKEN_PC_15:           return  PC_15;
    case TOKEN_PD_0:           return  PD_0;
    case TOKEN_PD_1:           return  PD_1;
    case TOKEN_PD_2:           return  PD_2;
    case TOKEN_PD_3:           return  PD_3;
    case TOKEN_PD_4:           return  PD_4;
    case TOKEN_PD_5:           return  PD_5;
    case TOKEN_PD_6:           return  PD_6;
    case TOKEN_PD_7:           return  PD_7;
    case TOKEN_PD_8:           return  PD_8;
    case TOKEN_PD_9:           return  PD_9;
    case TOKEN_PD_10:           return  PD_10;
    case TOKEN_PD_11:           return  PD_11;
    case TOKEN_PD_12:           return  PD_12;
    case TOKEN_PD_13:           return  PD_13;
    case TOKEN_PD_14:           return  PD_14;
    case TOKEN_PD_15:           return  PD_15;
    case TOKEN_PE_0:           return  PE_0;
    case TOKEN_PE_1:           return  PE_1;
    case TOKEN_PE_2:           return  PE_2;
    case TOKEN_PE_3:           return  PE_3;
    case TOKEN_PE_4:           return  PE_4;
    case TOKEN_PE_5:           return  PE_5;
    case TOKEN_PE_6:           return  PE_6;
    case TOKEN_PE_7:           return  PE_7;
    case TOKEN_PE_8:           return  PE_8;
    case TOKEN_PE_9:           return  PE_9;
    case TOKEN_PE_10:           return  PE_10;
    case TOKEN_PE_11:           return  PE_11;
    case TOKEN_PE_12:           return  PE_12;
    case TOKEN_PE_13:           return  PE_13;
    case TOKEN_PE_14:           return  PE_14;
    case TOKEN_PE_15:           return  PE_15;
    case TOKEN_PF_0:           return  PF_0;
    case TOKEN_PF_1:           return  PF_1;
    case TOKEN_PF_2:           return  PF_2;
    case TOKEN_PF_3:           return  PF_3;
    case TOKEN_PF_4:           return  PF_4;
    case TOKEN_PF_5:           return  PF_5;
    case TOKEN_PF_6:           return  PF_6;
    case TOKEN_PF_7:           return  PF_7;
    case TOKEN_PF_8:           return  PF_8;
    case TOKEN_PF_9:           return  PF_9;
    case TOKEN_PF_10:           return  PF_10;
    case TOKEN_PF_11:           return  PF_11;
    case TOKEN_PF_12:           return  PF_12;
    case TOKEN_PF_13:           return  PF_13;
    case TOKEN_PF_14:           return  PF_14;
    case TOKEN_PF_15:           return  PF_15;
    case TOKEN_PG_0:           return  PG_0;
    case TOKEN_PG_1:           return  PG_1;
    case TOKEN_PG_2:           return  PG_2;
    case TOKEN_PG_3:           return  PG_3;
    case TOKEN_PG_4:           return  PG_4;
    case TOKEN_PG_5:           return  PG_5;
    case TOKEN_PG_6:           return  PG_6;
    case TOKEN_PG_7:           return  PG_7;
    case TOKEN_PG_8:           return  PG_8;
    case TOKEN_PG_9:           return  PG_9;
    case TOKEN_PG_10:           return  PG_10;
    case TOKEN_PG_11:           return  PG_11;
    case TOKEN_PG_12:           return  PG_12;
    case TOKEN_PG_13:           return  PG_13;
    case TOKEN_PG_14:           return  PG_14;
    case TOKEN_PG_15:           return  PG_15;
    case TOKEN_PH_0:           return  PH_0;
    case TOKEN_PH_1:           return  PH_1;
//    case TOKEN_PH_2:           return  PH_2;
    case TOKEN_PH_3:            return  PH_3;
    case TOKEN_NC:            return NC;
    default: {            DEBUG_MESSAGE("ERROR %i\n", DBG_SYNTAX_ERROR_TOKEN_NOT_FOUND);      }
    }
    quitScriptProgram(1);
    return NC;
}

/** Instruction set and GPIO
* This array convert a normal text used in the script to TOKEN ENUM Words.
* Very important: Whenever a new GPIO or new Instruction is introduced, you
* should add the definition to this array.
* to calculate the hash .. you need to run the printfAllhash.
* You find it in the main.cpp
*/

const token_exchanger token_exchange_list[]{
//  inst             enum                           hash
    { "abs",            TOKEN_ABS                 ,  709362235            },
    { "acos",           TOKEN_ACOS                ,  1006755615           },
    { "aget",           TOKEN_AGET                ,  2462537784           },
    { "and",            TOKEN_AND                 ,  254395046            },
    { "aput",           TOKEN_APUT                ,  3265553669           },
    { "asin",           TOKEN_ASIN                ,  4272848550           },
    { "atan",           TOKEN_ATAN                ,  108579519            },
    { "baud_rate",      TOKEN_BAUD_RATE           ,  621844436            },
    { "begin",          TOKEN_LEFT_CURLY_BRACKET  ,  1748273790           },
    { "call",           TOKEN_CALL                ,  3018949801           },
    { "can_get",        TOKEN_CAN_GET             ,  1466093946           },
    { "can_send",       TOKEN_CAN_SEND            ,  3456323476           },
    { "clear",          TOKEN_CLEAR               ,  1550717474           },
    { "cls",            TOKEN_CLEAR               ,  59780555             },
    { "cos",            TOKEN_COS                 ,  4220379804           },
    { "create_reg",     TOKEN_CREATE_REG          ,  2523731252           },
    { "debug_port",     TOKEN_DEBUG_PORT          ,  166882598            },
    { "delay",          TOKEN_DELAY               ,  1322381784           },
    { "delete",         TOKEN_DELETE              ,  1740784714           },
    { "disable_debug",  TOKEN_DISABLE_DEBUG       ,  1026410291           },
    { "else",           TOKEN_ELSE                ,  3183434736           },
    { "enable_debug",   TOKEN_ENABLE_DEBUG        ,  1568674666           },
    { "end",            TOKEN_RIGHT_CURLY_BRACKET ,  1787721130           },
    { "exit",           TOKEN_EXIT                ,  3454868101           },
    { "for",            TOKEN_FOR                 ,  2901640080           },
    { "function",       TOKEN_FUNCTION            ,  2664841801           },
    { "get",            TOKEN_GET                 ,  1410115415           },
    { "goto",           TOKEN_GOTO                ,  4121104358           },
    { "i2c_get",        TOKEN_I2C_GET             ,  3187762496           },
    { "i2c_scl",        TOKEN_PB_8                ,  1006780394           },
    { "i2c_sda",        TOKEN_PB_9                ,  821387942            },
    { "i2c_send",       TOKEN_I2C_SEND            ,  2552757210           },
    { "if",             TOKEN_IF                  ,  959999494            },
    { "interruptin",    TOKEN_INTERRUPTIN         ,  1814824783           },
    { "invert",         TOKEN_INVERT              ,  829202337            },
    { "led1",           TOKEN_PA_5                ,  1666181409           },
    { "led2",           TOKEN_PA_5                ,  1615848552           },
    { "led3",           TOKEN_PA_5                ,  1632626171           },
    { "led4",           TOKEN_PA_5                ,  1716514266           },
    { "ln",             TOKEN_LN                  ,  1127334399           },
    { "log",            TOKEN_LOG                 ,  1062293841           },
    { "mod",            TOKEN_MOD                 ,  3751703171           },
    { "msec",           TOKEN_MSEC                ,  51408301             },
    { "nc",             TOKEN_NC                  ,  1446403350           },
    { "or",             TOKEN_OR                  ,  1563699588           },
    { "pa_0" ,          TOKEN_PA_0                ,  3324132589           },
    { "pa_1" ,          TOKEN_PA_1                ,  3307354970           },
    { "pa_2" ,          TOKEN_PA_2                ,  3290577351           },
    { "pa_3" ,          TOKEN_PA_3                ,  3273799732           },
    { "pa_4" ,          TOKEN_PA_4                ,  3257022113           },
    { "pa_5" ,          TOKEN_PA_5                ,  3240244494           },
    { "pa_6" ,          TOKEN_PA_6                ,  3223466875           },
    { "pa_7" ,          TOKEN_PA_7                ,  3206689256           },
    { "pa_8" ,          TOKEN_PA_8                ,  3189911637           },
    { "pa_9" ,          TOKEN_PA_9                ,  3173134018           },
    { "pa_10",          TOKEN_PA_10               ,  3202582494           },
    { "pa_11",          TOKEN_PA_11               ,  3219360113           },
    { "pa_12",          TOKEN_PA_12               ,  3169027256           },
    { "pa_13",          TOKEN_PA_13               ,  3185804875           },
    { "pa_14",          TOKEN_PA_14               ,  3269692970           },
    { "pa_15",          TOKEN_PA_15               ,  3286470589           },
    { "pb_0 ",          TOKEN_PB_0                ,  2365856232           },
    { "pb_1 ",          TOKEN_PB_1                ,  1929785233           },
    { "pb_2 ",          TOKEN_PB_2                ,  2567481850           },
    { "pb_3 ",          TOKEN_PB_3                ,  4278946083           },
    { "pb_4 ",          TOKEN_PB_4                ,  3842875084           },
    { "pb_5 ",          TOKEN_PB_5                ,  185604405            },
    { "pb_6 ",          TOKEN_PB_6                ,  1897068638           },
    { "pb_7 ",          TOKEN_PB_7                ,  2534765255           },
    { "pb_8 ",          TOKEN_PB_8                ,  1559250592           },
    { "pb_9 ",          TOKEN_PB_9                ,  2196947209           },
    { "pb_10",          TOKEN_PB_10               ,  2198227137           },
    { "pb_11",          TOKEN_PB_11               ,  2181449518           },
    { "pb_12",          TOKEN_PB_12               ,  2164671899           },
    { "pb_13",          TOKEN_PB_13               ,  2147894280           },
    { "pb_14",          TOKEN_PB_14               ,  2265337613           },
    { "pb_15",          TOKEN_PB_15               ,  2248559994           },
    { "pc_0" ,          TOKEN_PC_0                ,  3476355587           },
    { "pc_1" ,          TOKEN_PC_1                ,  3459577968           },
    { "pc_2" ,          TOKEN_PC_2                ,  3509910825           },
    { "pc_3" ,          TOKEN_PC_3                ,  3493133206           },
    { "pc_4" ,          TOKEN_PC_4                ,  3543466063           },
    { "pc_5" ,          TOKEN_PC_5                ,  3526688444           },
    { "pc_6" ,          TOKEN_PC_6                ,  3577021301           },
    { "pc_7" ,          TOKEN_PC_7                ,  3560243682           },
    { "pc_8" ,          TOKEN_PC_8                ,  3610576539           },
    { "pc_9" ,          TOKEN_PC_9                ,  3593798920           },
    { "pc_10",          TOKEN_PC_10               ,  3714239680           },
    { "pc_11",          TOKEN_PC_11               ,  3731017299           },
    { "pc_12",          TOKEN_PC_12               ,  3747794918           },
    { "pc_13",          TOKEN_PC_13               ,  3764572537           },
    { "pc_14",          TOKEN_PC_14               ,  3781350156           },
    { "pc_15",          TOKEN_PC_15               ,  3798127775           },
    { "pd_0" ,          TOKEN_PD_0                ,  4094573774           },
    { "pd_1" ,          TOKEN_PD_1                ,  4111351393           },
    { "pd_2" ,          TOKEN_PD_2                ,  4061018536           },
    { "pd_3" ,          TOKEN_PD_3                ,  4077796155           },
    { "pd_4" ,          TOKEN_PD_4                ,  4161684250           },
    { "pd_5" ,          TOKEN_PD_5                ,  4178461869           },
    { "pd_6" ,          TOKEN_PD_6                ,  4128129012           },
    { "pd_7" ,          TOKEN_PD_7                ,  4144906631           },
    { "pd_8" ,          TOKEN_PD_8                ,  3960352822           },
    { "pd_9" ,          TOKEN_PD_9                ,  3977130441           },
    { "pd_10",          TOKEN_PD_10               ,  376183171            },
    { "pd_11",          TOKEN_PD_11               ,  359405552            },
    { "pd_12",          TOKEN_PD_12               ,  409738409            },
    { "pd_13",          TOKEN_PD_13               ,  392960790            },
    { "pd_14",          TOKEN_PD_14               ,  443293647            },
    { "pd_15",          TOKEN_PD_15               ,  426516028            },
    { "pe_0" ,          TOKEN_PE_0                ,  2049161273           },
    { "pe_1" ,          TOKEN_PE_1                ,  2032383654           },
    { "pe_2" ,          TOKEN_PE_2                ,  2015606035           },
    { "pe_3" ,          TOKEN_PE_3                ,  1998828416           },
    { "pe_4" ,          TOKEN_PE_4                ,  2116271749           },
    { "pe_5" ,          TOKEN_PE_5                ,  2099494130           },
    { "pe_6" ,          TOKEN_PE_6                ,  2082716511           },
    { "pe_7" ,          TOKEN_PE_7                ,  2065938892           },
    { "pe_8" ,          TOKEN_PE_8                ,  2183382225           },
    { "pe_9" ,          TOKEN_PE_9                ,  2166604606           },
    { "pe_10",          TOKEN_PE_10               ,  1228434978           },
    { "pe_11",          TOKEN_PE_11               ,  1245212597           },
    { "pe_12",          TOKEN_PE_12               ,  1194879740           },
    { "pe_13",          TOKEN_PE_13               ,  1211657359           },
    { "pe_14",          TOKEN_PE_14               ,  1161324502           },
    { "pe_15",          TOKEN_PE_15               ,  1178102121           },
    { "pf_0" ,          TOKEN_PF_0                ,  530289988            },
    { "pf_1" ,          TOKEN_PF_1                ,  547067607            },
    { "pf_2" ,          TOKEN_PF_2                ,  563845226            },
    { "pf_3" ,          TOKEN_PF_3                ,  580622845            },
    { "pf_4" ,          TOKEN_PF_4                ,  463179512            },
    { "pf_5" ,          TOKEN_PF_5                ,  479957131            },
    { "pf_6" ,          TOKEN_PF_6                ,  496734750            },
    { "pf_7" ,          TOKEN_PF_7                ,  513512369            },
    { "pf_8" ,          TOKEN_PF_8                ,  396069036            },
    { "pf_9" ,          TOKEN_PF_9                ,  412846655            },
    { "pf_10",          TOKEN_PF_10               ,  1005489573           },
    { "pf_11",          TOKEN_PF_11               ,  988711954            },
    { "pf_12",          TOKEN_PF_12               ,  971934335            },
    { "pf_13",          TOKEN_PF_13               ,  955156716            },
    { "pf_14",          TOKEN_PF_14               ,  938379097            },
    { "pf_15",          TOKEN_PF_15               ,  921601478            },
    { "pg_0" ,          TOKEN_PG_0                ,  611624367            },
    { "pg_1" ,          TOKEN_PG_1                ,  594846748            },
    { "pg_2" ,          TOKEN_PG_2                ,  645179605            },
    { "pg_3" ,          TOKEN_PG_3                ,  628401986            },
    { "pg_4" ,          TOKEN_PG_4                ,  544513891            },
    { "pg_5" ,          TOKEN_PG_5                ,  527736272            },
    { "pg_6" ,          TOKEN_PG_6                ,  578069129            },
    { "pg_7" ,          TOKEN_PG_7                ,  561291510            },
    { "pg_8" ,          TOKEN_PG_8                ,  745845319            },
    { "pg_9" ,          TOKEN_PG_9                ,  729067700            },
    { "pg_10",          TOKEN_PG_10               ,  4238242116           },
    { "pg_11",          TOKEN_PG_11               ,  4255019735           },
    { "pg_12",          TOKEN_PG_12               ,  4271797354           },
    { "pg_13",          TOKEN_PG_13               ,  4288574973           },
    { "pg_14",          TOKEN_PG_14               ,  4171131640           },
    { "pg_15",          TOKEN_PG_15               ,  4187909259           },
    { "ph_0",           TOKEN_PH_0                ,  1240185146           },
    { "ph_1",           TOKEN_PH_1                ,  1256962765           },
   // { "ph_2",           TOKEN_PH_2                ,  1206629908           },
    { "ph_3" ,          TOKEN_PH_3                ,  1223407527           },
    { "ph_10",          TOKEN_PH_10               ,  3994508359           },
    { "ph_11",          TOKEN_PH_10               ,  3977730740           },
    { "ph_12",          TOKEN_PH_12               ,  4028063597           },
    { "ph_13",          TOKEN_PH_13               ,  4011285978           },
    { "ph_14",          TOKEN_PH_14               ,  3927397883           },
    { "ph_15",          TOKEN_PH_15               ,  3910620264           },
    { "put",            TOKEN_PUT                 ,  1769118190           },
    { "pwm",            TOKEN_GEN_PWM             ,  1315828287           },
    { "pwm_out",        TOKEN_PB_3                ,  776577380            },
    { "quit",           TOKEN_EXIT                ,  1200064310           },
    { "rem",            TOKEN_REM                 ,  1240764167           },
    { "round",          TOKEN_ROUND               ,  1326178875           },
    { "sec",            TOKEN_SEC                 ,  3139892658           },
    { "serial_rx",      TOKEN_PA_3                ,  459559496            },
    { "serial_tx",      TOKEN_PA_2                ,  527655710            },
    { "print",          TOKEN_PRINT               ,  372738696            },
    { "sin",            TOKEN_SIN                 ,  3761252941           },
    { "sizeof",         TOKEN_SIZE_OF_VAR         ,  1860254461           },
    { "spi_cs",         TOKEN_PB_6                ,  2820037140           },
    { "spi_miso",       TOKEN_PA_6                ,  2880368186           },
    { "spi_module",     TOKEN_SPI_MODULE          ,  1106696674           },
    { "spi_mosi",       TOKEN_PA_7                ,  928801494            },
    { "spi_sck",        TOKEN_PA_5                ,  1289356717           },
    { "spi_send",       TOKEN_SPI_SEND            ,  3637117612           },
    { "spi_single",     TOKEN_SPI_SINGLE          ,  536422558            },
    { "sqrt",           TOKEN_SQRT                ,  2112764879           },
    { "tan",            TOKEN_TAN                 ,  2633446552           },
    { "ticker",         TOKEN_TICKER              ,  1595386483           },
    { "usbrx",          TOKEN_PA_3                ,  4175791683           },
    { "usbtx",          TOKEN_PA_2                ,  82553173             },
    { "usec",           TOKEN_USEC                ,  1903720885           },
    { "user_button",    TOKEN_PC_13               ,  3141615303           },
    { "while",          TOKEN_WHILE               ,  231090382            },
    /*    Add here more instruction to the script language   */
    { NULL   ,           TOKEN_BAD                 ,     0                }
    };
#endif
