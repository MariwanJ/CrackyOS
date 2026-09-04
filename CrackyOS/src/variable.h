/**
 ******************************************************************************
 * Variable definition (inside the script program)instruction
 * @author  Mariwan Jalal
 * @version V0.0.1
 * @date
 * @brief  This function will take care of the variables and the rest of the
 *line. Note: All variables are global. You can change the nature of the
 *variable at any time from text to int or to float vice versa
 ********************************************************************************
 */

#ifndef SRC_VARIABLE_H_
#define SRC_VARIABLE_H_

int inline token_plusminus(const int &varPosition, int &);
int inline toke_leftSquareBracket(int &varPosition, int &newVariable);
int inline token_equial(int &varPosition, int &);

/**We have an var[...]
 * either it is var[] = nr
 * or var[]={nr1,nr2,..etc}
 * but both the left side is without number.
 * */
int inline toke_leftSquareBracket_token_rightSquareBracket(int &varPosition,
                                                           int &newVariable);

/**We have an var[nr]
 * either it is var[nr] = nr
 * or var[nr]=nr1
 * but both the left side is without number.
 * */
int inline toke_leftSquareBracket_token_Number(int &varPosition,
                                               const int &newVariable);

/**
 * User entered the variable as follow
 * var[]=nr1
 * */
int inline newOneNumberSquareBracket(int &varPosition, const int &newVariable,
                                     const int &DotFound);

/**
 * User entered the variable as follow
 * var[]={nr1,n2, ..etc}
 * */
int inline newMultipleNumberSquareBracket(int &varPosition,
                                          const int &newVariable,
                                          const int &DotFound,
                                          const int &countComma); // OK

int inline countCommaInAssignment(int &DotFound);
#endif /* SRC_VARIABLE_H_ */
