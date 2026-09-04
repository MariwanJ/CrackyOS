/**
 ******************************************************************************
 * @file    digital.cpp
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief   This file contains Digital IO instructions and definitions.
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
#include "types.h"
/** Global list variable to keep the pin names/bus of GPIO. Create objects
 * dynamically. */
GlobalGP *globalGPIOvariable = NULL;

/** Class reg constructor
 *
 * @param name A string represent the register/bus name
 * @param enumNames GPIO pin names .. An array of enum numbers.
 * */
reg::reg(char *name, int conf, int *enumNames) {
  /* if a pin is not defined, send -1 or NC to the class*/
  bus = NULL;
  _name = name;
  _config = conf;
  if (enumNames == NULL) {
    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_REG_CLASS);
    quitScriptProgram(1);
    return;
  }
  for (int i = 0; i < 16; i++) {
    enum_r_bit[i] = enumNames[i];
    if (enumNames[i] != TOKEN_NC) {
      if (GlobalGPIO_REG->add(enumNames[i], FTOKEN_REG) < 0) {
        for (int j = 0; j < i; j++) {
          if (enum_r_bit[j] != TOKEN_NC) {
            ReNode *addedNode = GlobalGPIO_REG->find(enum_r_bit[j]);
            GlobalGPIO_REG->remove(&addedNode);
          }
        }
        DEBUG_MESSAGE("ERROR %i\n",
                      DBG_MSG_SYNTAX_ERROR_REG_PIN_CANNOT_REASIGNS);
        quitScriptProgram(1);
        return;
      }
    }
  }

  bus = new BusInOut(retrievePinNameFromTOKEN_NAME(enum_r_bit[0]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[1]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[2]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[3]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[4]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[5]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[6]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[7]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[8]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[9]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[10]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[11]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[12]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[13]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[14]),
                     retrievePinNameFromTOKEN_NAME(enum_r_bit[15]));
  if (bus == NULL) {
    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_OBJECT_ALLOCATION_ERROR_BUS);
  }
  configBus(_config); // Configure the bus for in or out
  _hashname = CalculateHashOfString(_name, strlen(_name));
}

/** save configuration type and apply the configuration to the bus
 * @param type : Configure the bus as In or Out
 * */
void reg::configBus(unsigned char type) {
  _config = type;
  if (_config == GPIO_BUS_IN) {
    /***/
    bus->input();
  } else {
    bus->output();
  }
}

/** reg class destructor*/
reg::~reg() {
  /*clean up*/
  delete bus;
  bus = NULL;
  if (_name)
    delete[] _name;
  _name = NULL;
}

/** Find a register ..
 *  @param Searchname       if Searchname==NULL it will collect the search name
 * from p_prog.
 */
GlobalGP *findReg(char *Searchname) {
  // Stop running if we have error
  STOP_CODE_EXECUTION_NULL
  unsigned int hashName = 0;
  unsigned char found = 1;
  GlobalGP *p_scan = globalGPIOvariable;
  if (p_scan == NULL) {
    return NULL;
  }
  hashName =
      CalculateHashOfString(p_prog->txtString, strlen(p_prog->txtString));
  if (p_scan->next == NULL && p_scan->prev == NULL) {
    // We have only one item.
    found = 0;
    if (p_scan->scripPin.tag == GPIO_BUS_IN ||
        p_scan->scripPin.tag == GPIO_BUS_OUT) {
      if (p_scan->scripPin.gpio.busGpio->_hashname == hashName)
        found = 1;
    }
  } else {
    while (p_scan->prev != NULL) {
      p_scan = p_scan->prev; // find the first item.
    }
    while (1) {
      if ((p_scan->scripPin.tag == GPIO_BUS_IN ||
           p_scan->scripPin.tag == GPIO_BUS_OUT) &&
          p_scan->scripPin.gpio.busGpio->_hashname == hashName) {
        found = 1;
        break; // Go out .. we found it.
      }
      if (p_scan->next == NULL) {
        found = 0;
        break; // go out
      }
      p_scan = p_scan->next;
    }
  }
  if (found == 1)
    return p_scan;
  else
    return NULL; // not found.
}

