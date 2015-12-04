/*
 * miadc.c
 *
 *  Created on: 4/12/2015
 *      Author: Felipe
 */

#include "miadc.h"

/**
 * Funcion para iniciar Mi Adc.
 */
void NewMiadc(){
   ADC_CLOCK_SETUP_T ADCSetup;
   Chip_ADC_Init(MI_ADC_ID,&ADCSetup);
   Chip_ADC_SetBurstCmd(MI_ADC_ID, DISABLE);
}

/**
 * Funcion para leer el adc dependiendo del canal.
 * uint8_t canal: Numero del canal del adc que se lee.
 * Retorna uint16_t: valor del adc segun canal.
 */
uint16_t leerMiadc(uint8_t canal){
   uint16_t adcVal=0;
   Chip_ADC_EnableChannel(MI_ADC_ID,canal,ENABLE);
   Chip_ADC_SetStartMode(MI_ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

   while((Chip_ADC_ReadStatus(MI_ADC_ID , canal , ADC_DR_DONE_STAT) != SET));
   Chip_ADC_ReadValue(MI_ADC_ID , canal, &adcVal);

   Chip_ADC_EnableChannel(MI_ADC_ID,canal,DISABLE);
   return adcVal;
}
