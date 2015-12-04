/*
 * miuart.h
 *
 *  Created on: 4/12/2015
 *      Author: Felipe
 */

#ifndef MIUART_H_
#define MIUART_H_

#define MI_UART LPC_USART2
#define MI_BAUDIOS 115200

#include "chip.h"

#define FIN_LINEA '\r'

struct BufferIn{
   int16_t buffer[64];
   uint8_t len;
};


void NewMiuart(void);
void miuartPrintLn(char *str);
void miuartPrint(char *str);
void miuartTxChr(char str);
int16_t miuartRxChr(void);
uint8_t miuartRxLn(struct BufferIn *buffer);

//Buffer
void BufferToStr(struct BufferIn *buffer,char *str);

#endif /* MIUART_H_ */
