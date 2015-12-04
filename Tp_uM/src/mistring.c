/*
 * mistring.c
 *
 *  Created on: 4/12/2015
 *      Author: Felipe
 */

#include "mistring.h"

/**
 * Funcion para verificar si dos cadenas de caracteres son iguales.
 * Retorna uint8_t: 1 si las dos cadenas son iguales.
 */
uint8_t strEqual(char *a, char *b) {
   while (*a == *b) {
      if (*a == '\0' || *b == '\0')
         break;
      a++;
      b++;
   }

   if (*a == '\0' && *b == '\0')
      return 1;
   else
      return 0;
}

/**
 * Funcion para convertir una cadena de caracteres en un entero.
 * char a[]: Cadena de caracteres.
 * Retorna int32_t: entero convertido.
 */
int32_t strToInt(char a[]) {
   int32_t c, sign, offset, n;
   if (a[0] == '-') {
      sign = -1;
   }
   if (sign == -1) {
      offset = 1;
   } else {
      offset = 0;
   }
   n = 0;
   for (c = offset; a[c] != '\0'; c++) {
      n = n * 10 + a[c] - '0';
   }
   if (sign == -1) {
      n = -n;
   }
   return n;
}

/**
 * Funcion para convertir un entero en una cadena de caracteres.
 * char str[]: Cadena de caracteres donde se almacena el entero convertido.
 * uint16_t num: Entero que ha convertir en cadena de caracteres.
 */
void intToStr(char str[], uint16_t num) {
   uint16_t i, rem, len = 0, n;
   n = num;
   while (n != 0) {
      len++;
      n /= 10;
   }
   if (len == 0) {
      str[0] = '0';
      len++;
   }
   for (i = 0; i < len; i++) {
      rem = num % 10;
      num = num / 10;
      str[len - (i + 1)] = rem + '0';
   }
   str[len] = '\0';
}
