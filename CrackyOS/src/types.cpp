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
#include "types.h"
#include "hardware_used.h"
#include "ticker_hw.h"
RegisterGPIO* GlobalGPIO_REG = NULL;
ReNode::ReNode() :_enumName(0),
_Functionality(0),
prev(NULL),
next(NULL)
{
}
ReNode::~ReNode()
{
}

RegisterGPIO::RegisterGPIO() :_pNodeItem(NULL) {
}
RegisterGPIO::~RegisterGPIO()
{
    goToFirstItem();
    if (_pNodeItem == NULL) return; //Nothing to do
    while (_pNodeItem->next != NULL) {
        _pNodeItem = _pNodeItem->next;
        delete _pNodeItem->prev;
        _pNodeItem->prev = NULL;
    }
    delete _pNodeItem;
    _pNodeItem = NULL;
}
/**
 * This function will free up the memory for the GPIO used before if it doesn't have the same functionality.
 * SERIAL, SPI MASTER and REG is not allowed
 * Return 1 if removed
 * Return 0 if not removed .. It has the same functionality
 * Return -1 if not found.
 *
 * */
int RegisterGPIO::gpioFreeMem(int enumName, int function) {
    /**
     *  Remove the item if the functionality changes.
     *  otherwise return 0
     *
     */
     //Stop running if we have error
    STOP_CODE_EXECUTION_NULL

        int oldFunctionality = 0;
    ReNode* regGPIO_linkedListItem = NULL;
    regGPIO_linkedListItem = find(enumName);    //Find the node from the RegisterGPIO
    if (regGPIO_linkedListItem == NULL) {
        return -1; //Nothing to be done here. The pinname is not registered here and not used.
    }
    GlobalGP* p_FreeGIPscan = NULL;

    oldFunctionality = regGPIO_linkedListItem->_Functionality;
    if (oldFunctionality == function)
        return 0;
    if (oldFunctionality == FTOKEN_TICKER && function != FTOKEN_TICKER) {
        FreeUp_Tickers(enumName);
        remove(&regGPIO_linkedListItem);
        return 1;
    }
    //Functionality changed .. remove the item from the registered list.
    remove(&regGPIO_linkedListItem);
    regGPIO_linkedListItem = NULL;
    //Depending on the old functionality
    switch (oldFunctionality) {
    case FTOKEN_TICKER:                         //As it is an GPII_PIN OUT ..
    case FTOKEN_DIGITAL_IN_OUT: {
        //Find the GPIO and delete it in both register and global GPIO
        p_FreeGIPscan = findGPIOpin(enumName);
        if (p_FreeGIPscan == NULL)
            return -1; // Nothing to be done here, pin not found.
        //TODO : This must be an error .. DO WE NEED TO STOP THE PROGRAM? 2018-12-18
    }
                              if (p_FreeGIPscan->scripPin.tag == GPIO_PIN_IN
                                  || p_FreeGIPscan->scripPin.tag == GPIO_PIN_OUT
                                  || p_FreeGIPscan->scripPin.tag == GPIO_IN_ANALOG
                                  || p_FreeGIPscan->scripPin.tag == GPIO_OUT_ANALOG
                                  || p_FreeGIPscan->scripPin.tag == GPIO_TICKER) {
                                  if (p_FreeGIPscan->next == NULL && p_FreeGIPscan->prev == NULL) {
                                      //Only one item
                                      globalGPIOvariable = NULL;
                                  }
                                  else if (p_FreeGIPscan->next == NULL) {
                                      //Last item
                                      p_FreeGIPscan->prev->next = NULL;
                                  }
                                  else if (p_FreeGIPscan->prev == NULL) {
                                      //First item
                                      p_FreeGIPscan->next->prev = NULL;
                                  }
                                  else {
                                      //In between items.
                                      p_FreeGIPscan->next->prev = p_FreeGIPscan->prev;
                                      p_FreeGIPscan->prev->next = p_FreeGIPscan->next;
                                  }
                                  delete p_FreeGIPscan;
                                  return 1;
                              }
                              else {
                                  DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_IF_UNKNOWN);
                                  quitScriptProgram(1); //Exit the program.
                              }

                              break;

    case FTOKEN_SPI_CS:
    case FTOKEN_SPI_MOSI:
    case FTOKEN_SPI_MISO:
    case FTOKEN_SPI_CLK: {
        /**
         * WE DON'T ALLOW SERIAL PINS TO BE REASSIGNED .. THIS IS A BAD CODE
         * */

        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SERIAL_SPI_CANNOT_REASIGNS);
        quitScriptProgram(1); //Exit the program.
    }
                       break;
    case FTOKEN_SERIAL_TX:
    case FTOKEN_SERIAL_RX: {
        /**
         * WE DON'T ALLOW SERIAL PINS TO BE REASSIGNED .. THIS IS A BAD CODE
         * */

        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_SERIAL_PIN_CANNOT_REASIGNS);
        quitScriptProgram(1); //Exit the program.
    }
                         break;
    case FTOKEN_REG: {
        /**
         * WE DON'T ALLOW REG PINS TO BE REASSIGNED .. THIS IS A BAD CODE
         * */
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_REG_PIN_CANNOT_REASIGNS);
        quitScriptProgram(1); //Exit the program.
    }
                   break;
    case FTOKEN_CAN_RD:
    case FTOKEN_CAN_TD: {
        /**
         * Be aware that you cannot only remove the RD or the TD ..You have to free both.
         * Programmer should be careful when they reassign the code.
         * */
        canType* pScan = gCanVariable;
        canType* tpScan;
        ReNode *_tmp=0;
        if (pScan == 0)
            return -1; //Nothing to do here
        while (pScan->next != NULL) {
            if ((pScan->current.CANRD == enumName || pScan->current.CANTD == enumName)) {
                if (pScan->next == NULL && pScan->prev == NULL) {
                    /*We have only one CAN..remove it*/
                	_tmp=find(pScan->current.CANRD);
                    remove(&_tmp);
                	_tmp=find(pScan->current.CANTD);
                    remove(&_tmp);
                    gCanVariable = NULL;
                    delete pScan;
                    return 1;   //We are done.
                }
                /* We have can in the middle position*/
                if (pScan->prev != NULL && pScan->next != NULL) {
                    tpScan = pScan;
                    pScan->prev->next = tpScan->next;
                    pScan->next->prev = tpScan->prev;
                	_tmp=find(pScan->current.CANRD);
                    remove(&_tmp);
                	_tmp=find(pScan->current.CANTD);
                    remove(&_tmp);
                    delete tpScan;
                    return 1;
                }
                if (pScan->prev != NULL && pScan->next == NULL) {
                    /* We should have a last item*/
                    tpScan = pScan;
                    pScan = pScan->prev;
                    pScan->next = NULL;
                	_tmp=find(pScan->current.CANRD);
                    remove(&_tmp);
                	_tmp=find(pScan->current.CANTD);
                    remove(&_tmp);
                    delete tpScan;
                    return 1;
                }
                if (pScan->prev == NULL && pScan->next != NULL) {
                    /* We should have the 1st item*/
                    tpScan = pScan;
                    pScan = pScan->next;
                    pScan->prev = NULL;
                	_tmp=find(pScan->current.CANRD);
                    remove(&_tmp);
                	_tmp=find(pScan->current.CANTD);
                    remove(&_tmp);
                    delete tpScan;
                    return 1;
                }
            }
            pScan = pScan->next;
        }
        return -1;
    }

    case FTOKEN_PWM: {
        if (enumName == PWM_Var.pwm1.enum_pinname) {
            /* we need to remove the PWM and the list*/
            PWM_Var.pwm1.enum_pinname = TOKEN_NC;
            delete PWM_Var.pwm1.pwm;
            PWM_Var.pwm1.pwm = NULL;
            return 1;
        }

        if (enumName == PWM_Var.pwm2.enum_pinname) {
            /* we need to remove the PWM and the list*/
            PWM_Var.pwm2.enum_pinname = TOKEN_NC;
            delete PWM_Var.pwm2.pwm;
            PWM_Var.pwm2.pwm = NULL;
            return 1;
        }

        if (enumName == PWM_Var.pwm3.enum_pinname) {
            /* we need to remove the PWM and the list*/
            PWM_Var.pwm3.enum_pinname = TOKEN_NC;
            delete PWM_Var.pwm3.pwm;
            PWM_Var.pwm3.pwm = NULL;
            return 1;
        }

        if (enumName == PWM_Var.pwm4.enum_pinname) {
            /* we need to remove the PWM and the list*/
            PWM_Var.pwm4.enum_pinname = TOKEN_NC;
            delete PWM_Var.pwm4.pwm;
            PWM_Var.pwm4.pwm = NULL;
            return 1;
        }
    }
                   break;
    }
    return 0;
}

