/*
 * miadc.h
 *
 *  Created on: 4/12/2015
 *      Author: Felipe
 */

#ifndef MIADC_H_
#define MIADC_H_

#include "chip.h"

#define MI_ADC_ID LPC_ADC0

void NewMiadc();
uint16_t leerMiadc(uint8_t canal);

#endif /* MIADC_H_ */