int deleteReg(char *name) {
  GlobalGP *target = findReg(name);
  if (target == NULL || target->scripPin.gpio.busGpio == NULL)
    return 0;

  for (int i = 0; i < 16; i++) {
    if (target->scripPin.gpio.busGpio->enum_r_bit[i] != TOKEN_NC) {
      ReNode *pinNode =
          GlobalGPIO_REG->find(target->scripPin.gpio.busGpio->enum_r_bit[i]);
      if (pinNode != NULL)
        GlobalGPIO_REG->remove(&pinNode);
    }
  }

  freeOneGPIOVariable(&target);
  return 1;
}
/**
 * This function will add a new digital GPIO.
 * It could be used for IN,OUT,Ticker or InterrupIn.
 * @param enumPinName  Name of the pin using ENUM value
 * @param type         Type of the GPIO Interruptin, Ticker, or IN/OUT
 *
 * */
GlobalGP *addGPIOpin_DIGITAL(int enumPinName, int type) {
  GlobalGP *result, *gp_scan;
  result = gp_scan = NULL;
  bool foundDigital = false;
  switch (type) {
  case GPIO_TICKER: {
    GlobalGPIO_REG->add(enumPinName, FTOKEN_TICKER);
    foundDigital = true;
    break;
  }
  case GPIO_PIN_IN:
  case GPIO_PIN_OUT: {
    GlobalGPIO_REG->add(enumPinName, FTOKEN_DIGITAL_IN_OUT);
    foundDigital = true;
    break;
  }
  case GPIO_INTERRUPTIN: {
    GlobalGPIO_REG->add(current_token_word, FTOKEN_INTERRUPT_IN);
    break;
  }
  default: {
    // Error
    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_INOUT);
    quitScriptProgram(1);
    return NULL;
  } break;
  }

  result = findGPIOpin(enumPinName);
  if (result != NULL && foundDigital) {
    // Here we have the pin name, and it should be digital out/in or ticker
    if (!(result->scripPin.tag == GPIO_PIN_OUT ||
          result->scripPin.tag == GPIO_TICKER ||
          result->scripPin.tag == GPIO_PIN_IN)) {
      // We have a wrong type of pin. Not digital - remove it totally
      freeOneGPIOVariable(&result);
      result = NULL;
    }
  } else if (result != NULL && !(foundDigital)) {
    // we have the pin, it must be Interrupt .. but we should check if it is
    // correct.
    if (result->scripPin.tag != GPIO_INTERRUPTIN) {
      // We have wrong item.
      freeOneGPIOVariable(&result);
      result = NULL;
    }
  }

  if (result == NULL) {
    /* We have a new GPIO pin that is not in the global variable.
    We need to save it in the list*/

    gp_scan = globalGPIOvariable;
    if (gp_scan != NULL) {
      while (gp_scan->next != NULL)
        gp_scan = gp_scan->next; // Find the last item
      // Here GB is not NULL, add a new item and fix the link. result will still
      // keep the added item.
      gp_scan->next = new GlobalGP();
      if (gp_scan->next == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_INOUT);
        quitScriptProgram(1);
        return NULL;
      }
      gp_scan->next->prev = gp_scan;
      gp_scan->next->next = NULL;
      gp_scan = gp_scan->next;
      result = gp_scan;
    } else {
      // Here, Global variable is empty. add new item.
      gp_scan = new GlobalGP;
      if (gp_scan == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_INOUT);
        quitScriptProgram(1);
        return NULL;
      }
      gp_scan->prev = NULL;
      gp_scan->next = NULL;
      result = gp_scan;
      globalGPIOvariable = result;
    }
    result->scripPin.gpio.busGpio = NULL; // We don't have a bus
    result->scripPin.gpio.OnePinGPIO = NULL;
    result->scripPin.gpio.OnePinGPIO = new pinGPIO();
    if (result->scripPin.gpio.OnePinGPIO == NULL) {
      DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_PIN);
      quitScriptProgram(1);
      return NULL;
    }
    result->scripPin.gpio.OnePinGPIO->pinNameEnum = enumPinName;
    result->scripPin.tag = type;
    if (type != GPIO_INTERRUPTIN) {
      result->scripPin.gpio.OnePinGPIO->genInOut.InOutPin =
          new DigitalInOut(retrievePinNameFromTOKEN_NAME(enumPinName));
      if (result->scripPin.gpio.OnePinGPIO->genInOut.InOutPin == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_INOUT);
        quitScriptProgram(1);
        return NULL;
      }
    } else {
      result->scripPin.gpio.OnePinGPIO->genInOut.interrupt_in =
          new InterruptIn(retrievePinNameFromTOKEN_NAME(enumPinName));
      if (result->scripPin.gpio.OnePinGPIO->genInOut.interrupt_in == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_INOUT);
        quitScriptProgram(1);
        return NULL;
      }
    }
  }
  // If we find the GPIO in the variable, we don't' need to do anything. just
  // change the value.
  if (type == GPIO_PIN_OUT || type == GPIO_TICKER)
    result->scripPin.gpio.OnePinGPIO->genInOut.InOutPin
        ->output(); // Configure the pin as Output
  else if (type == GPIO_PIN_IN) {
    result->scripPin.gpio.OnePinGPIO->genInOut.InOutPin
        ->input(); // Configure the pin as Output
  }
  return result;
}
/** Find a specific defined gpio name in the list
 * @param enumPinName GPIO pin name using enum value
 * */