/**
 * Point back to the first item in the linked list.
 * */
void RegisterGPIO::goToFirstItem(void) {
    if (_pNodeItem != NULL) {
        while (_pNodeItem->prev != NULL)
            _pNodeItem = _pNodeItem->prev;
    }
}
/**
 * Add a new item to the list of GPIO
 * return 1 if added
 * return 0 if not added
 * */

int RegisterGPIO::add(int enumName, int function) {
    //Stop running if we have error
    STOP_CODE_EXECUTION_MINUS

        int result;
    //Free memory allocated for the GPIO in the Global GPIO listed link(not the REG listed link)
    result = gpioFreeMem(enumName, function);
    if (result == -2)
        return -1;
    if (result == 0) {
        //don't add it is the same.
        return 0; //Not added
    }
    if (_pNodeItem == NULL) {
        //We have no items here .. add just.
        _pNodeItem = new (ReNode);
        if (_pNodeItem == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
            quitScriptProgram(1);
        }
        _pNodeItem->next = NULL;
        _pNodeItem->prev = NULL;
        _pNodeItem->_enumName = enumName;
        _pNodeItem->_Functionality = function;
    }
    else {
        goToFirstItem();
        while (_pNodeItem->next != NULL)
            _pNodeItem = _pNodeItem->next;

        _pNodeItem->next = new (ReNode);
        if (_pNodeItem->next == NULL) {
            DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_MEM_ALLOCATION_ERROR);
            quitScriptProgram(1);
        }
        _pNodeItem->next->prev = _pNodeItem;
        _pNodeItem->next->next = NULL;
        _pNodeItem->next->_enumName = enumName;
        _pNodeItem->next->_Functionality = function;
    }
    return 1;
}
/**
 * Find a GPIO in the linked list and return a pointer to the item.
 * @param enumName Name of the GPIO to find
 * @return A pointer to the found item or NULL
 * */
