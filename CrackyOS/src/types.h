/**
 ******************************************************************************
 * @file    types.h
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   All script program decoding and analyzing done here.
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

#ifndef TYPES_H
#define TYPES_H

#include <mbed.h>

#ifndef NULL
#define NULL 0
#endif

/**
 * If this flag "CXX_FLAGS += -D_DEBUG=1" is not used,
 *  this code will not run
 *
 * */
#ifdef _DEBUG
#define DEBUG_MESSAGE(...) DEBUG_Print(__VA_ARGS__)
#else
#define DEBUG_MESSAGE(...)
#endif

#define MAX_HASH_STRING_LENGTH                                                 \
  20 // Be careful this could cause problem that is difficult to debug.
     // 2020-05-14
/** This function is to allow normal message to be sent to the serial port. This
   shouldn't be used as debug message. Use DEBUG_MESSAGE for that purpose. */
extern int SERIAL_Print(const char *format, ...);

/* We implement this "stop" to all function to avoid running any code
 *  after error and return back to main.
 *
 *      Stop running if we have error
 */

#define SERIAL_MESSAGE(...) SERIAL_Print(__VA_ARGS__)

#define STOP_CODE_EXECUTION_MINUS                                              \
  if (StopExecution == 1)                                                      \
    return -1;
#define STOP_CODE_EXECUTION                                                    \
  if (StopExecution == 1)                                                      \
    return;
#define STOP_CODE_EXECUTION_NULL                                               \
  if (StopExecution == 1)                                                      \
    return 0;

#define USED_VARIABLE unsigned char
#define SCRIPT_MAX_VARIABLE_NUMBERS 32
#define SCRIPT_MAX_NESTED_FOR_WHILE_LOOP 10 // TODO : How many is OK??

#define DUMMY_SPI_DUMMY 0XAA // Do we need these?? 2018-07-27
#define DUMMY_SPI_ACK 0X33   // Do we need these?? 2018-07-27

/** Max allowed size of I2C buffer*/
#define MAX_READ_BUFF_I2C 256

#define MAX_ALLOWED_MESSAGELENGTH 2 * 1024 // 2Kbytes

/** Function to check a specific bit in a variable.*/
#define checkBitInVariable(var, pos) ((var & (1 << pos)) == (1 << pos))

/** Serial communication class definition */

/** Class specialSerial definition */
class specialSerial : public UnbufferedSerial {
public:
  /**
   * Class specialSerial constructor
   * @param tx : Pinname TX UART GPIO
   * @param rx : Pinname RX UART GPIO
   * @param baud: BaudRate value
   * */
  specialSerial(PinName tx, PinName rx, int baud);
  /**
   * Return RX GPIO PinName
   * @return RX PinName
   */
  PinName retrieveRX();
  /**
   * Return TX GPIO PinName
   * @return TX PinName
   */
  PinName retrieveTX();

private:
  /**
   * Keeps RX PinName object
   * */

  PinName RX;
  /**
   * Keeps TX PinName object
   * */

  PinName TX;
};

/**
 * Struct definition for serial com ports.
 * */
typedef struct {
  specialSerial *Debug_pc;
  specialSerial *Serial_pc;
} SerialCommunication;

/**
 * Global variable keeps the com port objects
 * Both Serial and debug
 * */
extern SerialCommunication globalSerialCOMvar;

typedef struct ftick {
  Ticker *mTicker;  // Ticker object
  int interValInMs; // Interval in millisecond.
  void *pToFun(void);
} tickerVar;

typedef struct retFArr {
  float *arrayToReturn;
  int arraySize;
} returnedArray;
typedef struct {
  PwmOut *pwm;
  int enum_pinname;
} specialPwmOut;

typedef struct {
  specialPwmOut pwm1;
  specialPwmOut pwm2;
  specialPwmOut pwm3;
  specialPwmOut pwm4;
} pwmpinname;

typedef enum { T_NULL, T_FLOAT, T_INT, T_STRING } VarType;
typedef union {
  int *pi;        // Array of values
  float *pf;      // Array of values
  char *txtvalue; // Array of char
} objType;

/**
 *Linked list definition
 *This linked list is the main linked list used to keep the script program
 *instructions and other parts.
 */
typedef struct node {
  struct node *next;  // Next item in the list
  struct node *prev;  // Previous item in the list
  unsigned int index; // Position of the item in the list.. An unsigned int.
  char *txtString;
} node_t;

typedef node_t instr_linkedList;
/** struct to save function name and start/end
 * */
typedef struct fList {
  fList *prev;
  fList *next;
  char *name;
  instr_linkedList
      *start; // Should point to the first char of the arg1 or the '{' symbol
  instr_linkedList *end; // Do we need this????
} GlobalfunList;

// Dynamically created table that keeps pointer to each line in the source code
// for indexing
typedef struct tlistObj {
  tlistObj *prev;
  tlistObj *next;
  instr_linkedList *pLineStart;
  unsigned int lineNo;
} listObj;

/**Bus Reg class Definition*/
class reg {
public:
  /** Class reg constructor
   *
   * @param name A string represent the register/bus name
   * @param enumNames GPIO pin names .. An array of enum numbers.
   * */
  explicit reg(char *name, int conf, int *enumNames);

  reg(const reg &cpyreg) = delete;
  /**
   * Class reg destructor
   * */
  ~reg();
  /**
   * Class reg Copy operator
   * */
  void operator=(const reg &) = delete;

  /**
   * variable keeps the bus names in enum numbers.
   * */
  int enum_r_bit[16]; // Keep enum names of the pins

  /**
   * Pointer keeps the created object. MEM should be cleared as it is
   * dynamically created.
   * */
  BusInOut *bus;