GlobalGP *findGPIOpin(int enumPinName) {
  // Stop running if we have error
  STOP_CODE_EXECUTION_NULL

  int SearchForPinName;
  if (enumPinName == -1) {
    SearchForPinName = current_token_word;
  } else {
    SearchForPinName = enumPinName;
  }
  int found = 1;
  GlobalGP *p_scan = globalGPIOvariable;
  if (p_scan == NULL)
    return NULL;
  while (p_scan->next != NULL)
    p_scan = p_scan->next; // find the first item.

  while (p_scan->scripPin.gpio.OnePinGPIO->pinNameEnum != SearchForPinName) {
    if (p_scan->prev == NULL) {
      found = 0;
      break;
    }
    p_scan = p_scan->prev;
  }
  if (found == 1)
    return p_scan;
  else
    return NULL; // not found.
}

/** put a digital data on GPIO
 * Syntax of the instruction is like follow:
 *   put NAME,value
 *       NAME:  could be a collection of GPIO (bus) up to 16 no.
 *              or represent one GPIO pin defined in tokens.h
 *       Value: 1, 0 or if the NAME is a bus .. the value depends on the bus
 * 0b...n bits.
 */
int token_put(void) {
  // Stop running if we have error
  STOP_CODE_EXECUTION_MINUS

  DEBUG_MESSAGE("INFO %i\n", DBG_MSG_PUT_START);
  int value = 0;
  process_token(TOKEN_PUT); // Get next token word.
  unsigned char checkISReg = 0;
  GlobalGP *p_scan = NULL;
  switch (current_token_word) {
    // Here we have a name which is a bus name.
  case TOKEN_VARIABLE: {
    p_scan = findReg(NULL);
    if (p_scan == NULL) {
      DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_BUS_NOT_FOUND);
      return quitScriptProgram(1);
    }
    process_token(TOKEN_VARIABLE);
    checkISReg = 1;
    break;
  }
  default: {
    // one GPIO
    p_scan = addGPIOpin_DIGITAL(current_token_word, GPIO_PIN_OUT);
    if (p_scan == NULL) {
      // We have a bad instruction.
      DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_PUT_REG);
      return quitScriptProgram(1);
    }
    process_token(current_token_word);
    checkISReg = 0;
    break;
  }
  }
  if (current_token_word == TOKEN_COMMA)
    process_token(TOKEN_COMMA);

  // Get the value to be sent to the GPIO
  if (current_token_word == TOKEN_NUMBER) {
    value = convert_text_to_number();
    process_token(TOKEN_NUMBER);
  } else if (current_token_word == TOKEN_VARIABLE) {
    value =
        RetrieveValueInGlobalVariable(); // retrieve the variable value (could
                                         // be an element of an array).
  } else {
    // We have a bad instruction.
    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_PUT_REG);
    return quitScriptProgram(1);
  }
  // We have a register. We need to find out the pins
  if (checkISReg != 0) {
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_REG_PUT);
    *p_scan->scripPin.gpio.busGpio->bus = value;
  } else {
    // We don't have a reg, but it is a GPIO.
    p_scan->scripPin.gpio.OnePinGPIO->genInOut.InOutPin->write(value);
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_PUT_END);
  }
  return 0; // Check if this correct
}