ReNode* RegisterGPIO::find(int enumName) {
    //Stop running if we have error
    STOP_CODE_EXECUTION_NULL

        goToFirstItem();
    ReNode* _tempItem = _pNodeItem;
    if (_tempItem == NULL) {
        //NOT FOUND .. NO ITEMS IN THE LISET
        return 0;
    }
    while (1) {
        if (_tempItem->_enumName == enumName) {
            return _tempItem;
        }
        else {
            if (_tempItem->next != NULL)
                _tempItem = _tempItem->next;
            else
                return 0;
        }
    }
}
/**
 * Remove a GPIO from the linked list
 * @param t A node
 * */
    void RegisterGPIO::remove(ReNode** t) {
    //Stop running if we have error
    STOP_CODE_EXECUTION

        if (*t == NULL) {
            return; //Nothing to do
        }

    if ((*t)->next == NULL && (*t)->prev == NULL) {
        //First Item and we have only one item.
        delete (*t);
        *t = NULL;
        _pNodeItem = NULL;
    }
    else if ((*t)->next != NULL && (*t)->prev == NULL) {
        //First item but we have more items linked to it
        (*t)->next->prev = NULL;
        _pNodeItem = (*t)->next;
        delete *t;
        *t = NULL;
    }
    else if ((*t)->next == NULL && (*t)->prev != NULL) {
        //Last item
        (*t)->prev->next = NULL;
        _pNodeItem = (*t)->prev;
        delete(*t);
        *t = NULL;
    }
    else if ((*t)->next != NULL && (*t)->prev != NULL) {
        (*t)->next->prev = (*t)->prev;
        (*t)->prev->next = (*t)->next;
        _pNodeItem = (*t)->prev;
        delete *t;
        *t = NULL;
    }
    else {
        //We shoulnd't be here
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_LISTED_LINK_NOT_FOUND);
        quitScriptProgram(1); //Exit the program.
    }
}