  /** save configuration type and apply the configuration to the bus
   * @param type : Configure the bus as In or Out
   * */
  void configBus(unsigned char type);
  /**
   *      Hash number of the bus name.
   * */
  uint32_t _hashname;

protected:
  /**
   * Bus name
   * */
  char *_name;
  /**
   * Defines the bus type    0=In , 1 =Out
   * */
  unsigned char _config;
};

/** Struct to hold pin name and status (type : in =0, out =1)*/
typedef union {
  DigitalInOut *InOutPin;
  InterruptIn *interrupt_in;
  AnalogIn *aInPin;
  AnalogOut *aOutPin;
} generalPin;
typedef struct {
  int pinNameEnum;
  generalPin genInOut;
} pinGPIO;
/** Union between One GPIO and a bus*/
typedef union {
  pinGPIO *OnePinGPIO;
  reg *busGpio;
} _globalGPIO;

/** Struct to save label info to goto instruction*/
typedef struct {
  char *name;
  instr_linkedList *start;
  instr_linkedList *end;
} lblscript;
/** Global label struct/object to keep info about labels used in the script*/
typedef struct pLab {
  pLab *next;
  pLab *prev;
  lblscript label;
} glLBL;

/**
 * sturct definition for CAN object
 * */
typedef struct {
  int CANTD; // Enum name
  int CANRD; // Enum name
} can;

typedef struct canme {
  canme *prev;
  canme *next;
  can current;
  CAN *_CanDevice;
} canType;

/** Class definition for GPOI inside our script.
 *  This class will be used in a linked list
 *  It will keep the link to the function used for interruptIn
 * */
class GlobalGPclass {
public:
  /**
   * GlobalGPclass Class constructor
   * */
  GlobalGPclass();
  /**
   * GlobalGPclass Class destructor
   * */

  ~GlobalGPclass();
  /**
   * Functionality of the global GP
   * */
  int tag;
  _globalGPIO gpio;
  GlobalfunList *fun;
  //        instr_linkedList *funStart;  //This will keep a pointer to a
  //        function which will be called when interrupt happen.
  //        instr_linkedList *funEnd;  //This will keep a pointer to a function
  //        which will be called when interrupt happen.
private:
};

/** Link list of the object will be used. */
typedef struct Gnode {
  GlobalGPclass scripPin;
  Gnode *next;
  Gnode *prev;
} GlobalGP;

/**This is used for "for_loop instruction"*/
typedef struct tb {
  int *variable; // pointer to the global variable
  int value;     // variable used to save the value
  int position;  // global variable position, used to refresh the value safely
} while_variable_obj;

/**This is used for "for_loop instruction"*/
/**This is used for "for_loop instruction"*/
typedef struct {
  instr_linkedList
      *p_StartForLoop; // Pointer for the first line after for instruction
  instr_linkedList
      *p_EndForLoop; // Pointer for last line after for block of instructions
  int varIteration1; // Keeps the position of the first variable ( i.e. (for (
                     // here
  int varIteration2; // Keeps the position of the second variable ( i.e.
                     // (for(i=here,...
  int varIteration3; // Keeps the position of the third variable ( i.e. (for
                     // (i=0; i<here ...
  //    int varIteration4;                                       // Keeps the
  //    position of the third variable ( i.e. (for (i=0; i10, here...
  int enumLogicalOperation; // Keeps the logical operation inside the for-loop
                            // instruction
  int var3;                 // Keeps the value of the comparison
  int var4;                 // Keeps the +1 or -1 value for the iteration
} token_for_object;

/**This is used for "while_loop instruction"*/
typedef struct {
  instr_linkedList
      *p_StartWhileLoop; // Pointer for the first line after for instruction
  instr_linkedList
      *p_EndWhileLoop; // Pointer for last line after for block of instructions
  int varIteration; // Iteration  variable, the variable used inside the while.
                    // (for.ex while (j<k) .. here j is this variable.
  int varIterationPosition; // Global position of the left-hand variable.
  while_variable_obj
      varLogicalRelation; // After the logical symbol, this variable will be
                          // there or a number. (for.ex while (j<k) .. here K is
                          // this variable
  int enumLogicalOperation; // Keep the logical operation inside the for-loop
                            // instruction
} token_while_object;

/**This struct keep the name in text to enum */
typedef struct {
  const char *token_word; // text (string) corresponding the
                          // commands/instruction set token
  int token_enum; // STANDARD WORDS USED IN THE SCRIPTING PROGRAM (look at the
                  // enum in tokens.h)
  unsigned int hash;
} token_exchanger;

/**This is used for "if instruction"*/
typedef struct {
  instr_linkedList *pIfEnd;
  instr_linkedList *pElseEnd;
  float value1;
  float value2;
  int operation;
} ifInstruction;

/**This is used for interruptIn
 *
 * */
typedef struct intSt {
  int enumPinName;
  GlobalfunList *function; // pointer to the function inside the global function
                           // list (functionList variable)
  intSt *prev;             // Should point to the first line of block of code
  intSt *next;             // End line of block of code.
} gINTstruct;

/**
 * Global variable keeps the function list
 * */
extern GlobalfunList *functionList;

/**
 * Global variable used in tokenizetofile.cpp
 * */
extern instr_linkedList *scriptProg;

/** We cannot put these in the normal tokens ..
 * Otherwise we increase the time used for each search
 * Since these tokens uses only when you create a new GPIO ..
 * It is not so important to have it with the global tokens.
 * They will not be used much.
 * This enum is used with the GlobalGPIO_REG
 * */
enum {
  FTOKEN_PWM = 100,
  FTOKEN_REG,
  FTOKEN_DIGITAL_IN_OUT,
  FTOKEN_SERIAL_TX,
  FTOKEN_SERIAL_RX,
  FTOKEN_CAN_TD,
  FTOKEN_CAN_RD,
  FTOKEN_I2C_CLK,
  FTOKEN_I2C_SDA,
  FTOKEN_SPI_MOSI,
  FTOKEN_SPI_MISO,
  FTOKEN_SPI_CLK,
  FTOKEN_SPI_CS,
  FTOKEN_TICKER,
  FTOKEN_INTERRUPT_IN,
  FTOKEN_NC
};

