/*
 * mistring.h
 *
 *  Created on: 4/12/2015
 *      Author: Felipe
 */

#ifndef MISTRING_H_
#define MISTRING_H_

#include "chip.h"

uint8_t strEqual(char *a, char *b);
int32_t strToInt(char a[]);
void intToStr(char str[], uint16_t num);

#endif /* MISTRING_H_ */