/** Class definition for GPOI inside our script.
 *
 * */
GlobalGPclass::GlobalGPclass() {
    tag = GPIO_NC;
    gpio.OnePinGPIO = NULL;
    gpio.busGpio = NULL;
    fun = NULL;
}

/**
 * GlobalGPclass Class destructor
 * */
GlobalGPclass::~GlobalGPclass() {
    //todo: check what we should do here.
}


/**
 * Remove only one variable from MEM.
 * @param target: GPIO name.
 * */
void freeOneGPIOVariable(GlobalGP** target) {
    if (target == NULL)
    {
        DEBUG_MESSAGE("ERROR %i\n", DBG_MSG_SYNTAX_ERROR_VAR_NOT_FOUND);
        return; //Nothing to do
    }
    if ((*target)->next == NULL && (*target)->prev == NULL) {
        //We have only one item.
        globalGPIOvariable = NULL;
    }
    else if ((*target)->next == NULL && (*target)->prev != NULL) {
        //Last item
        (*target)->prev->next = NULL;
    }
    else if ((*target)->prev == NULL && (*target)->next != NULL) {
        //First Item
        (*target)->next->prev = NULL;
        globalGPIOvariable = (*target)->next;
    }
    else {
        //In the middle.
        (*target)->prev->next = (*target)->next;
        (*target)->next->prev = (*target)->prev;
    }
    switch ((*target)->scripPin.tag) {
    case GPIO_PIN_IN:
    case GPIO_PIN_OUT:
    case GPIO_TICKER:
    {
        if ((*target)->scripPin.gpio.OnePinGPIO->genInOut.InOutPin != NULL) {
            delete ((*target)->scripPin.gpio.OnePinGPIO->genInOut.InOutPin);
            (*target)->scripPin.gpio.OnePinGPIO->genInOut.InOutPin = NULL;
        }
        break; }
    case GPIO_IN_ANALOG: {
        if ((*target)->scripPin.gpio.OnePinGPIO->genInOut.aInPin != NULL) {
            delete ((*target)->scripPin.gpio.OnePinGPIO->genInOut.aInPin);
            (*target)->scripPin.gpio.OnePinGPIO->genInOut.aInPin = NULL;
        }
        break; }
    case GPIO_OUT_ANALOG: {
        if ((*target)->scripPin.gpio.OnePinGPIO->genInOut.aOutPin != NULL) {
            delete ((*target)->scripPin.gpio.OnePinGPIO->genInOut.aOutPin);
            (*target)->scripPin.gpio.OnePinGPIO->genInOut.aOutPin = NULL;
        }
        break; }
    case GPIO_BUS_IN:
    case GPIO_BUS_OUT: {
        if ((*target)->scripPin.gpio.busGpio != NULL) {
            delete ((*target)->scripPin.gpio.busGpio);
            (*target)->scripPin.gpio.busGpio = NULL;
        }
        break; }
    }
    delete(*target);
    *target = NULL;
    return;
}