/**
 * Tag values of each GPIO.
 * This enum is used wit the GlobalGP
 * */
enum {
  GPIO_PIN_IN = 0,
  GPIO_PIN_OUT,
  GPIO_IN_ANALOG,
  GPIO_OUT_ANALOG,
  GPIO_BUS_IN,
  GPIO_BUS_OUT,
  GPIO_TICKER,
  GPIO_INTERRUPTIN,
  GPIO_NC
};

/******************************************************************************************
******************************************************************************************

        Things to be checked
        1-PWM
        2-Reg
        3-Digital IN
        4-Digitl OUT
        5-Analog IN
        6-Analog OUT
        7-Serial
        8-CAN
        9-I2C
        10-SPI
        11-Ticker
        12-InterruptIn
        13-
    */

class ReNode {
public:
  /**
   * ReNode calss constructor
   * */
  ReNode();
  /**
   * ReNode calss detructor
   * */
  ~ReNode();

  /**
   * Variable keeps the GPIO name
   * */
  int _enumName;
  /**
   * Variable keeps functionality of the GPIO
   * */

  int _Functionality;

  /**
   * Pointer to the next node item
   * */
  ReNode *prev;

  /**
   * Pointer to the next node item
   * */
  ReNode *next;
};

/**
 * Class RegisterGPIO
 * Uses ReNode class to save functionality and GPIO name
 * */
class RegisterGPIO {
public:
  /**
   * Class RegisterGPIO constructor
   * */
  RegisterGPIO();
  /**
   * Class RegisterGPIO destructor
   * */
  ~RegisterGPIO();

  /**
   * Node object
   * */
  ReNode *_pNodeItem;
  /**
   *  add new GPIO to the linked list.
   *  @param enumName GPIO Name in enum representation
   *  @param function GPIO's functionality look at the enum   (FTOKEN_PWM,
   * ..etc)
   * */
  int add(int enumName, int function);
  /**
   * Find a GPIO in the linked list and return a pointer to the item.
   * @param enumName Name of the GPIO to find
   * @return A pointer to the found item or NULL
   * */
  ReNode *find(int enumName);
  /**
   * Remove a GPIO from the linked list
   * @param t A node
   * */
  void remove(ReNode **t);
  /**
   * This function will free up the memory for the GPIO used before if it
   * dosen't have the same functionality. SERIAL, SPI MASTER and REG is not
   * allowed Return 1 if removed Return 0 if not removed .. It has the same
   * functionality Return -1 if not found.
   *
   * */
  int gpioFreeMem(int enumName, int function);
  /**
   * Point back to the first item in the linked list.
   * */
  void goToFirstItem(void);
};

/**
 * Class SourCodeLine definitions
 * */
class SourceCodeLine {
public:
  /**
   * Class SourceCodeLine Constructor
   * */
  SourceCodeLine();
  /**
   * Class SourceCodeLine Constructor
   * @param pSourceCodeText : Linkedlistpointer to the script program loaded
   * from sdcard.
   * */
  explicit SourceCodeLine(instr_linkedList *pSourceCodeText);

  /**
   * Class SourceCodeLine destructor
   * */
  ~SourceCodeLine();
  /**
   * This function will return a pointer to the list element. You can use it to
   *get line number of the active instruction
   *@param pCurrent : A pointer to a instruction or arguments that we need to
   *find the linenumber
   * */
  listObj *findPointer(instr_linkedList *pCurrent);
  /**
   * This function create linenumbers associated with each instruction line. Use
   * it in the begining to create the listed link
   *
   * */
  void scanSourceCode();

protected:
  /**
   * This function adds a new item to the listedlink.
   * @param pPosition a new pointer to instruction or argument that will be
   * saved in the
   * */
  void addNewLine(instr_linkedList *pPosition);

private:
  /**
   * Local pointer-variable keeps the class object
   * */
  listObj *m_TableOfPointer;
  /**
   * Local variable points to the source code.
   * */
  instr_linkedList *m_psourceCode;

  /**
   * Local variable points to the first item in the linked list.
   * */

  listObj *mfirst;
};
/**
 * struct definition for can obj and name
 * */

void add_Reserved_GPIO(void);

/** Search in the list for a specific reg*/
GlobalGP *findReg(char *);

/** Delete a named register bus and release its pin registrations. */
int deleteReg(char *name);

/** Add new digital in/out */
GlobalGP *addGPIOpin_DIGITAL(int enumPinName, int type);

/** Search in the list for a specific GPIO pin*/
GlobalGP *findGPIOpin(int enumPinName);

/** If you give a name to the function it will delete only that GPIO variable,
 *if you give NULL it will delete all
 *@param  enumGPIOname : Variable to delete from memory, if =NULL, all variables
 *will be deleted
 */
void free_scriptGPIOVariable(void);

/** free up the internal parts of one gpio variable*/
void freeOneGPIOVariable(GlobalGP **target);

/** free up specific gpio variable*/
void free_scriptGPIOVariableByGlobalName(GlobalGP *GPIOname);

/** Add new digital in/out */
GlobalGP *addGPIOpin_DIGITAL(int enumPinName, int type);

/** Class lobalVariables*/
class GlobalVariables {
public:
  /**
   * Class GlobalVariables Constructor
   * */
  GlobalVariables();

  /**
   * Class GlobalVariables Destructor
   * */
  ~GlobalVariables();
  /**
   * Variable type
   * */
  VarType tag; // Variable type

  /**
   * Variable name in text
   * */
  char *varname; // Variable Name
  /**
   * Variable name in hash number
   * */
  uint32_t hashVarnme; // Variable name in hash presentatin.
  /**
   * Value object int, float or text
   * */
  objType Value; // Value
  /**
   * If the variable is an array .. this value will be >0
   * */
  int sizeOfArray; // Size of the array (if any)

