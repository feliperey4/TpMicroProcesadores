/*
 * miuart.c
 *
 *  Created on: 4/12/2015
 *      Author: Felipe
 */

#include "miuart.h"

/**
 * Funcion para incializar MiUart.
 */
void NewMiuart(void){

   if (MI_UART == LPC_USART0) {
      Chip_SCU_PinMuxSet(0x6, 4, (SCU_MODE_PULLDOWN | SCU_MODE_FUNC2)); /* P6.5 : UART0_TXD */
      Chip_SCU_PinMuxSet(0x6, 5,
            (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS
                  | SCU_MODE_FUNC2));/* P6.4 : UART0_RXD */
   } else if (MI_UART == LPC_UART1) {
      Chip_SCU_PinMuxSet(0x1, 13, (SCU_MODE_PULLDOWN | SCU_MODE_FUNC2)); /* P1.13 : UART1_TXD */
      Chip_SCU_PinMuxSet(0x1, 14,
            (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS
                  | SCU_MODE_FUNC2)); /* P1.14 : UART1_RX */
   } else if (MI_UART == LPC_USART2) {
      Chip_SCU_PinMux(7, 1, MD_PDN, FUNC6); /* P1.13 : UART1_TXD */
      Chip_SCU_PinMux(7, 2, MD_PLN | MD_EZI | MD_ZI, FUNC6); /* P1.14 : UART1_RX */
   }

   Chip_UART_Init(MI_UART);
   Chip_UART_SetBaud(MI_UART, MI_BAUDIOS);
   Chip_UART_ConfigData(MI_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);
   Chip_UART_TXEnable(MI_UART);
}

/**
 * Funcion para enviar un byte.
 * char str: byte o caracter para enviar.
 */
void miuartTxChr(char str){
   while ((Chip_UART_ReadLineStatus(MI_UART) & UART_LSR_THRE) == 0) {}
   Chip_UART_SendByte(MI_UART, (uint8_t) str);
}

/**
 * Funcion para enviar una cadena de caracteres.
 * char *str: Cadena de caracteres.
 */
void miuartPrint(char *str){
   while (*str != '\0') {
      miuartTxChr(*str++);
   }
}

/**
 * Funcion para enviar un candena de caracteres y la finaliza con un salto de linea.
 * char *str: Cadena de caracteres.
 */
void miuartPrintLn(char *str){
   miuartPrint(str);
   miuartPrint("\n\r");
}

/**
 * Funcion para recibir un byte o caracter.
 * Retorna int16_t: caracter recibido.
 */
int16_t miuartRxChr(){
   int16_t dato=-1;
   if (Chip_UART_ReadLineStatus(MI_UART) & UART_LSR_RDR) {
      dato= (int16_t) Chip_UART_ReadByte(MI_UART);
   }
   return dato;
}

/**
 * Funcion para recibir una cadena de caracteres con un salto de linea.
 * struct BufferIn *buffer: Buffer donde almacena la cadena.
 * Retorna uint8_t: 1 si la cadena fue recibida con un salto de linea.
 */
uint8_t miuartRxLn(struct BufferIn *buffer){
   uint8_t flag=0;
      while((buffer->buffer[buffer->len]=miuartRxChr())>-1){
         if(buffer->buffer[buffer->len]==FIN_LINEA){
            flag=1;
            break;
         }
         buffer->len++;
      }
      return flag;
}

/**
 * Funcion para convertir un buffer en una cadena de caracteres.
 * struct BufferIn *buffer: Buffer donde almacena la cadena.
 * char *str: cadena de caracteres de salida.
 */
void BufferToStr(struct BufferIn *buffer,char *str){
   uint16_t i;
   for(i=0;i<buffer->len+1;i++)
      *(str+i)='\0';
   for(i=0;i<buffer->len;i++){
      *(str+i)=buffer->buffer[i];
   }
}