/**Get digital input data. You must provide the instruction a variable name
 * Syntax of the instruction is like follow:
 *   get REGNAME,variable
 *   get GPIO, variable
 *   While REGNAME   could be a collection of GPIO defines with create_reg
 *                   or Represent one GPIO pins defined in tokens.h
 *  variable         value will be saved in the variable.
 */
int token_get(void) {
  // Stop running if we have error
  STOP_CODE_EXECUTION_MINUS

  DEBUG_MESSAGE("INFO %i\n", DBG_MSG_GET_INST_START);
  GlobalGP *p_scan = NULL;
  GlobalGP *gp_scan = NULL;
  int varPosition = -1;
  char *varTestBuff = NULL;
  unsigned char checkISReg = 0;
  int value = 0;
  process_token(TOKEN_GET); // Get next token word.
  switch (current_token_word) {
  case TOKEN_VARIABLE: {
    // This is a bus.
    p_scan = findReg(NULL);
    if (p_scan == NULL) {
      DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_BUS_NOT_FOUND);
      return quitScriptProgram(1);
    }
    process_token(TOKEN_VARIABLE);
    p_scan->scripPin.gpio.busGpio->configBus(GPIO_BUS_IN);
    p_scan->scripPin.gpio.busGpio->bus->input();
    checkISReg = 1;
    break;
  }
  default: {
    GlobalGPIO_REG->add(current_token_word, FTOKEN_DIGITAL_IN_OUT);
    p_scan = findGPIOpin(-1);
    if (p_scan == NULL) {
      /* We have a new GPIO pin that is not in the global variable.
       We need to save it in the list*/

      gp_scan = globalGPIOvariable;
      if (gp_scan != NULL) {
        while (gp_scan->next != NULL)
          gp_scan = gp_scan->next; // Find the last item
        gp_scan->next = new GlobalGP;
        if (gp_scan->next == NULL) {
          DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
          return quitScriptProgram(1);
        }

        gp_scan->next->prev = gp_scan;
        gp_scan->next->next = NULL;
        p_scan = gp_scan->next;
      } else {
        gp_scan = new GlobalGP;
        if (gp_scan == NULL) {
          DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
          return quitScriptProgram(1);
        }

        gp_scan->next = NULL;
        gp_scan->prev = NULL;
        p_scan = gp_scan;
        globalGPIOvariable = p_scan;
      }
      p_scan->scripPin.tag = GPIO_PIN_IN;
      p_scan->scripPin.gpio.busGpio = NULL;
      p_scan->scripPin.gpio.OnePinGPIO = new pinGPIO();
      if (p_scan->scripPin.gpio.OnePinGPIO == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_PIN);
        return quitScriptProgram(1);
      }
      // GlobalGPIO_REG->add(current_token_word,FTOKEN_DIGITAL_IN_OUT);
      p_scan->scripPin.gpio.OnePinGPIO->genInOut.InOutPin =
          new DigitalInOut(retrievePinNameFromTOKEN_NAME(current_token_word));
      if (p_scan->scripPin.gpio.OnePinGPIO->genInOut.InOutPin == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_INOUT);
        return quitScriptProgram(1);
      }
      p_scan->scripPin.gpio.OnePinGPIO->pinNameEnum = current_token_word;
      p_scan->scripPin.gpio.OnePinGPIO->genInOut.InOutPin
          ->input(); // Configure the pin as input
    }
    process_token(current_token_word);
    checkISReg = 0;
    break;
  }
  }
  //"2nd argument

  if (current_token_word == TOKEN_COMMA) {
    process_token(TOKEN_COMMA);
  }
  //  find the variable in the global variables
  if (current_token_word == TOKEN_VARIABLE) {
    int strLength = strlen(p_prog->txtString);
    varTestBuff = (char *)calloc(strLength + 1, sizeof(char));
    if (varTestBuff == NULL) {
      DEBUG_MESSAGE("FAILED TO ALLOCATE MEM varTestBuff\n");
      return quitScriptProgram(1);
    }

    strncpy(varTestBuff, p_prog->txtString,
            strLength); // We don't need to add '\0' as we uses calloc

    varPosition = TestSystem_get_variable(varTestBuff);
    // DEBUG_MESSAGE("VaribleName was %s\n",varTestBuff);
  } else {
    // We have a bad instruction.
    DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND_GET);
    return quitScriptProgram(1); // exit ..bad instruction.
  }

  // Read the value
  if (checkISReg == 1)
    value = p_scan->scripPin.gpio.busGpio->bus->read();
  else
    value = p_scan->scripPin.gpio.OnePinGPIO->genInOut.InOutPin->read();

  if (varPosition == -1) {
    // No variable found .. create it.
    varPosition = TestSystem_get_empty_variable_pos();
    if (varPosition < 0) {
      DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_ERROR_VAR_OVERFLOW);
      return quitScriptProgram(1);
    }
    addNewScriptVariable(varTestBuff, T_INT, 1, varPosition);
    varTestBuff = NULL;
    *script_variables[varPosition].Value.pi = (int)value;
  } else {
    if (script_variables[varPosition].tag == T_FLOAT) {
      script_variables[varPosition].tag = T_INT;
      if (script_variables[varPosition].Value.pf != NULL)
        delete script_variables[varPosition].Value.pf;
      script_variables[varPosition].Value.pf = NULL;
      script_variables[varPosition].Value.pi = new int[1];
      script_variables[varPosition].sizeOfArray = 1;
      if (script_variables[varPosition].Value.pi == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GET_INST);
        return quitScriptProgram(1);
      }
      *script_variables[varPosition].Value.pi = value;
    } else if (script_variables[varPosition].tag == T_INT) {
      *script_variables[varPosition].Value.pi = value;
    } else {
      if (script_variables[varPosition].tag == T_STRING) {
        script_variables[varPosition].tag = T_INT;
        if (script_variables[varPosition].Value.txtvalue != NULL)
          delete[] script_variables[varPosition].Value.txtvalue;

        script_variables[varPosition].Value.txtvalue = NULL;
        script_variables[varPosition].Value.pi = new int[1];
        script_variables[varPosition].sizeOfArray = 1;
        if (script_variables[varPosition].Value.pi == NULL) {
          DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GET_INST);
          return quitScriptProgram(1);
        }
        *script_variables[varPosition].Value.pi = value;
      }
    }
  }

  if (varTestBuff != NULL) {
    free(varTestBuff);
    varTestBuff = NULL;
  }
  process_token(TOKEN_VARIABLE);
  DEBUG_MESSAGE("INFO %i\n", DBG_MSG_GET_INST_END);
  return 0; // Check if this correct
}