  /**
   * Remove integer variable
   * */
  void removePI();

  /**
   * Remove float variable
   * */
  void removePF();

  /**
   * Remove Text variable
   * */
  void removeTXTString();

  /**
   * Remove variable name (text)
   * */

  void removeVarname();
};

/** This variable will keep the baudrate .. Default is 115200.
 */
extern int baud_rate;
extern int current_token_word;

extern osThreadId_t mainThreadID;
/** Global variable that keeps the label positions and names.*/
extern glLBL *globallabelVar;

/**This will activate or deactivate debug messages to RS232      1=DEBUG
 * 0=NO-DEBUG MESSAGES*/
extern unsigned char InterActiveMode;

/**This will activate or deactivate debug messages to RS232      1=DEBUG
 * 0=NO-DEBUG MESSAGES*/
extern unsigned char StopExecution;

/**
 * This function will register systems GPIO that are in the architecture of the
 * system. example of these are SPI, Serial, I2C ..etc
 * */
extern RegisterGPIO *GlobalGPIO_REG; // This will be used to keep track of all
                                     // GPIO's functionality

/**
 * Global pointer keeps the CAN variable.
 * */
extern canType *gCanVariable;

/**
 * Global variable keeps the GPIO variables
 * */
extern GlobalGP *globalGPIOvariable;

/*Variable will be used as temp. holder of the executable function .. at the
 * time interrupt happens. This variable is always = NULL if not an interruptin
 * happens.
 * */
extern GlobalfunList *varGlobActiveInterruptFunc;

/**
 * Linked list keeps the interruptIn pins and their function pointers.
 * */
extern gINTstruct *GlobLinkedListInterruptIn;

/** Labels will be kept here*/
extern glLBL *globallabelVar;

/**All token names are saved in this list */
extern const token_exchanger token_exchange_list[];

/**Array of variables to keep track of for_loop instruction*/
/**FOR -LOOP INSTRUCTION WILL BE ACCEPTED ONLY IF THEY ARE 2 NESTED FOR-LOOP. NO
 * MORE. FOR SIMPLICITY */
extern token_for_object *forExtract;

/**WHILE -LOOP INSTRUCTION WILL BE ACCEPTED ONLY IF THEY ARE
 * '=SCRIPT_MAX_NESTED_FOR_WHILE_LOOP' NESTED FOR-LOOP. NO MORE. FOR
 * SIMPLICITY*/
extern token_while_object *whileExtract;

/** This variable will count the no. of for-loop instructions inside a block*/
extern unsigned char nestedForloop;
/** This variable will count the no. of while-loop instructions inside a block*/
extern unsigned char nestedWhileloop;
/** This will be used when we have for & while loops used inside each other. */
extern unsigned char nestedForWhileloop;

/** Global variable will be used to keep the name, value of all variable used in
 * the script program*/
/** Max allowed number of variables per script is defined by
 * SCRIPT_MAX_VARIABLE_NUMBERS*/
extern GlobalVariables *script_variables;

/** Pointers to read the code. Max no of instructions is what "int" give us */
extern instr_linkedList *p_prog, *p_prog_next;

/** This variable will keep the current token word found in the script program*/
extern int current_token_word;

/** Variable used to keep track of the SD-Card script file  */
extern FILE *testProgw;
/** This class is used to keep the line number of the script program*/
extern class SourceCodeLine *pScriptIndex;

/** PWM Variable  - used to keep track of 4 channel PWM signal pin names and
 * period/duration values*/
extern pwmpinname PWM_Var;

/** Actual replacement of printf - this uses the ringbuffer*/
extern int DEBUG_Print(const char *format, ...);
/** This variable is used to keep the status of debug messge enable
 *   if DEBUG_MESSAGE_ENABLE=0   No message will be send to the termianl
 *      DEBUG_MESSAGE_ENABLE=0   Debug Message will be delivered to terminal
 */
extern unsigned char DEBUG_MESSAGE_ENABLE;

/**
 * Break pressed variable
 * */
extern unsigned char BreakPressed;

/** Thread that take care of debug messages to terminal*/
extern Thread *debugThread;

int enable_disableDebugThread(void);

/* This function will remove the extra '\' from the following commands that
found in the script program sent to the system. Without that, they will they
will be shown as they are and the commands they represent will not be executed
by printf.
*        \'  single quote
*        \"  double quote
*        \?  question mark
*        \\  backslash
*        \a  audible bell
*        \b  backspace
*        \f  form feed -
*        \n  line feed -
*        \r  carriage return
*        \t  horizontal tab
*        \v  vertical tab
*@param  InputString : message should be fixed
*@return corrected string.
*/
char *correctString(const char *InputString);
/** Sub thread to take care of debug serial communications
 *@param : No parameter needed*/
void debugThreadFunction(void);
/**
 *Create the port .. As it may vari between boards, we
 *should have it like this.
 */
int createDebugPC();

/** Quit the running of the program. Depending value received as parameter
 *
 *@param  exitType  If exitType=1 it will finish the execution, if exitType=0 it
 *will return to the caller function
 */
int quitScriptProgram(unsigned char exitType);

/**
 * ERROR and INFO CODE enum. Use this enum to interpret INFO/ERROR codes
 * you find in the debug terminal.
 *
 * */
enum DebugInfo {
  DBG_MSG_AGET_END = 1010,
  DBG_MSG_AGET_START = 1020,
  DBG_MSG_APUT_END = 1030,
  DBG_MSG_APUT_START = 1040,
  DBG_MSG_ARITHMETIC_EXP_EXECUTED = 1050,
  DBG_MSG_ARRAY_ITEM_NOT_FOUND = 1060,
  DBG_MSG_ARRAY_VAR_NOT_FOUND = 1070,
  DBG_MSG_BAD_SCRIPT_END_DIRECTLY = 1080,
  DBG_MSG_BAD_VARIABLE_NAME = 1090,
  DBG_MSG_BAUD_RATE_COMMAND_EXECUTED = 1100,
  DBG_MSG_BAUD_RATE_END = 1110,
  DBG_MSG_BAUD_RATE_START = 1120,
  DBG_MSG_CALCULATE_RIGHT_SUIDE_OF_EQUATION = 1130,
  DBG_MSG_CALL_END = 1140,
  DBG_MSG_CALL_START = 1150,
  DBG_MSG_CANNOT_OPEN_FILE = 1160,
  DBG_MSG_CAN_GET_END = 1170,
  DBG_MSG_CAN_GET_START = 1180,
  DBG_MSG_CAN_SEND_END = 1190,
  DBG_MSG_CAN_SEND_START = 1200,
  DBG_MSG_CLEAR_END = 1210,
  DBG_MSG_CLEAR_START = 1220,
  DBG_MSG_COMMA_NOT_FOUND = 1230,
  DBG_MSG_COMPARISON_LOGIC_OPERATION_STARTED = 1240,
  DBG_MSG_CREATE_REG_END = 1250,
  DBG_MSG_CREATE_REG_START = 1260,
  DBG_MSG_DAD_FILE_CORRUPTED = 1270,
  DBG_MSG_DATA_VARIABLE_ARGUMENT_NOT_FOUND = 1280,
  DBG_MSG_DELAY_START = 1290,
  DBG_MSG_DELAY_END = 1300,
  DBG_MSG_DOUBLE_OR_SYMBOL_REQUIRED = 1310,
  DBG_MSG_END_OF_PROG = 1320,
  DBG_MSG_END_OF_SCRIPT_LINE = 1330,
  DBG_MSG_END_OF_WHILE_LOOP = 1340,
  DBG_MSG_BUS_ALREADY_DEFINED = 1350,
  DBG_MSG_ERROR_CANNOT_CREATE_FILE = 1360,
  DBG_MSG_ERROR_CANNOT_OPEN_DAD_FILE = 1370,
  DBG_MSG_ERROR_FATAL_MISMATCHING_TOKENWORD = 1380,
  DBG_MSG_ERROR_FATAL_WE_SHOULDNT_BE_HERE0 = 1390,
  DBG_MSG_ERROR_FATAL_WE_SHOULDNT_BE_HERE1 = 1391,
  DBG_MSG_ERROR_FATAL_WE_SHOULDNT_BE_HERE2 = 1392,
  DBG_MSG_ERROR_FATAL_WE_SHOULDNT_BE_HERE3 = 1393,
  DBG_MSG_ERROR_INVALID_ARRAY_INDEX = 1400,
  DBG_MSG_ERROR_ITEM_NOT_FOUND = 1410,
  DBG_MSG_ERROR_MAX_STRINGLENGTH = 1420,
  DBG_MSG_ERROR_NESTEDFORLOOP_NESTEDWHILELOOP_NOT_ZERO = 1430,
  DBG_MSG_ERROR_NO_PROGRAM = 1440,
  DBG_MSG_ERROR_OVERFLOW = 1450,
  DBG_MSG_ERROR_READ_ERROR = 1460,
  DBG_MSG_ERROR_SCRIPT_VARIABLE_OVERFLOW = 1470,
  DBG_MSG_ERROR_VAR_OVERFLOW = 1480,
  DBG_MSG_FOR_LOOP_END = 1490,
  DBG_MSG_FOR_LOOP_START = 1500,
  DBG_MSG_FUNCTION_END = 1510,
  DBG_MSG_FUNCTION_START = 1520,
  DBG_MSG_GET_INST_END = 1530,
  DBG_MSG_GET_INST_START = 1540,
  DBG_MSG_GOTO_END = 1550,
  DBG_MSG_GOTO_START = 1560,
  DBG_MSG_I2C_GET_END = 1570,
  DBG_MSG_I2C_GET_START = 1580,
  DBG_MSG_I2C_SEND_END = 1590,
  DBG_MSG_I2C_SEND_START = 1600,
  DBG_MSG_IF_END = 1610,
  DBG_MSG_IF_START = 1620,
  DBG_MSG_INSTRUCTION_EXECUTION_FINISHED = 1630,
  DBG_MSG_LABEL_END = 1640,
  DBG_MSG_LISTED_LINK_NOT_FOUND = 1650,
  DBG_MSG_MEM_ALLOCATION_ERROR = 1660,
  DBG_MSG_MEM_ALLOCATION_ERROR_BUFIFSTATMENT = 1670,
  DBG_MSG_MEM_ALLOCATION_ERROR_CALCULATE_LEFTSIDE = 1680,
  DBG_MSG_MEM_ALLOCATION_ERROR_CALCULATE_RIGHTSIDEOFVAREQUATION = 1690,
  DBG_MSG_MEM_ALLOCATION_ERROR_CAN_SEND = 1700,
  DBG_MSG_MEM_ALLOCATION_ERROR_CAN_GET = 1710,
  DBG_MSG_MEM_ALLOCATION_ERROR_CONCAT_FUNCTION = 1720,
  DBG_MSG_MEM_ALLOCATION_ERROR_FOREXTRACT = 1730,
  DBG_MSG_MEM_ALLOCATION_ERROR_FORTEMPBUF = 1740,
  DBG_MSG_MEM_ALLOCATION_ERROR_FOR_LOOP = 1750,
  DBG_MSG_MEM_ALLOCATION_ERROR_FUNCTIONLIST = 1760,
  DBG_MSG_MEM_ALLOCATION_ERROR_FUNCTION_CALL = 1770,
  DBG_MSG_MEM_ALLOCATION_ERROR_FUNCTION_NAME = 1780,
  DBG_MSG_MEM_ALLOCATION_ERROR_GAP_SCAN = 1790,
  DBG_MSG_MEM_ALLOCATION_ERROR_GENINOUT_AOUTPIN = 1800,
  DBG_MSG_MEM_ALLOCATION_ERROR_GET_INST = 1810,
  DBG_MSG_MEM_ALLOCATION_ERROR_GET_SERIAL = 1820,
  DBG_MSG_MEM_ALLOCATION_ERROR_GLOBAL_GPIO = 1830,
  DBG_MSG_MEM_ALLOCATION_ERROR_GLOBAL_VAR = 1840,
  DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_BUS = 1850,
  DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_INOUT = 1860,
  DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_PIN = 1870,
  DBG_MSG_MEM_ALLOCATION_ERROR_I2CSLAVE = 1880,
  DBG_MSG_MEM_ALLOCATION_ERROR_I2CVAR = 1890,
  DBG_MSG_MEM_ALLOCATION_ERROR_I2C_GET = 1900,
  DBG_MSG_MEM_ALLOCATION_ERROR_PWM_VAR = 1910,
  DBG_MSG_MEM_ALLOCATION_ERROR_READTOLINKEDLIST = 1920,
  DBG_MSG_MEM_ALLOCATION_ERROR_RINGBUF = 1930,
  DBG_MSG_MEM_ALLOCATION_ERROR_SCRIPT_VAR = 1940,
  DBG_MSG_MEM_ALLOCATION_ERROR_SERIAL_BUFF = 1950,
  DBG_MSG_MEM_ALLOCATION_ERROR_SERIAL_GET = 1960,
  DBG_MSG_MEM_ALLOCATION_ERROR_SERIAL_SEND = 1970,
  DBG_MSG_MEM_ALLOCATION_ERROR_SPIVAR = 1980,
  DBG_MSG_MEM_ALLOCATION_ERROR_TABLEOFPOINTER = 1990,
  DBG_MSG_MEM_ALLOCATION_ERROR_TOKENIZER = 2000,
  DBG_MSG_MEM_ALLOCATION_ERROR_VAR = 2010,
  DBG_MSG_MEM_ALLOCATION_ERROR_VAR_BUF = 2020,
  DBG_MSG_MEM_ALLOCATION_ERROR_WHILEEXTRACT = 2030,
  DBG_MSG_MEM_ALLOCATION_ERROR_WHILE_LOOP = 2040,
  DBG_MSG_NEEDS_EQUAL_SIGN = 2050,
  DBG_MSG_NESTED_FOR_LOOP = 2060,
  DBG_MSG_NESTED_WHILE_LOOP = 2070,
  DBG_MSG_NOT_END_OF_PROG = 2080,
  DBG_MSG_NO_EMPTY_GLOBAL_VAR_FOUND = 2090,
  DBG_MSG_NULL_POINTER_SEND_TO_READSDCARD = 2100,
  DBG_MSG_OBJECT_ALLOCATION_ERROR_BUS = 2110,
  DBG_MSG_OBJECT_ALLOCATION_ERROR_CAN = 2120,
  DBG_MSG_PUT_END = 2130,
  DBG_MSG_PUT_START = 2140,
  DBG_MSG_P_PROG_WAS_NULL = 2150,
  DBG_MSG_REACHED_INSTRU_EXECUTIUON = 2160,
  DBG_MSG_REG_PUT = 2170,
  DBG_MSG_REM_END = 2180,
  DBG_MSG_REM_START = 2190,
  DBG_MSG_RETURN_AFTER_FOR_LOOP_EXEC = 2200,
  DBG_MSG_RUN_SCRIPT_PROG = 2210,
  DBG_MSG_SERIAL_GET_END = 2220,
  DBG_MSG_PRINT_END = 2230,
  DBG_MSG_SPI_SEND_END = 2240,
  DBG_MSG_SPI_SEND_START = 2250,
  DBG_MSG_START_SCRIPT = 2260,
  DBG_MSG_SYNTAX_ERROR_ARGUMENT_APUT = 2270,
  DBG_MSG_SYNTAX_ERROR_ARGUMENT_VAR_NOT_FOUND = 2280,
  DBG_MSG_SYNTAX_ERROR_ARGUMENT_VAR_NOT_FOUND_AGET = 2290,
  DBG_MSG_SYNTAX_ERROR_ARGUMENT_VAR_NOT_FOUND_CANWRITE = 2300,
  DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND0 = 2310,
  DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND1 = 2311,
  DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND2 = 2312,
  DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND3 = 2313,
  DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND4 = 2314,
  DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND5 = 2315,
  DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND6 = 2316,
  DBG_MSG_SYNTAX_ERROR_ARRAY_ITEM_NOT_FOUND7 = 2317,