/**
 * Create New bus
 * Bus will not be created if it is exist.
 * @param type : In or Out
 * */
int createNewBus(int type) {
  // Stop running if we have error
  STOP_CODE_EXECUTION_NULL

  /* You don't need to give any parameter to this function.
  If p_prog is located at reg name .. everything will be correct.
  */
  GlobalGP *p_scanGlobGPIO = NULL;
  p_scanGlobGPIO = globalGPIOvariable;
  GlobalGP *temp = NULL;

  temp = findReg(NULL);
  if (temp != NULL) {
    DEBUG_MESSAGE("INFO %i\n", DBG_MSG_BUS_ALREADY_DEFINED);
    if (temp->scripPin.tag != type) {
      // We have to change the type.
      temp->scripPin.gpio.busGpio->configBus(type);
      temp->scripPin.tag = type;
    }
    // Now we have to move to the end of the line.
    process_token(TOKEN_VARIABLE);
    token_jump_to(RetrieveEndOfInstruction());
  } else {
    int enumReg[16];
    for (int i = 0; i < 16; i++) {
      enumReg[i] = TOKEN_NC;
    }

    int stringLength = strlen(p_prog->txtString);
    char *nameBuf = NULL;

    nameBuf = new char[stringLength + 1]();
    if (nameBuf == NULL) {
      DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GLOBAL_GPIO);
      quitScriptProgram(1);
      return -1;
    }
    // snprintf(nameBuf, stringLength,"%s", p_prog->txtString);
    strncpy(nameBuf, p_prog->txtString,
            stringLength); // We don't need to add '\0' as we uses calloc

    process_token(TOKEN_VARIABLE);

    if (p_scanGlobGPIO == NULL) {
      p_scanGlobGPIO = new GlobalGP;
      if (p_scanGlobGPIO == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
        delete[] nameBuf;
        quitScriptProgram(1);
        return -1;
      }

      p_scanGlobGPIO->next = NULL;
      p_scanGlobGPIO->prev = NULL;
      globalGPIOvariable = p_scanGlobGPIO;
    } else {
      // forward wind to the end of the linked list.
      while (p_scanGlobGPIO->next != NULL)
        p_scanGlobGPIO = p_scanGlobGPIO->next;

      p_scanGlobGPIO->next = new GlobalGP;
      if (p_scanGlobGPIO->next == NULL) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
        delete[] nameBuf;
        quitScriptProgram(1);
        return -1;
      }

      (p_scanGlobGPIO->next)->next = NULL;
      (p_scanGlobGPIO->next)->prev = p_scanGlobGPIO;
      p_scanGlobGPIO = p_scanGlobGPIO->next;
    }
    instr_linkedList *scanProg = NULL;
    scanProg = RetrieveEndOfInstruction();
    unsigned char index = 0;
    while (p_prog->index < scanProg->index) {
      if (current_token_word == TOKEN_COMMA)
        process_token(TOKEN_COMMA);
      if (index >= 16) {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_GPIO_NOT_FOUND);
        quitScriptProgram(1);
        return -1;
      }
      if (retrievePinNameFromTOKEN_NAME(current_token_word) != NC) {
        enumReg[index] = current_token_word;
        process_token(current_token_word);
      } else {
        // MUST BE AN ERROR
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_GPIO_NOT_FOUND);
        freeOneGPIOVariable(&p_scanGlobGPIO);
        delete[] nameBuf;
        quitScriptProgram(1);
        return -1;
      }
      index++;
    }
    if (index < 2) {
      DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_GPIO_NOT_FOUND);
      freeOneGPIOVariable(&p_scanGlobGPIO);
      delete[] nameBuf;
      quitScriptProgram(1);
      return -1;
    }

    p_scanGlobGPIO->scripPin.tag = type;
    p_scanGlobGPIO->scripPin.gpio.OnePinGPIO = NULL;
    p_scanGlobGPIO->scripPin.gpio.busGpio = new reg(nameBuf, type, enumReg);
    if (p_scanGlobGPIO->scripPin.gpio.busGpio == NULL ||
        p_scanGlobGPIO->scripPin.gpio.busGpio->bus == NULL) {
      DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR_GPIO_BUS);
      freeOneGPIOVariable(&p_scanGlobGPIO);
      delete[] nameBuf;
      quitScriptProgram(1);
      return -1;
    }
    nameBuf = NULL; // The location will be cleared by the destructor
  }
  return 1;
}

/** Create register for keeping several pins as a port
 * Syntax of the instruction is like follow:
 *  create_reg REGNAME BIT15....,BIT6,BIT5,BIT4,BIT3,BIT2,BIT1,BIT0
 *  You can choose from min 2 to max 16 pins ..
 *  While BIT15 ...BIT0 represent the GPIO pins defined in this file
 *  REGNAME: is the bus name creates by the instruction.
 *            Can be used later for sending/getting data
 */
int token_create_reg(void) {
  // Stop running if we have error
  STOP_CODE_EXECUTION_MINUS

  DEBUG_MESSAGE("INFO %i\n", DBG_MSG_CREATE_REG_START);
  process_token(TOKEN_CREATE_REG); // Get next statement
  createNewBus(GPIO_BUS_OUT);      // Default creation is output.
  DEBUG_MESSAGE("INFO %i\n", DBG_MSG_CREATE_REG_END);
  return 1;
}