  DBG_MSG_SYNTAX_ERROR_ARRAY_USED_INITIALIZE_FOR_LOOP = 2320,
  DBG_MSG_SYNTAX_ERROR_BAUDRATE_NUMBER_NOT_FOUND = 2330,
  DBG_MSG_SYNTAX_ERROR_BUS_DOES_NOT_SUPPORT_ANALOG_GET = 2340,
  DBG_MSG_SYNTAX_ERROR_BUS_DOES_NOT_SUPPORT_ANALOG_OUT = 2350,
  DBG_MSG_SYNTAX_ERROR_BUS_NOT_FOUND = 2360,
  DBG_MSG_SYNTAX_ERROR_CANTD_WRONG = 2370,
  DBG_MSG_SYNTAX_ERROR_CANRD_WRONG = 2380,
  DBG_MSG_SYNTAX_ERROR_CAN_ADDRESS_NOT_FOUND = 2390,
  DBG_MSG_SYNTAX_ERROR_DATASIZE_NOT_FOUND = 2400,
  DBG_MSG_SYNTAX_ERROR_DBOULEQUATIOAN_REQUIRED = 2410,
  DBG_MSG_SYNTAX_ERROR_DEFINE_ADD_ALLOWED_STRING_VAR = 2420,
  DBG_MSG_SYNTAX_ERROR_DELAY_NOT_DEFINED = 2430,
  DBG_MSG_SYNTAX_ERROR_DELAY_VALUE_NOT_FOUND = 2440,
  DBG_MSG_SYNTAX_ERROR_ELEMENT_NOT_FOUND = 2450,
  DBG_MSG_SYNTAX_ERROR_EQUAL_NOT_FOUND = 2460,
  DBG_MSG_SYNTAX_ERROR_BAD_INSTRUCTION_EQUAL_NOT_FOUND = 2470,
  DBG_MSG_SYNTAX_ERROR_FLOAT_INSIDE_LOOP = 2480,
  DBG_MSG_SYNTAX_ERROR_FLOAT_USED_AS_INDEX_FOR_ARRAY = 2490,
  DBG_MSG_SYNTAX_ERROR_FOR_LOOP_COMPARISION_BETWEEN_UNAVAILABLE_VAR = 2500,
  DBG_MSG_SYNTAX_ERROR_FOR_LOOP_INITIALIZE = 2510,
  DBG_MSG_SYNTAX_ERROR_FOR_LOOP_THIRDPART_MISSING = 2520,
  DBG_MSG_SYNTAX_ERROR_FOR_LOOP_VARIABLE_NOT_FOUND = 2530,
  DBG_MSG_SYNTAX_ERROR_FUNCTION_DEF = 2540,
  DBG_MSG_SYNTAX_ERROR_FUNCTION_NAME_NOT_FOUND = 2550,
  DBG_MSG_SYNTAX_ERROR_GOTO_WRONG = 2560,
  DBG_MSG_SYNTAX_ERROR_GPIO_NOT_FOUND = 2570,
  DBG_MSG_SYNTAX_ERROR_I2C_ADDRESS_NOT_FOUND = 2580,
  DBG_MSG_SYNTAX_ERROR_I2C_DATALENGTH_GRATERTHAN_ARRAY = 2590,
  DBG_MSG_SYNTAX_ERROR_I2C_DATALENGTH_NOT_FOUND = 2600,
  DBG_MSG_SYNTAX_ERROR_I2C_VAR_NOT_FOUND = 2610,
  DBG_MSG_SYNTAX_ERROR_IF = 2620,
  DBG_MSG_SYNTAX_ERROR_IF_UNKNOWN = 2630,
  DBG_MSG_SYNTAX_ERROR_INDEX_INSIDE_ARRAY_WRONG = 2640,
  DBG_MSG_SYNTAX_ERROR_INIT_VAR_FOR_LOOP_NOT_FOUND = 2650,
  DBG_MSG_SYNTAX_ERROR_INIT_VAR_NOT_FOUND = 2660,
  DBG_MSG_SYNTAX_ERROR_INT_OR_FLOAT_NOT_FOUND = 2670,
  DBG_MSG_SYNTAX_ERROR_LABEL_NOT_FOUND = 2680,
  DBG_MSG_SYNTAX_ERROR_LEFT_CURLY_BRACKET_NOT_FOUND = 2690,
  DBG_MSG_SYNTAX_ERROR_LEFT_PARENTHESIS = 2700,
  DBG_MSG_SYNTAX_ERROR_LEFT_PARENTHESIS_MISSING = 2710,
  DBG_MSG_SYNTAX_ERROR_LOGICAL_OPERATION_FOR_LOOP = 2720,
  DBG_MSG_SYNTAX_ERROR_NOT_INT_NUMBER_INC = 2730,
  DBG_MSG_SYNTAX_ERROR_NOT_SUPPORTED = 2740,
  DBG_MSG_SYNTAX_ERROR_NO_COMMA_FOR_LOOP = 2750,
  DBG_MSG_SYNTAX_ERROR_NO_EQUAL_SIGN = 2760,
  DBG_MSG_SYNTAX_ERROR_NO_LCURLY_FOUND = 2770,
  DBG_MSG_SYNTAX_ERROR_ONLY_2_NESTED_LOOP_ALLOWED = 2780,
  DBG_MSG_SYNTAX_ERROR_ONLY_4_PWM_ALLOWED = 2790,
  DBG_MSG_SYNTAX_ERROR_PUT_REG = 2800,
  DBG_MSG_SYNTAX_ERROR_PWM_PERIOD_NOT_FOUND = 2810,
  DBG_MSG_SYNTAX_ERROR_PWM_PINNAME_WRONG = 2820,
  DBG_MSG_SYNTAX_ERROR_PWM_PULSWIDTH_NOT_FOUND = 2830,
  DBG_MSG_SYNTAX_ERROR_QUOTATION_MISSING = 2840,
  DBG_MSG_SYNTAX_ERROR_REG_PIN_CANNOT_REASIGNS = 2850,
  DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_MISSING = 2860,
  DBG_MSG_SYNTAX_ERROR_RIGHT_CURLY_BRACKET_NOT_FOUND = 2870,
  DBG_MSG_SYNTAX_ERROR_RIGHT_PARENTHESIS_MISSING = 2880,
  DBG_MSG_SYNTAX_ERROR_RIGHT_SIDE_OF_VAR_EQUATION = 2890,
  DBG_MSG_SYNTAX_ERROR_RIGHT_SQUARE_BRACKET = 2900,
  DBG_MSG_SYNTAX_ERROR_RIGHT_SQUARE_BRACKET_MISSING = 2910,
  DBG_MSG_SYNTAX_ERROR_SCRIPT_PROG_EMPTY = 2920,
  DBG_MSG_SYNTAX_ERROR_SERIAL_PIN_CANNOT_REASIGNS = 2930,
  DBG_MSG_SYNTAX_ERROR_SERIAL_SPI_CANNOT_REASIGNS = 2940,
  DBG_MSG_SYNTAX_ERROR_SPI_12BIT_STRING_NOT_SUPPORTED = 2950,
  DBG_MSG_SYNTAX_ERROR_SPI_ADDRESS_NOT_FOUND = 2960,
  DBG_MSG_SYNTAX_ERROR_SPI_DATA_NOT_FOUND = 2970,
  DBG_MSG_SYNTAX_ERROR_SPI_DATA_VAR_NOT_FOUND = 2980,
  DBG_MSG_SYNTAX_ERROR_SPI_FLOAT_NOT_SUPPOIRTED = 2990,
  DBG_MSG_SYNTAX_ERROR_SPI_FREQ_NOT_FOUND = 3000,
  DBG_MSG_SYNTAX_ERROR_SPI_MASTER_OR_SLAVE_NOT_FOUND = 3010,
  DBG_MSG_SYNTAX_ERROR_SPI_MODE_NOT_FOUND = 3020,
  DBG_MSG_SYNTAX_ERROR_STRING_ARRAY_NOT_NUMBER = 3030,
  DBG_MSG_SYNTAX_ERROR_TOKEN_NOT_FOUND_IN_RETRIVE_PIN_NAME = 3040,
  DBG_MSG_SYNTAX_ERROR_TOKEN_SIZE_IS_BIG = 3050,
  DBG_MSG_SYNTAX_ERROR_USE_ANOTHER_VARIABLE = 3060,
  DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND = 3070,
  DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND_DELAY = 3080,
  DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND_GET = 3090,
  DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND_IF = 3100,
  DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND_SERIAL_GET = 3110,
  DBG_MSG_SYNTAX_ERROR_VAR_NOT_INITIALIZED = 3120,
  DBG_MSG_SYNTAX_ERROR_VAR_NOT_NUMBER = 3130,
  DBG_MSG_SYNTAX_ERROR_VAR_NOT_USED_INSIDE_WHILE = 3140,
  DBG_MSG_SYNTAX_ERROR_VAR_WRONG = 3150,
  DBG_MSG_SYNTAX_ERROR_WHILE = 3160,
  DBG_MSG_SYNTAX_ERROR_WRONG_GPIO = 3170,
  DBG_MSG_SYNTAX_ERROR_WRONG_GPIO_CAN = 3180,
  DBG_MSG_SYNTAX_ERROR_WRONG_VAR = 3190,
  DBG_MSG_SYNTAX_LEFT_PARENTHESIS_NOT_FOUND = 3200,
  DBG_MSG_SYNTAX_TOKEN_RIGHT_CURLY_BRACKET_NOTFOUND = 3210,
  DBG_MSG_SYNTAX_VAR_MISSING_I2C = 3220,
  DBG_MSG_TO_LOWER_END = 3230,
  DBG_MSG_TO_LOWER_START = 3240,
  DBG_MSG_TO_UPPER_END = 3250,
  DBG_MSG_TO_UPPER_START = 3260,
  DBG_MSG_VARIABLE_END = 3270,
  DBG_MSG_VARIABLE_NOT_FOUND = 3280,
  DBG_MSG_VARIABLE_START = 3290,
  DBG_MSG_WARNING_ONLY_ONE_ITEM_FOUND = 3300,
  DBG_MSG_WARNING_SPI_DELAY_NOT_DEFINED = 3310,
  DBG_MSG_WHILE_LOOP_END = 3320,
  DBG_MSG_WHILE_LOOP_START = 3330,
  DBG_SYNTAX_ERROR_BUS_NOT_FOUND = 3340,
  DBG_SYNTAX_ERROR_SPI_ADDRESS_NOT_FOUND = 3350,
  DBG_SYNTAX_ERROR_TOKEN_NOT_FOUND = 3360,
  DBG_MSG_DELETE_START = 3370,
  DBG_MSG_DELETE_END = 3380,
  DBG_MSG_REG_CLASS = 3390,
  DBG_MSG_TICKER_END = 3400,
  DBG_MSG_INPUT_INTERRUPT_END = 3410,
  DBG_MSG_FAILED_TO_OPEN_DIR = 3420,
  DBG_MSG_MEM_ALLOCATION_ERROR_TICKER = 3430,
  DBG_MSG_MEM_ALLOCATION_ERROR_TICKER_DIGITALOUT = 3440,
  DBG_MSG_MEM_ALLOCATION_FLIPPER = 3450,
  DBG_MSG_TICKER_STATUS = 3460,
  DBG_MSG_TICKER_PINNAME = 3470,
  DBG_MSG_TICKER_INTERVAL = 3480,
  DBG_MSG_TICKER_START_STOP = 3490,
  DBG_MSG_TICKER_OBJECT_DELETION_NOT_FOUND = 3500,
  DBG_MSG_SYNTAX_ERROR_TICKER = 3510,
  DBG_MSG_ERROR_HASH_NAME = 3520,
  DBG_MSG_INTERRUPT_IN_PINNAME_WRONG = 3530,
  DBG_MSG_INTERRUPT_IN_FUNCTION_MISSING = 3540,
  DBG_MSG_INTERRUPT_IN_GPIO_NOTFOUND = 3550,
  DBG_MSG_INTERRUPT_MEM_ALLOCATION_FAILED = 3560,
  DBG_MSG_SYNTAX_ERROR_SIZE_OF_VAR = 3570,
  DBG_MSG_SYNTAX_ERROR_PRINT0 = 3580,
  DBG_MSG_SYNTAX_ERROR_PRINT1 = 3581,
  DBG_MSG_SYNTAX_ERROR_PRINT2 = 3582,
  DBG_MSG_PRINT_SYNTAX_NOT_SUPPORTED = 3583
};

#endif
